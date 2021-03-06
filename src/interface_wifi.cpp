#include <Arduino.h>
#include "SPIFFS.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"

#include "maquina_estados.h"
#include "interface_wifi.h"
#include "credenciais.h"


void InterfaceWiFi_t::iniciarWiFi()
{
    Serial.println("[InterfaceWiFi] Iniciando interface WiFi");

    // Conectar no WiFI
    WiFi.begin(SSID, PASSWORD);
    Serial.print("[InterfaceWiFi] Conectando no WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.print("\n[InterfaceWiFi] Conectado no IP: ");
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
        Serial.println("[InterfaceWiFi] Botão imprimir");
        this->imprimir();    
        request->redirect("/");
    });

    // Botão cancelar
    server.on("/cancelar", HTTP_GET, [this](AsyncWebServerRequest *request){
        this->cancelar();    
        Serial.println("[InterfaceWiFi] Botão cancelar");
        request->redirect("/");
    });

    // Botão imprimir
    server.on("/calibrar", HTTP_GET, [this](AsyncWebServerRequest *request){
        this->calibrar();    
        request->redirect("/");
    });

    // Botão carregar programa
    server.on("/carregar", HTTP_POST, [](AsyncWebServerRequest *request){
        request->send(200);
    }, this->carregarPrograma);

    // AJAX update estado
    server.on("/updateEstado", HTTP_GET, handleEstado);

    // Inicia o servidor
    server.begin();
}

void InterfaceWiFi_t::imprimir()
{
    Evento evento = IMPRIMIR;
    if(xQueueSendToBack(xQueueEventos, &evento, portMAX_DELAY) != pdTRUE) {
        Serial.println("[InterfaceWiFi] Erro ao enviar evento à fila");
    }
}

void InterfaceWiFi_t::cancelar()
{
    Evento evento = CANCELAR;
    if(xQueueSendToBack(xQueueEventos, &evento, portMAX_DELAY) != pdTRUE) {
        Serial.println("[InterfaceWiFi] Erro ao enviar evento à fila");
    }}

void InterfaceWiFi_t::calibrar()
{
    Evento evento = CALIBRAR;
    if(xQueueSendToBack(xQueueEventos, &evento, portMAX_DELAY) != pdTRUE) {
        Serial.println("[InterfaceWiFi] Erro ao enviar evento à fila");
    }}

void InterfaceWiFi_t::carregarPrograma(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
    if (!InterfaceWiFi.carregando) {
        Evento evento = CARREGAR;
        if(xQueueSendToBack(xQueueEventos, &evento, portMAX_DELAY) != pdTRUE) {
            Serial.println("[InterfaceWiFi] Erro ao enviar evento à fila");
        }
    }

    while(!InterfaceWiFi.carregando) vTaskDelay(pdMS_TO_TICKS(100));

    // https://github.com/smford/esp32-asyncwebserver-fileupload-example/blob/master/example-01/example-01.ino
    String logmessage = "[InterfaceWiFi] Client: " + request->client()->remoteIP().toString() + " " + request->url();
    Serial.println(logmessage);

    if (!index) {
        logmessage = "[InterfaceWiFi] Upload Start: " + String(filename);
        // open the file on first call and store the file handle in the request object
        request->_tempFile = SPIFFS.open("/gcode.txt", "w");
        Serial.println(logmessage);
    }

    if (len) {
        // stream the incoming chunk to the opened file
        request->_tempFile.write(data, len);
        logmessage = "[InterfaceWiFi] Writing file: " + String(filename) + " index=" + String(index) + " len=" + String(len);
        Serial.println(logmessage);
    }

    if (final) {
        logmessage = "[InterfaceWiFi] Upload Complete: " + String(filename) + ",size: " + String(index + len);
        // close the file handle as the upload is now done
        request->_tempFile.close();
        Serial.println(logmessage);
        request->redirect("/");
        InterfaceWiFi.carregando = false;
    }
}

String InterfaceWiFi_t::processor(const String& var){
//   Serial.println("[InterfaceWiFi] " + var);
  if(var == "ESTADO"){
    switch(MaquinaEstados.getEstado()) {
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

void InterfaceWiFi_t::handleEstado(AsyncWebServerRequest *request)
{
    String sEstado;
    switch(MaquinaEstados.getEstado()) {
    case IDLE:
        sEstado = "Idle";
        break;
    case IMPRIMINDO:
        sEstado = "Imprimindo";
        break;
    case CALIBRANDO:
        sEstado = "Calibrando";
        break;
    }
    request->send(200, "text/plane", sEstado);
}