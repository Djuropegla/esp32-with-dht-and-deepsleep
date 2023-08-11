#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

#define uS_TO_S_FACTOR 1000000
#define TIME_TO_SLEEP  900

#define DHTPIN 25
#define DHTTYPE DHT11
DHT dht11(DHTPIN,DHTTYPE);

const char* ssidHome = "";
const char* passwordHome = "";

const char* url = "";

float temperature = 0;
float humidity = 0;

void connectToWiFi(){
  WiFi.mode(WIFI_OFF);
  delay(1000);

  WiFi.mode(WIFI_STA); 
  WiFi.begin(ssidHome, passwordHome);
  Serial.println("\nConnecting");

  while(WiFi.status() != WL_CONNECTED){
      Serial.print(".");
      delay(400);
  }
  
  Serial.print("\nConnected to: "); Serial.println(WiFi.SSID());
  Serial.print("Local ESP32 IP: "); Serial.println(WiFi.localIP());
}

void loadDHT11Data(){
  temperature = dht11.readTemperature();
  humidity = dht11.readHumidity();

  if(isnan(temperature) || isnan(humidity)){
    Serial.println("Failed to read from DHT sensor!");
    temperature = 0;
    humidity = 0;
  }
  Serial.printf("Temperature: %f °C\n", temperature);
  Serial.printf("Humidity: %f %\n", humidity);
}

void sendDHTDataToAPI(){

  loadDHT11Data();

  String updateData = "{\"t\": "+String(temperature)+",\"h\": "+String(humidity)+"}";

  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(updateData);
  String payload = http.getString();
  Serial.print("URL: "); Serial.println(url);
  Serial.print("Data: "); Serial.println(updateData);
  Serial.print("Http code: "); Serial.println(httpResponseCode);
  Serial.print("Response payload: "); Serial.println(payload);
}

void setup() {
  delay(500);
  Serial.begin(115200);
  dht11.begin();

  connectToWiFi();
  sendDHTDataToAPI();

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");

  Serial.println("Going to sleep now");
  delay(1000);
  Serial.flush(); 
  esp_deep_sleep_start();
}

void loop() {}
