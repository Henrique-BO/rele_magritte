#include <Arduino.h>
#include "SPIFFS.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"

#include "maquina_estados.h"
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

    // Página principal
    server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/index.html", String(), false, this->processor);
    });
    
    // Arquivo style.css
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/style.css", "text/css");
    });

    // Botão imprimir
    server.on("/imprimir", HTTP_GET, [this](AsyncWebServerRequest *request){
        this->imprimir();    
        request->redirect("/"); // TODO provavelmente não vai dar tempo de atualizar o estado
    });

    // Botão cancelar
    server.on("/cancelar", HTTP_GET, [this](AsyncWebServerRequest *request){
        this->cancelar();    
        request->redirect("/"); // TODO provavelmente não vai dar tempo de atualizar o estado
    });

    // Botão imprimir
    server.on("/calibrar", HTTP_GET, [this](AsyncWebServerRequest *request){
        this->calibrar();    
        request->redirect("/"); // TODO provavelmente não vai dar tempo de atualizar o estado
    });

    // Botão carregar programa
    server.on("/carregar", HTTP_POST, [](AsyncWebServerRequest *request){
        request->send(200);
    }, this->carregarPrograma);

    // Inicia o servidor
    server.begin();
}

void InterfaceWiFi::imprimir()
{
    Evento evento = IMPRIMIR;
    while(xQueueSendToBack(xQueueEventos, &evento, portMAX_DELAY) != pdTRUE);
}

void InterfaceWiFi::cancelar()
{
    Evento evento = CANCELAR;
    while(xQueueSendToBack(xQueueEventos, &evento, portMAX_DELAY) != pdTRUE);
}

void InterfaceWiFi::calibrar()
{
    Evento evento = CALIBRAR;
    while(xQueueSendToBack(xQueueEventos, &evento, portMAX_DELAY) != pdTRUE);
}

void InterfaceWiFi::carregarPrograma(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
    if (maquinaEstados.getEstado() == IDLE) {
        // https://github.com/smford/esp32-asyncwebserver-fileupload-example/blob/master/example-01/example-01.ino
        String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
        Serial.println(logmessage);

        if (!index) {
            logmessage = "Upload Start: " + String(filename);
            // open the file on first call and store the file handle in the request object
            // request->_tempFile = SPIFFS.open("/" + filename, "w");
            request->_tempFile = SPIFFS.open("/gcode.txt", "w");
            Serial.println(logmessage);
        }

        if (len) {
            // stream the incoming chunk to the opened file
            request->_tempFile.write(data, len);
            logmessage = "Writing file: " + String(filename) + " index=" + String(index) + " len=" + String(len);
            Serial.println(logmessage);
        }

        if (final) {
            logmessage = "Upload Complete: " + String(filename) + ",size: " + String(index + len);
            // close the file handle as the upload is now done
            request->_tempFile.close();
            Serial.println(logmessage);
            request->redirect("/");
        }
    }
}

String InterfaceWiFi::processor(const String& var){
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