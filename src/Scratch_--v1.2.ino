#include <Wire.h>
#include <RTClib.h>
#include <DHT.h>
#include <WiFi.h>
#include <FirebaseESP32.h>

#define DHT_PIN 4          // DHT11 sensor pin
#define MOISTURE_PIN 34    // Soil moisture sensor pin
#define RELAY_PIN 14       // Relay module control pin

// Firebase credentials
#define FIREBASE_HOST "your-firebase-project-id.firebaseio.com"
#define FIREBASE_AUTH "your-firebase-auth-token"

RTC_DS3231 rtc;
DHT dht(DHT_PIN, DHT11);

// Firebase configuration
FirebaseData firebaseData;

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);

  // Connect to Wi-Fi
  connectToWiFi();

  // Initialize Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {
  // Read sensor data
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int moisture = analogRead(MOISTURE_PIN);

  // Get current time from RTC
  DateTime now = rtc.now();

  // Check watering schedule (example: water twice a day)
  if ((now.hour() == 8 || now.hour() == 18) && moisture < 500) {
    // Water the plant
    activateWaterPump();
  }

  // Update Firebase with sensor data
  updateFirebase(temperature, humidity, moisture);

  delay(60000);  // Delay for 1 minute (adjust as needed)
}

void connectToWiFi() {
  // Your Wi-Fi credentials
  char ssid[] = "your-ssid";
  char password[] = "your-password";

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void activateWaterPump() {
  digitalWrite(RELAY_PIN, HIGH);  // Turn on water pump
  delay(10000);  // Run the pump for 10 seconds (adjust as needed)
  digitalWrite(RELAY_PIN, LOW);   // Turn off water pump
}

void updateFirebase(float temperature, float humidity, int moisture) {
  Firebase.setInt(firebaseData, "/temperature", temperature);
  Firebase.setInt(firebaseData, "/humidity", humidity);
  Firebase.setInt(firebaseData, "/moisture", moisture);
  Firebase.pushTimestamp(firebaseData, "/log");
}
