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

int Date;
int Heure;
float Temp;
float Hum;
int COV;
int Alde;
int CO2;
int PM_25;
int PM_1;
int PM_10;


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
  WiFi.mode(WIFI_STA); // configuration en mode stations
  WiFi.begin(ssid, password); // connection grâce à l'identifiant et le mot de passe
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000); // attente de connection
  }
  Serial.println(WiFi.localIP()); // écriture de l'adresse IP
}

/*Valeurs d'essaie*/
void valeur(){
  Date = random(1, 32); // 1 to 31
  Heure = random(0, 24); // 0 to 23
  Temp = random(0, 36); // 0 to 35
  Hum = random(0, 100); // 0 to 99
  COV = random(0, 25001); // 0 to 25000
  Alde = random(0, 10001); // 0 to 10000, assuming some max value for demonstration
  CO2 = random(0, 1001); // 0 to 1000
  PM_25 = random(0, 1001); // 0 to 1000, assuming some max value for demonstration
  PM_1 = random(0, 1001); // 0 to 1000, assuming some max value for demonstration
  PM_10 = random(0, 1001); // 0 to 1000, assuming some max value for demonstration
}

/*Communication entre l'esp-32 et la page web*/
void comServeur(){
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request) {
    valeur();
    String data = "{\"temperature\": " + String(Temp) + 
                  ", \"humidity\": " + String(Hum) + 
                  ", \"CO2\": " + String(CO2) + 
                  ", \"COV\": " + String(COV) + 
                  ", \"FormeAlde\": " + String(Alde) + 
                  ", \"PM_1\": " + String(PM_1) + 
                  ", \"PM_25\": " + String(PM_25) + 
                  ", \"PM_10\": " + String(PM_10) + "}"; // mise à jour des valeurs de la page web 
    request->send(200, "application/json", data); // chaine JSON
  });
  //Gestions du fichier .html
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SD, "/index.html", "text/html");
  });
  //Gestions des fichiers statiques
  server.serveStatic("/", SD, "/");// utilisation de la carte SD
  server.serveStatic("/styles.css", SD, "/styles.css");// utilisation du fichier css dans la carte sd
  server.serveStatic("/script.js", SD, "/script.js");// utilisation du fichier js dans la carte sd
  server.begin(); // démarage du serveur
}

void setup() {
  Serial.begin(115200);
  initWiFi();
  initSDCard();
  comServeur();
}

void loop() {
 valeur();
 delay(5000);
}