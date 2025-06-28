#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "MAX30100_PulseOximeter.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"

// WiFi Credentials
const char* ssid = "Varshitha";      
const char* password = "password";   

// ThingSpeak Settings
const char* server = "http://api.thingspeak.com/update";
String apiKey = "JCHZLB5XQGIHOOQW";  

// Sensor Pins
#define DHTPIN 19      
#define DS18B20 5       
#define DHTTYPE DHT11   
#define MQ135PIN 34     

// Sensor Initialization
DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(DS18B20);
DallasTemperature sensors(&oneWire);
PulseOximeter pox;
Adafruit_MPU6050 mpu;

// Web Server
WebServer serverWeb(80);

// Variables
float temperature, humidity, bodyTemp, BPM, SpO2;
uint32_t lastReportTime = 0;
#define REPORTING_PERIOD_MS 1000  

bool fallDetected = false;  
int mq135Value = 0;         

void onBeatDetected() {
  Serial.println("💓 Beat Detected!");
}

// I2C Scanner
void scanI2CDevices() {
  Serial.println("\n🔍 Scanning I2C Devices...");
  byte count = 0;
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.print("✅ I2C device found at address: 0x");
      Serial.println(address, HEX);
      count++;
      delay(10);
    }
  }
  if (count == 0) {
    Serial.println("❌ No I2C devices found. Check wiring!");
  }
}

// HTML page
String generateWebPage() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
  html += "<meta http-equiv='refresh' content='3'>";
  html += "<title>ESP32 Sensor Dashboard</title></head><body style='font-family: Arial; text-align:center;'>";
  html += "<h1>📈 ESP32 Health Monitoring</h1>";
  html += "<p>🌡 Room Temperature: " + String(temperature) + " °C</p>";
  html += "<p>💧 Room Humidity: " + String(humidity) + " %</p>";
  html += "<p>🔥 Body Temperature: " + String(bodyTemp) + " °C</p>";
  html += "<p>💓 Heart Rate (BPM): " + String(BPM) + "</p>";
  html += "<p>🩸 SpO₂ Level: " + String(SpO2) + " %</p>";
  html += "<p>🏭 Air Quality (MQ135): " + String(mq135Value) + "</p>";
  html += "<p>🚨 Fall Detection: " + String(fallDetected ? "YES" : "NO") + "</p>";
  html += "<p><small>Auto-refresh every 3 seconds</small></p>";
  html += "</body></html>";
  return html;
}

void handleRoot() {
  serverWeb.send(200, "text/html", generateWebPage());
}

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\n✅ Connected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Start Web Server
  serverWeb.on("/", handleRoot);
  serverWeb.begin();
  Serial.println("✅ Web server started!");

  // Run I2C Scanner
  scanI2CDevices();

  // Initialize MPU6050
  Serial.print("Initializing MPU6050...");
  if (!mpu.begin(MPU6050_I2CADDR_DEFAULT, &Wire)) {
    Serial.println("❌ FAILED. Check MPU6050 wiring!");
    while (1) delay(20);
  } else {
    Serial.println("✅ SUCCESS");
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  }

  // Initialize Temp Sensors
  dht.begin();
  sensors.begin();

  // Initialize Pulse Oximeter
  Serial.print("Initializing MAX30100...");
  if (!pox.begin()) {
    Serial.println("❌ FAILED. Check MAX30100 wiring!");
    while (1);
  } else {
    Serial.println("✅ SUCCESS");
    pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
    pox.setOnBeatDetectedCallback(onBeatDetected);
  }

  pinMode(MQ135PIN, INPUT);
}

void loop() {
  pox.update();  
  serverWeb.handleClient();  // Handle web client
  sensors.requestTemperatures();  

  sensors_event_t acc, gcc, temp;
  mpu.getEvent(&acc, &gcc, &temp);

  float totalAcc = sqrt(
    acc.acceleration.x * acc.acceleration.x +
    acc.acceleration.y * acc.acceleration.y +
    acc.acceleration.z * acc.acceleration.z
  );
  

  fallDetected = (totalAcc > 10.0 || totalAcc < 3.0);

  mq135Value = analogRead(MQ135PIN);

  if (millis() - lastReportTime > REPORTING_PERIOD_MS) {
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
    bodyTemp = sensors.getTempCByIndex(0);
    BPM = pox.getHeartRate();
    SpO2 = pox.getSpO2();

    Serial.println("\n******** Sensor Readings ********");
    Serial.print("🌡 Room Temperature: "); Serial.print(temperature); Serial.println("°C");
    Serial.print("💧 Room Humidity: "); Serial.print(humidity); Serial.println("%");
    Serial.print("🔥 Body Temperature: "); Serial.print(bodyTemp); Serial.println("°C");
    Serial.print("💓 Heart Rate (BPM): "); Serial.println(BPM);
    Serial.print("🩸 SpO2 Level: "); Serial.print(SpO2); Serial.println("%");
    Serial.print("🏭 Air Quality (MQ135): "); Serial.println(mq135Value);

    if (fallDetected) {
      Serial.println("🚨 FALL DETECTED! 🚨");
    } else {
      Serial.println("✅ No Fall Detected");
    }
    Serial.println("*********************************\n");

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      String url = server;
      url += "?api_key=" + apiKey;
      url += "&field1=" + String(temperature);
      url += "&field2=" + String(humidity);
      url += "&field3=" + String(bodyTemp);
      url += "&field4=" + String(BPM);
      url += "&field5=" + String(SpO2);
      url += "&field6=" + String(mq135Value);
      url += "&field7=" + String(fallDetected ? 1 : 0);

      http.begin(url);
      int httpCode = http.GET();
      if (httpCode > 0) {
        Serial.println("✅ Data pushed to ThingSpeak!");
      } else {
        Serial.println("❌ Failed to push data to ThingSpeak!");
      }
      http.end();
    } else {
      Serial.println("❌ WiFi not connected. Data not sent.");
    }

    lastReportTime = millis();
  }
}
