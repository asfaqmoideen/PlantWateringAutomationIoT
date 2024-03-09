#include <Wire.h>
#include <RTClib.h>
#include <DHT.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define DHT_PIN 4          // DHT11 sensor pin
#define MOISTURE_PIN 13    // Soil moisture sensor pin
#define RELAY_PIN 5        // Relay module control pin

int soilmoisture;
unsigned long wateringStartTime = 0;  // Timestamp for when watering starts
const unsigned long wateringDuration = 2000;  // Watering duration in milliseconds
DHT dht(DHT_PIN, DHT11);
RTC_DS3231 rtc;

const char* ssid = "Asfaq's moto g32";
const char* password = "ShivajiDas";

const char* aioUsername = "asfaqmoideen";
const char* aioKey = "aio_SVvd31f70VOWrWzZMOpXIlZy1Lht";
const char* mqttServer = "io.adafruit.com";
const int mqttPort = 1883;

const char* soilStatusTopic = "asfaqmoideen/feeds/soil-status";
const char* temperatureTopic = "asfaqmoideen/feeds/plant-temperature";
const char* humidityTopic = "asfaqmoideen/feeds/plant-humidity";
const char* moistureTopic = "asfaqmoideen/feeds/plant-moisture";
const char* lastWateredTopic = "asfaqmoideen/feeds/last-watered";
const char* pumpControlTopic = "asfaqmoideen/feeds/pump-control";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);

  if (!rtc.begin()) {
    Serial.println("RTC module is NOT found");
    Serial.flush();
    while (1);
  }

  connectToWiFi();
  client.setServer(mqttServer, mqttPort);
}

void loop() {
  DateTime now = rtc.now();
  Serial.print("ESP32 RTC Date Time: ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  soilmoisture = digitalRead(MOISTURE_PIN);

  Serial.print("Soil Status: ");
  Serial.print(soilmoisture == 1 ? "Dry" : "Wet");
  Serial.print(", Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Humidity: ");
  Serial.println(humidity);

  // Publish sensor data to Adafruit IO MQTT feeds
  publishToMQTT(soilmoisture > 3000 ? "Dry" : "Wet", temperature, humidity, soilmoisture);

  // Check watering schedule (example: water twice a day)
  if ((now.hour() == 9 || now.hour() == 15) && soilmoisture > 3000) {
    if (wateringStartTime == 0) {
      wateringStartTime = millis();
      activateWaterPump();
    }
  }

  // Check if it's time to stop watering
  if (wateringStartTime > 0 && millis() - wateringStartTime >= wateringDuration) {
    stopWaterPump();
    wateringStartTime = 0;
  }

  // Connect to Adafruit IO MQTT broker
  if (!client.connected()) {
    reconnect();
  }

  // Subscribe to pump control topic
  client.subscribe(pumpControlTopic);

  client.loop();
  delay(1000);
}

void publishToMQTT(const char* soilStatus, float temperature, float humidity, int moisture) {
  client.publish(soilStatusTopic, soilStatus);
  client.publish(temperatureTopic, String(temperature).c_str());
  client.publish(humidityTopic, String(humidity).c_str());
  client.publish(moistureTopic, String(moisture).c_str());

  if (wateringStartTime > 0) {
    client.publish(lastWateredTopic, String(millis() - wateringStartTime).c_str());
  }
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting Adafruit IO MQTT connection...");
    if (client.connect("ESP32Client", aioUsername, aioKey)) {
      Serial.println("Connected to Adafruit IO MQTT broker");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void activateWaterPump() {
  Serial.println("Watering the plant!");
  digitalWrite(RELAY_PIN, LOW);  // Turn on water pump
}

void stopWaterPump() {
  Serial.println("Stopping the watering!");
  digitalWrite(RELAY_PIN, HIGH);   // Turn off water pump
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Handle incoming MQTT messages
  if (strcmp(topic, pumpControlTopic) == 0) {
    if (payload[0] == '1') {
      activateWaterPump();
    } else if (payload[0] == '0') {
      stopWaterPump();
    }
  }
}
