#define BLYNK_TEMPLATE_ID "XXXXXXXXXX"
#define BLYNK_DEVICE_NAME "XXXXXXXXXX"
#define BLYNK_FIRMWARE_VERSION        "0.1.0"
#define BLYNK_PRINT Serial
#define APP_DEBUG
#define USE_NODE_MCU_BOARD
#include "BlynkEdgent.h"
#include "DHT.h"
#define DHTPIN 14 //D5 pin of NodeMCU
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;
const int sensor_pin = A0;  // Connect Soil moisture analog sensor pin to A0 of NodeMCU
int pump = D6; // pump connection
float t,h;
void sendSensor()
{
t = dht.readTemperature();
h = dht.readHumidity();
Blynk.virtualWrite(V2, t);
Blynk.virtualWrite(V3, h);
}
void setup() {
  Serial.begin(9600); /* Define baud rate for serial communication */
  dht.begin();
  BlynkEdgent.begin();
  delay(500);
  timer.setInterval(1000L, sendSensor);
  pinMode(pump, OUTPUT);
}
void loop() {
  BlynkEdgent.run(); // Initiates Blynk
  timer.run();
  float soil_moisture;
  soil_moisture = ( 100.00 - ( (analogRead(sensor_pin)/1023.00) * 100.00 ) );
  Blynk.virtualWrite(V1,soil_moisture);
  Serial.print("Soil Moisture = ");
  Serial.print(soil_moisture);
  Serial.println("%");
  Serial.print("Temperature = ");
  Serial.println(t);
  Serial.print("Humidity = ");
  Serial.println(h);
  if (soil_moisture < 30)
  {
    digitalWrite(pump, HIGH);
    Serial.println("Water Pump On");
    Blynk.virtualWrite(V4, HIGH);
  }
  else
  {
    digitalWrite(pump, LOW);
    Serial.println("Water Pump Off");
    Blynk.virtualWrite(V4, LOW);
  }
}
