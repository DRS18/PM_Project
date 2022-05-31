//DHT11 Lib
#include <DHT.h>
#include <MQ2.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Arduino to NodeMCU Lib
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

//Initialise Arduino to NodeMCU (5=Rx & 6=Tx)
SoftwareSerial nodemcu(5, 6);

//Initialisation of DHT11 Sensor
#define DHTPIN 7
DHT dht(DHTPIN, DHT11);
float temp, hum;

// Initialize LCD
// LiquidCrystal_I2C lcd(0x27, 16, 2);

// Initialize gas sensor
int Analog_Input = A0;
int lpg, co, smoke;
//MQ2 mq2(Analog_Input);

void setup() {
  Serial.begin(9600);

  dht.begin();
//  mq2.begin();
  nodemcu.begin(9600);
  delay(1000);

  Serial.println("Program started");
}

void loop() {

  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& data = jsonBuffer.createObject();

  //Obtain Temp and Hum data
  dht11_func();
//  mq2_func();

  //Assign collected data to JSON Object
  data["humidity"] = hum;
  data["temperature"] = temp; 

  //Send data to NodeMCU
  data.printTo(nodemcu);
  jsonBuffer.clear();

  delay(2000);
}

void dht11_func() {
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  Serial.print("Humidity: ");
  Serial.println(hum);
  Serial.print("Temperature: ");
  Serial.println(temp);
}

void mq2_func() {
  lpg = mq2.readLPG();
  co = mq2.readCO();
  smoke = mq2.readSmoke();
  
  Serial.print("LPG: ");
  Serial.println(lpg);
  Serial.print("CO: ");
  Serial.println(co);
  Serial.print("Smoke: ");
  Serial.println(smoke);
}
