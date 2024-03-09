/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-temperature-humidity-sensor-lcd
 */

#include <DHT.h>

#define DHT_SENSOR_PIN  23 // ESP32 pin GIOP23 connected to DHT11 sensor
#define DHT_SENSOR_TYPE DHT11

 // I2C address 0x3F, 16 column and 2 rows
DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

void setup() {
  dht_sensor.begin(); // initialize the DHT sensor

  Serial.begin(9600);
}

void loop() {
  float humi  = dht_sensor.readHumidity();    // read humidity
  float tempC = dht_sensor.readTemperature(); // read temperature


  // check whether the reading is successful or not
  if (isnan(tempC) || isnan(humi)) {
  
    Serial.print("Failed");
  } else {
  // display position
    Serial.print("Temp: ");
    Serial.print(tempC);     // display the temperature
    Serial.print("°C");

    Serial.println("Humi: ");
   Serial.print(humi);      // display the humidity
  Serial.print("%");
  }

  // wait a 2 seconds between readings
  delay(2000);
}
