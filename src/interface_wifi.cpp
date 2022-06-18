#include <Arduino.h>
#include "SPIFFS.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"

#include "definicoes_sistema.h"
#include "interface_wifi.h"


void InterfaceWiFi::iniciarWiFi()
{
    // Conectar no WiFI
    WiFi.begin(SSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando no WiFi...");
    }
    Serial.print("Conectado no IP: ");
    Serial.println(WiFi.localIP());

    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/index.html", String(), false, processor);
    });
    
    // Route to load style.css file
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/style.css", "text/css");
    });
}

String processor(const String& var){
  Serial.println(var);
  if(var == "ESTADO"){
    switch(maquinaEstados.getEstado()) {
    case IDLE:
        return "Idle";
    case IMPRIMINDO:
        return "Imprimindo";
    case CALIBRANDO:
        return "Calibrando";
    }
  }
  return String();
}