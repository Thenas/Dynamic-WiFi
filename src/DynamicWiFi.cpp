/*
Dynamic WiFi
Copyright (C) 2020-2021 by Adrián Tena <guilladrian@gmail.com> 
The MIT License (MIT)
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "DynamicWiFi.h"

Manager::Manager(){
    EEPROM.begin(512);
    isConected = false;
    WiFiStaus = 0;
}
Manager::~Manager()
{
  DEBUG_PRINTLN("[WiFi] eliminando instancia.");
}

IPAddress Manager::getIP(){
    return this->_IP;
    
}

boolean Manager::tryConnect(){
    String SSID = readEEPROM(SSID_ADDRESS, MAX_ALLOC_SSID);
    String PASS = readEEPROM(PASS_ADDRESS, MAX_ALLOC_PASS);
    DEBUG_PRINTLN("[WiFi] Iniciando intento de conexion");
    DEBUG_PRINTLN("\t[WiFi] PASS LEIDO: " + PASS);
    DEBUG_PRINTLN("\t[WiFi] SSID LEIDO: " + SSID);
    
    nAvailableNetworks = WiFi.scanNetworks();
    while (WiFi.scanComplete() == -1);

    int i;
    for(i = 0; i < nAvailableNetworks; i++){
        DEBUG_PRINTLN(String(i+1) + ".-  " +  WiFi.SSID(i));
        if(SSID == WiFi.SSID(i)){
            DEBUG_PRINTLN("\t[WiFi] Coincidencia encontrada");
            break;
        }
    }

    if(i < nAvailableNetworks){
        WiFi.mode(WIFI_STA);
        WiFi.begin(SSID, PASS);
        WiFiStaus = WiFi.waitForConnectResult();
        if(WiFiStaus == WL_CONNECTED ){
            DEBUG_PRINTLN("\t[WiFi] Conexion exitosa!");
            isConected = true; 
            _IP = WiFi.localIP();
            _MAC = WiFi.macAddress();
        }
        else{
            isConected = false;
        }
        return isConected;
    }
    else{
        DEBUG_PRINTLN("\t[WiFi] Red fuera de rango, iniciando modo configuracion");
        return configurateMode();
    }

    return true;
}
boolean Manager::configurateMode(){
    
    server.reset(new ESP8266WebServer(80));
    dnsServer.reset(new DNSServer());
    WiFi.mode(WIFI_AP);
    DEBUG_PRINT("[WiFi] Creando punto de acceso");
    while(!WiFi.softAP(CONFIG_SSID, CONFIG_PASS)){
        DEBUG_PRINT(".");
    }
    DEBUG_PRINT("\n\t[WiFi] Creado exitosamente!\n");
    _IP = WiFi.softAPIP();
    DEBUG_PRINT("\tIP: "); DEBUG_PRINTLN(_IP);
    
    dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer->start(DNS_PORT, "*", _IP);

    server->on("/", std::bind(&Manager::serverOn,this));
    server->on("/save",std::bind(&Manager::saveCredentials, this));
    server->begin();
    unsigned long starTime =  millis() / 1000;
    while(true){
        dnsServer->processNextRequest();
        server->handleClient();
        if(millis()/1000 - starTime >  60*5){
            delay(1000);
            ESP.reset();
            delay(1000);
        }
    }
    return true;
}

void Manager::serverOn(){

    nAvailableNetworks = WiFi.scanNetworks();
    while (WiFi.scanComplete() == -1);

    String HTML = FPSTR(HTML_HEADER);
    HTML += FPSTR(HTML_BODY);

    for(int i = 0; i<nAvailableNetworks; i++){
        HTML += FPSTR(SSID_TEMPLATE);
        HTML.replace("{i}", String(i));
        HTML.replace("{name}", WiFi.SSID(i));
        HTML.replace("{img}", RSSI2IMG(i)); 
    }
    HTML += FPSTR(HTML_TABLE_END);
    HTML += FPSTR(HTML_FORM);
    HTML += FPSTR(HTML_END);

    server->sendHeader("Content-Length", String(HTML.length()));
    server->send(HTTP_CODE_OK, "text/html", HTML);
}


 void Manager::saveCredentials(){
     String _pass = server->arg("pass");
     String _ssid = server->arg("ssid");
     
    writeEEPROM(SSID_ADDRESS, MAX_ALLOC_SSID, server->arg("ssid"));
    writeEEPROM(PASS_ADDRESS, MAX_ALLOC_PASS, server->arg("pass"));
    
    DEBUG_PRINTLN("\t[WiFi] El dispositivo se reiniciara para terminar la configruacion");
    delay(1000);
    ESP.restart();
    delay(1000);
 }

String Manager::RSSI2IMG(int index){
    int32_t rssi = 2 * (WiFi.RSSI(index) + 100);
    DEBUG_PRINT("[WiFi] SSID: " + WiFi.SSID(index));
    DEBUG_PRINT("   ");
    DEBUG_PRINTLN("[WiFi] Porcent: " + String(rssi));

    if(rssi < 0 )                      return FPSTR(IMG_0);
    else if (0 <= rssi && rssi <= 25 ) return FPSTR(IMG_25);
    else if (25 <  rssi && rssi <= 50) return FPSTR(IMG_50);
    else if (50 < rssi && rssi <= 75)  return FPSTR(IMG_75);
    else                               return FPSTR(IMG_100);
}
 
String Manager::readEEPROM(int address, int maxAlloc){
    byte chr;
    String result;
    for (int i = address; i < address+maxAlloc; i++) {
        chr = EEPROM.read(i);
        if (chr != 255) {
            result += (char)chr;
        }
    }
    return result;
}

bool Manager::writeEEPROM(int address, int maxAlloc, String toSave){
    int len = toSave.length();
    if(len > maxAlloc){
        DEBUG_PRINTLN("[WiFi] Dato demasiado grande para la escritura");
        return false; 
    }
    char inchar[maxAlloc]; 
    toSave.toCharArray(inchar, len+1);
    for (int i = 0; i < len; i++) {
    EEPROM.write(address+i, inchar[i]);
    }
    for (int i = len; i < maxAlloc; i++) {
    EEPROM.write(address+i, 255);
    }
    EEPROM.commit();
    return true;
}   

String Manager::getMAC(){
    return this->_MAC;
}