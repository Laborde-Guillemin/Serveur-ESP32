/* ____                                        _      __   ____   ___
  / __/ ___   ____ _  __ ___  __ __  ____     | | /| / /  / __/  / _ )
 _\ \  / -_) / __/| |/ // -_)/ // / / __/     | |/ |/ /  / _/   / _  |
/___/  \__/ /_/   |___/ \__/ \_,_/ /_/        |__/|__/  /___/  /____/ */

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <Adafruit_BME280.h>

/*Port SPI*/
#define SCK  18
#define MISO  19
#define MOSI  23
#define CS  32

// Replace with your network credentials
const char* ssid = "WIFI-CIEL"; //WIFI-CIEL
const char* password = "alcasarciel"; //alcasar-ciel

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
SPIClass spi = SPIClass(VSPI);

Adafruit_BME280 bme;
float Temp;// bme.readTemperature();
float Hum;// bme.readHumidity()

void initSDCard(){
  spi.begin(SCK, MISO, MOSI, CS);
  if(!SD.begin(CS,spi,80000000)){
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}
// Initialisation du capteur BME280
void initBME280(){
    if (!bme.begin(0x76)) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }
}
/*Basile*/
void capteurT() {
    // Mesure des valeurs BME280
    Temp = bme.readTemperature();
    Hum = bme.readHumidity();

    Serial.print("Temperature = ");
    Serial.print(Temp);
    Serial.println(" °C");
    Serial.print("Humidité = ");
    Serial.print(Hum);
    Serial.println(" %");
}

void setup() {
  Serial.begin(115200);
  initWiFi();
  initSDCard();
  initBME280();
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
    capteurT();
    String data = "{\"temperature\": " + String(Temp, 2) + ", \"humidity\": " + String(Hum, 2) + "}";
    request->send(200, "application/json", data);
  });

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SD, "/index.html", "text/html");
     //server.serveStatic("/Serv/index.html", SD, "text/html");
  });
  server.serveStatic("/", SD, "/");
  server.serveStatic("/styles.css", SD, "/styles.css");
  server.serveStatic("/script.js", SD, "/script.js");
  server.begin();
}

void loop() {
 capteurT();
 delay(5000);
}