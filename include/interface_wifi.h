#ifndef INTERFACE_WIFI_H
#define INTERFACE_WIFI_H

#include "ESPAsyncWebServer.h"

#define SSID "abcabc"
#define PASSWORD "abcabc"

String processor(const String& var);

class InterfaceWiFi {
    public:
        InterfaceWiFi(): server(AsyncWebServer(80)) {}
        void iniciarWiFi();
        void carregarPrograma();
        void imprimir();
        void cancelar();
        void calibrar();

    private:
        AsyncWebServer server;
};

#endif