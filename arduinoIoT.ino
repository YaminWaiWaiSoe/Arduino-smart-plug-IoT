#include "DHT.h"
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include<iostream>
#include <sstream> // use stringstream class  
using namespace std;

#define BLYNK_AUTH_TOKEN "????????????????????????????????"  // blynk token

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

#define DHTPIN 14     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11

int pin = 36; //PIR sensor

int standardTem = 50;  // temperature threshold 20
String device = "????????????????????????????????????????";  // smart plug device ID
String token = "????????????????????????????????";  // smart plug device token

String serverPath = "https://eu-wap.tplinkcloud.com/?token="+token;

char auth[] = BLYNK_AUTH_TOKEN;

DHT dht(DHTPIN, DHTTYPE);

const char* ssid =  "???";  // wifi name
const char* password = "???"; // wifi password

unsigned long previousMillis = 0;  // for wifi reconnection
unsigned long interval = 30000; 

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
}

int userMode = 0;
int pinValue = 0;

BLYNK_WRITE(V0)
{
  userMode = param.asInt(); // assigning incoming value from pin V1 to a variable
  
  Serial.print("Mode: ");
  Serial.println(userMode);
  // process received value
}

BLYNK_WRITE(V1)
{
  pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  
  Serial.print("Temp: ");
  Serial.println(pinValue);
  // process received value
}

void blynkValue(){
  HTTPClient http;

  String server = "http://??????/get/V1";  
      // Your Domain name with URL path or IP address with path
  http.begin(server);
  int httpResponseCode = http.GET();
  if (httpResponseCode>0) {
        Serial.print("Blynk Value HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();

        std::string s;
        for(int i = 0; i<payload.length(); i++) {
          int check = isdigit(payload[i]);
          if(check){
            s += payload[i];
          }
        }
        stringstream obj;  
        int v;
        obj << s; // insert data into obj  
        obj >> v; // fetch integer type data  
        Serial.print("value: ");
        Serial.println(v);
        pinValue = v;
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
}

void blynkMode(){
  HTTPClient http;

  String server = "http://????/get/V0";
      // Your Domain name with URL path or IP address with path
  http.begin(server);
  int httpResponseCode = http.GET();
  if (httpResponseCode>0) {
        Serial.print("Blynk Mode HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
     
        for(int i = 0; i<payload.length(); i++) {
          int check = isdigit(payload[i]);
          if(check){
            stringstream obj;  
            int d;
             obj << payload[i]; // insert data into obj  
             obj >> d; // fetch integer type data  
            Serial.print("digit: ");
            Serial.println(d);
            userMode = d;
          }
        }
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
      if(userMode == 1){
        blynkValue();
      }
}

void setup() {
  
  Serial.begin(9600);

  initWiFi();
  
  pinMode(DHTPIN, INPUT);  //temperature
  pinMode(pin,INPUT);      // motion

  dht.begin(); 

  Blynk.begin(auth, ssid, password);

  blynkMode();
}

void smartPlug(int state){
  HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());  
      http.addHeader("Content-Type", "application/json");   

      StaticJsonDocument<256> doc;
      JsonObject root = doc.to<JsonObject>();
      root["method"] = "passthrough";
      
      JsonObject params= root.createNestedObject("params");
      params["deviceId"] = device;
      JsonObject requestData= params.createNestedObject("requestData");
      JsonObject system= requestData.createNestedObject("system");
      JsonObject set_relay_state= system.createNestedObject("set_relay_state");
      set_relay_state["state"] = state;
  
      String httpRequestData;
      serializeJson(root, httpRequestData);
  //    Serial.println(httpRequestData);
//      serializeJsonPretty(root, Serial);

               
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      // Free resources
      http.end();
}



void loop() {
  // Wait a few seconds between measurements.
  delay(5000);

  Blynk.run();

  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // motion detection
  bool isDetected = digitalRead(pin);
 
  // Check if any reads failed and exit early (to try again).
  if (isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }
  
  if(isDetected){
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    Serial.println("Presence detected");
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.println(F("°C ")); 
   
    if(userMode == 1){
      standardTem = pinValue;
    }
    else{
      standardTem = 0; // actual 20 and change condition <
    }
    Serial.print("Temp Threshold: ");
    Serial.println(standardTem);
    if(t>standardTem){
      smartPlug(1);
    }
    else
      smartPlug(0);
    delay(3000);
  }
  else{
   digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    Serial.println("Motion ended");
    smartPlug(0);
    delay(3000);
  }

  Serial.print("Temp Threshold:");
  Serial.println(standardTem);
}
