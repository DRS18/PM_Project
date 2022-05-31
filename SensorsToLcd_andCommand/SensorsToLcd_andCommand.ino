#include <IRremote.h>
#include <DHT.h>
#include <MQ2.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define DHTPIN 7
#define DHTTYPE DHT11

const int receiver = 11;
const int led = 9;
const int irLed = 3;
const int buttonPin = 4;

float hum, temp;

int displayShow = 0;
int airStatus = 0;

int Analog_Input = A0;
float lpg, co, smoke;
MQ2 mq2(Analog_Input);

DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2);

IRsend irsend;
IRrecv ir_receiver(receiver);         
decode_results results;

void blynkLED(int pin, int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(100);
    digitalWrite(pin, LOW);
    delay(20);
  }
}

void setup()   
{
  Serial.begin(9600);
  
  ir_receiver.enableIRIn(); 
  pinMode(led, OUTPUT);
//  pinMode(irLed, OUTPUT);
  pinMode(buttonPin, INPUT);

  // Initialize sensors
  dht.begin();
  mq2.begin();
 
  // Initialize lcd + turn on backlight
  lcd.backlight();
  lcd.init();
}


void loop() 
{
  int check = 0;
  
  getFromDHT();
  getFromMQ2();
  
  if (ir_receiver.decode(&results)) {
    Serial.println(results.value, HEX);
    translateIR();
    showResultsOnDisplay();
    check = 1;
    ir_receiver.resume(); 
    delay(200); 
  }

  if (digitalRead(buttonPin) == HIGH) {
    Serial.println("Butt pressed");
    if (displayShow == 0) {
      displayShow = 1;
    } else {
      displayShow = 0;
    }
    delay(500);
  }

  if (check == 0) {
    showResultsOnDisplay(); 
  }
}

void translateIR() 
{
  switch(results.value){

  case 0x9880:
  case 0x8880:
      Serial.println(" Change display "); 
      if (displayShow == 0 || displayShow == 2) {
        displayShow = 1;
      } else if (displayShow == 1) {
        displayShow = 0;
      }
      break;

  default: 
    Serial.println(" other button   ");
  }
}

void getFromDHT() {
  hum  = dht.readHumidity();
  temp = dht.readTemperature();

  if (temp > 30 && airStatus == 0) {
    irsend.sendJVC(0x9880, 16, 0);
    
    airStatus = 1;
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("AIR ON");
    Serial.println("AIR ON");
    
    delay(2000);
  } else if (temp < 30 && airStatus == 1) {
    irsend.sendJVC(0x8880, 16, 0);
    
    airStatus = 0;

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("AIR OFF");
    Serial.println("AIR OFF");
    
    delay(2000);
  }
  
  Serial.print("Humidity = ");
  Serial.println(hum);
  Serial.print("Temperature = ");
  Serial.println(temp);
}

void getFromMQ2() {
  lpg = mq2.readLPG();
  co = mq2.readCO();
  smoke = mq2.readSmoke();

  int tmp = displayShow;
  while (lpg > 0.10 || co > 0.10 || smoke > 0.10) {
    displayShow = 2;
    showResultsOnDisplay();
    
    blynkLED(led, 1);
    
    lpg = mq2.readLPG();
    co = mq2.readCO();
    smoke = mq2.readSmoke();
  }
  displayShow = tmp;

  Serial.print("lpg = ");
  Serial.println(lpg);
  Serial.print("co = ");
  Serial.println(co);
  Serial.print("smoke = ");
  Serial.println(smoke);
}

void showResultsOnDisplay() {
  lcd.clear();
  lcd.setCursor(0,0);

  if (displayShow == 0) {
    lcd.print("Humidity=");
    lcd.print(hum);
    lcd.print("%");
    
    lcd.setCursor(0,1);
    lcd.print("Tmp=");
    lcd.print(temp);
    lcd.print("Celsius");
  } else if (displayShow == 1) {
    lcd.print("LPG:");
    lcd.print(lpg);
    lcd.print(" CO:");
    lcd.print(co);
    
    lcd.setCursor(0,1);
    lcd.print("SMOKE:");
    lcd.print(smoke);
    lcd.print(" PPM");
  } else if (displayShow == 2) {
    lcd.print("FIRE!!!");
  }

  delay(3000);
}
