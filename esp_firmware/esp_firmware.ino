#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <BlynkSimpleEsp8266.h>

//D6 = Rx & D5 = Tx
SoftwareSerial nodemcu(D6, D5);

// BLYNK
char auth[] = "slEFXgjvf9REK0djPm5rQM3WewvfskCM";
char ssid[] = "C4";
char pass[] = "Dragos2000";
BlynkTimer timer;
#define BLYNK_PRINT Serial

void readFromUno() {
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& data = jsonBuffer.parseObject(nodemcu);

  if (data == JsonObject::invalid()) {
    //Serial.println("Invalid Json Object");
    jsonBuffer.clear();
    return;
  }

  Serial.println("JSON Object Recieved");
  Serial.print("Recieved Humidity:  ");
  float hum = data["humidity"];
  Serial.println(hum);
  Serial.print("Recieved Temperature:  ");
  float temp = data["temperature"];
  Serial.println(temp);
  Serial.println("-----------------------------------------");

  Blynk.virtualWrite(V5, hum);
  Blynk.virtualWrite(V6, temp);
}

void setup() {
  // Initialize Serial port
  Serial.begin(9600);
  nodemcu.begin(9600);
  while (!Serial) continue;

  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, readFromUno);
}

void loop() {
  Blynk.run();
  timer.run();
}
