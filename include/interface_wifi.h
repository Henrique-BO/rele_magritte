#ifndef INTERFACE_WIFI_H
#define INTERFACE_WIFI_H

#include "ESPAsyncWebServer.h"

#define SSID "Magdolna"
#define PASSWORD "41308133"


class InterfaceWiFi {
    public:
        InterfaceWiFi(): server(AsyncWebServer(80)) {}
        void iniciarWiFi();
        void imprimir();
        void cancelar();
        void calibrar();
        static void carregarPrograma(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
        static String processor(const String& var);
        static void handleEstado(AsyncWebServerRequest *request);

    private:
        AsyncWebServer server;
};

extern InterfaceWiFi interfaceWifi;

#endif