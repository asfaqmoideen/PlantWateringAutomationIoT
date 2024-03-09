#include <Wire.h>
#include <RTClib.h>
#include <DHT.h>
#include<FirebaseESP32.h>
#define WIFI_SSID "Asfaq's moto g32"
#define WIFI_PASSWORD "Shivajidas"
#define FIREBASE_HOST "https://espfirebase-cd50f-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "miqZ3KiVPOUd8y9aeuOSnnjUGWLfpZGMdU08Frza"


#define DHT_PIN 4          // DHT11 sensor pin
#define MOISTURE_PIN 13    // Soil moisture sensor pin
#define RELAY_PIN 5  // Relay module control pin
FirebaseData fbdo;
int soilmoisture ;
unsigned long wateringStartTime = 0;  // Timestamp for when watering starts
const unsigned long wateringDuration = 2000;  // Watering duration in milliseconds
DHT dht(DHT_PIN, DHT11);
RTC_DS3231 rtc;


/*void activateWaterPump() {
  digitalWrite(RELAY_PIN, LOW);  // Turn on water pump
  delay(10000);  // Run the pump for 10 seconds (adjust as needed)
  digitalWrite(RELAY_PIN, HIGH);   // Turn off water pump
}*/
void setup () {

  Serial.begin(9600);
  dht.begin(); // initialize the DHT sensor
  pinMode(RELAY_PIN,OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);  
  // SETUP RTC MODULE
  if (! rtc.begin()) {
    Serial.println("Please connect RTC Module");
  //  Serial.flush();
     // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.reconnectWiFi(true);

    while (1);
  }

  // automatically sets the RTC to the date & time on PC this sketch was compiled
// rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // manually sets the RTC with an explicit date & time, for example to set
  // January 21, 2021 at 3am you would call:
  // rtc.adjust(DateTime(2023, 11, 16, 19, 50, 0));
}

void loop () {
  firebasedata();
  DateTime now = rtc.now();
  Serial.print("ESP32 RTC Date Time: ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);
   float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int soilmoisture = digitalRead(MOISTURE_PIN);
  Serial.print("Soil Status :");
  Serial.print(soilmoisture);
  Serial.print(" Temperature :");
  Serial.print(temperature);
  Serial.print(" Humidity :");
  Serial.println(humidity);
 
  // Check watering schedule (example: water twice a day)
  if ((now.hour() == 8 || now.hour() == 15) && soilmoisture == 1) {
    // Water the plant if not already watering
    //Serial.print("Please Water the Plant");
   // Serial.print(wateringStartTime);
    if (wateringStartTime == 0) {
      wateringStartTime = millis();  // Record the start time
      activateWaterPump();
    }
  } else {
    // Reset watering start time when not watering
  //  wateringStartTime = 0;
  }

  // Check if it's time to stop watering
  if (wateringStartTime > 0 && millis() - wateringStartTime >= wateringDuration) {
    stopWaterPump();
    wateringStartTime = 0;  // Reset the watering start time
  }

  delay(1000);  // Delay for 1 second (adjust as needed)
}
void firebasedata(){
  
    sensors_event_t event;
  dht.temperature().getEvent(&event);
  float temp = event.temperature;
  Firebase.setFloat(fbdo,"Temperature",temp);
  
  dht.humidity().getEvent(&event);
  int hum = event.relative_humidity;

  Firebase.setFloat(fbdo,"Humidity",hum);

  
 
  String x;

    if (Firebase.getString(fbdo, "Asfaq"))
    {
      x = (fbdo.stringData());
      Serial.println(x);
      
    }
    if (x=="1")
    {
      digitalWrite(LED_BUILTIN,HIGH);
    }
    if (x=="0")
    {
      digitalWrite(LED_BUILTIN,LOW);
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
