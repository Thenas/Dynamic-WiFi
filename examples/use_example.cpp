#include <Arduino.h>
#include <DynamicWiFi.h>
#define LED 2

Manager WiFiManager;


void setup(){
    pinMode(LED,OUTPUT);
    Serial.begin(9600);
}

void loop(){
    if(!WiFiManager.isConected){
        digitalWrite(LED,HIGH); //logica negativa
        WiFiManager.tryConnect();
    }
    digitalWrite(LED,LOW); //logica negativa
    Serial.print("[MAIN] Direccion IP: ");
    Serial.println(WiFiManager.getIP());
    Serial.print("[MAIN] Direccion MAC: ");
    Serial.print(WiFiManager.getMAC());
    delay(2000);
}