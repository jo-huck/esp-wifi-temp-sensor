#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ArduinoOTA.h>

const int sleepzeit_Min=1;

const int dhtPin = D6;

const char* ssid     = "Huck!Lan";
const char* password = "HuckSpeed2018+"; 
const char* host = "192.168.1.49"; // muss ip adresse sein
const int httpPort = 80;
const char* uri = "/api/set";
float temp = 10.5;
float humi = 48.6;

DHT dht(dhtPin,DHT22);

void setup() {
    dht.begin();
    Serial.begin(9600);
    
    Serial.print("\nVerbinde mich mit Netz: ");
    Serial.println(ssid);
    WiFi.begin(ssid,password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi Verbindung aufgebaut");  
    Serial.print("Eigene IP des ESP-Modul: ");
    Serial.println(WiFi.localIP());
    ArduinoOTA.begin();
    temp = dht.readTemperature();
    humi = dht.readHumidity();
}
void loop() {
    for(int i = 0; i<6;i++) {
        ArduinoOTA.handle();
        delay(250);
    }
    WiFiClient client;
    String url = uri;   //Url wird zusammengebaut
    url += "?temp-out=";
    url += temp;
    url += "&humi-out=";
    url += humi;
    client.connect(host, httpPort);
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        Serial.println("Uebergabe klappt nicht, ich versuche es spaeter noch mal!");
        client.stop();
        WiFi.disconnect(); 
        Serial.print("Schlafe jetzt ...");
        ESP.reset();
    }
    }
    client.stop();
    WiFi.disconnect(); 
    Serial.println("\nVerbindung beendet");
    ArduinoOTA.handle();
    Serial.print("Schlafe jetzt ...");
    ESP.deepSleep( sleepzeit_Min*10000000);  //Angabe in Minuten
}