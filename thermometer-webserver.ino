#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#define DHTTYPE DHT22
#define DHTPIN 4 // D2

const char* ssid = "ssid";
const char* password = "pass";

ESP8266WebServer server(80);
 
// Initialize DHT sensor 
DHT dht(DHTPIN, DHT22, 11); // DHT11 or DHT22
 
float humidity, temp_f;
String webString=""; 
unsigned long previousMillis = 0;
const long interval = 2000;
 
void handle_root() {
  server.send(200, "text/plain", "Hello from the weather esp8266, read from /temp or /humidity or /all for both");
  delay(100);
}
 
void setup(void)
{
  // You can open the Arduino IDE Serial Monitor window to see what the code is doing
  Serial.begin(115200);  // Serial connection from ESP-01 via 3.3v console cable
  dht.begin();           // initialize temperature sensor

  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.print("\n\r \n\rWorking to connect");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("DHT Weather Reading Server");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
   
  server.on("/", handle_root);
  
  server.on("/temp", [](){
    gettemperature();
    webString="Temperature: "+String((int)temp_f)+" F";
    server.send(200, "text/plain", webString);
  });

  server.on("/humidity", [](){
    gettemperature();
    webString="Humidity: "+String((int)humidity)+"%";
    server.send(200, "text/plain", webString);
  });

  server.on("/all", [](){ 
    gettemperature();           // read sensor
    webString="Temperature: "+String((int)temp_f)+" F"+" | Humidity: "+String((int)humidity)+"%";
    pushData(String((int)temp_f),String((int)humidity));
    server.send(200, "text/plain", webString);
  });
  
  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(void)
{
  server.handleClient();
} 

void gettemperature() {
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;   

    humidity = dht.readHumidity();
    temp_f = dht.readTemperature(true);
    if (isnan(humidity) || isnan(temp_f)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
  }
}
