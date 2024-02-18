/*
  This code is based off of an earlier version that monitors 6 ds18b20 sensors and logs the data to google sheets every 5 minutes. 
  I have added a text feature utilizing whatsapp and callmebot api as well as a voltage sensor to monitor a solar powered battery.
*/



#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const char* ssid = "YOUR_SSID"; // Enter your Wifi SSID
const char* password = "YOUR_PASSWORD"; // Enter your networks password
const char* host1 = "script.google.com";
const char* host2 = "api.callmebot.com";
const int httpsPort = 443;
WiFiClientSecure client; // Initialize wifi client
String GAS_ID = "YourGoogleAppsScriptID"; // Enter Google Apps Script ID here
OneWire oneWire(14); // Assign Onwire bus to pin 14 / D5 on esp8266
DallasTemperature sensors(&oneWire); //initialize sensors
const int voltagePin = A0; // Connect your voltage sensor to A0
unsigned long previousMillis = 0; // Variable to store time
const long interval = 300000; // Time in between aquiring and logging sensor values and assesing wether or not to send text
const int thresholdColdLow = 30;  // Below this value, call cold text function
const int thresholdColdHigh = 35; // Above this value, call better function and allow text again
const int thresholdHotLow = 80; // Below this value, call better function and allow text again
const int thresholdHotHigh = 85; // Above this value, call hot text function
bool functionCalled = false;  // Variable to track whether the function has been called

void setup() {
  Serial.begin(115200);
  delay(500);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  client.setInsecure();
  sensors.begin();
}

void loop() {

  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval){
    sensors.requestTemperatures();
    float temps[6];
    for (int i = 0; i < 6; i++) {
      temps[i] = convertToFahrenheit(sensors.getTempCByIndex(i));
    }
    float voltage = readVoltage();
    sendData(temps, voltage);

    int insideTemp = sensors.getTempFByIndex(1); // Get value of sensor 2
    if(insideTemp < thresholdColdLow && !functionCalled){ 
      sendText("Im cold out here"); // Call to send I'm cold text
      functionCalled = true; // Set flag to true to indicate that the function has been called
    }
    if(insideTemp > thresholdColdHigh){
      sendText("Thats better"); // Call to send that's better text
      functionCalled = false; // Reset the flag when the sensor value goes above upper cold threshold
    }
    if(insideTemp > thresholdHotHigh && !functionCalled){
      sendText("Im burnin up out here"); // Call to send I'm hot text
      functionCalled = true; // Set flag to true to indicate that the function has been called
    }
    if(insideTemp < thresholdHotLow){
      sendText("Thats better"); // Call to send that's better text
      functionCalled = false; // Reset the flag when the sensor value goes below lower hot threshold
    }
    previousMillis = currentMillis;
  }

}

float convertToFahrenheit(float celsius) {
  return celsius * 9.0 / 5.0 + 32.0;
}

float readVoltage() {
  int sensorValue = analogRead(voltagePin);
  float voltage = sensorValue * (5.0 / 1023.0); // Assuming a simple voltage divider
  return voltage;
}

void sendData(float temps[6], float voltage) {
  if (!client.connect(host1, httpsPort)) {
    Serial.println("Connection failed");
    return;
  }
  String url = "/macros/s/" + GAS_ID + "/exec?";
  for (int i = 0; i < 6; i++) {
    url += "temp" + String(i + 1) + "=" + String(temps[i]) + "&";
  }
  url += "voltage=" + String(voltage);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host1 + "\r\n" +
               "User-Agent: ESP8266\r\n" +
               "Connection: close\r\n\r\n");

  while (client.connected()) {
    if (client.available()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        break;
      }
    }
  }
  client.stop();
}

void sendText(String message){
  String url = "first part of callmebot api here" + message + "last part of callmebot api here";

  if (!client.connect(host2, httpsPort)) {
    Serial.println("Connection failed");
    return;
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host2 + "\r\n" +
               "User-Agent: ESP8266\r\n" +
               "Connection: close\r\n\r\n");

  while (client.connected()) {
    if (client.available()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        break;
      }
    }
  }
  client.stop();
  }
}
