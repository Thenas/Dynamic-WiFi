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

#ifndef DynamicWiFi_h
    #define DynamicWiFi_h   

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
#include <DNSServer.h>

#define DNS_PORT 53
#define SSID_ADDRESS 0
#define MAX_ALLOC_SSID (SSID_ADDRESS + 31) 
#define PASS_ADDRESS (MAX_ALLOC_SSID + 1)
#define MAX_ALLOC_PASS (PASS_ADDRESS + 63)

#define QUALITY_0   0 
#define QUALITY_25  1 
#define QUALITY_50  2 
#define QUALITY_75  3
#define QUALITY_100 4

#define CONFIG_SSID "ModuleConfig"
#define CONFIG_PASS "123456789"

#ifdef DEBUG_WIFI
    #define DEBUG_PRINTLN(x) Serial.println(x)
    #define DEBUG_PRINT(x) Serial.print(x)
#else 
    #define DEBUG_PRINTLN(X) 
    #define DEBUG_PRINT(X)
#endif 

const char  IMG_0 [] PROGMEM = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAPCAQAAABOkSfeAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAAG5SURBVCjPY2AAg1DmCHGnQNM+k10mV0zvG983umyyz2R6gE+eOAMSYAyRNk81OmX00fif8X8g/gumgdDot8lF2+x4CbCqNNYYD9O9xj+M/5l8Nj5nutCizCbTNsOuyGSJ6SXjn2CNxyNcGRgZbFOM7wM5n0w3OMclSUXzxYjFKMYo5orWM+WJB7mbbQca8d/knVsig8lZ43+mt+0zvSTClS1SLJeangNqfGB80WKje0KcdC67c5zJQ6C5NxlMy83XublGipgnGp8BOQDZjSbXHVMLOUONTdebdYBcyRUrbDLR+IPxP6M3pvtNelyzndLNOkz3G30DKv5pMTOLp54ljRXsoQAzo7dGv8wPeYZHiGcLpyukyGTxFHKGOZqeAGr+6uEMD580fsvJJpP91GPlgguNDxrfMrlvfDSoMl4iWdxiosXMICWUsExjDdQ3Afnxn9EfsAv/mR2JVWNgKOQEBg0qMK42+mP0yWyVXb5HmsUWYBj+M2vHUAQCMfrGa/2KQkRz+eI5ctmt0003+TgzYAdprGmsHnpma8y2p4AtrWdiwA3cE4Bu/OdRwUAIRKub7jc97GuNLg4AngypjQceONsAAAAASUVORK5CYII=";
const char  IMG_25[] PROGMEM = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAPCAQAAABOkSfeAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAAG7SURBVCjPY2AAg1DmCHGnQNM+k10mV0zvG983umyyz2R6gE+eOAMSYAyRNk81OmX00fif8X8g/gumgdDot8lF2+x4CbCqNNYYD9O9xj+M/5l8Nj5nutCizCbTNsOuyGSJ6SXjn2CNxyNcGRgZbFOM7wM5n0w3OMclSUXzxYjFKMYo5orWM+WJB7mbbQca8d/knVsig8lZ43+mt+0zvSTClS1SLJeangNqfGB80WKje0KcdC67c5zJQ6C5NxlMy83XublGipgnGp8BOQDZjSbXHVMLOUONTdebdYBcyRUrbDLR+IPxP6M3pvtNelyzndLNOkz3G30DKv5pMTOLp54ljRXsoQAzo7dGv8wPeYZHiGcLpyukyGTxFHKGOZqeAGr+6uEMD580fsvJJpP91GPlgguNDxrfMrlvfDSoMl4iWdxiosXMICWUsExjDdQ3Afnxn9EfsAv/mR2JVWNgKOQEBg0qMK42+mP0yWyVXb5HmsUWYBj+M2vHUAQCMfrGa/2KEkUn8s3nyGW3Tjfd5OPMgB2ksaaxrtY7vObw9nVgS+uZGHCDnQmH/hz6d7CCgRDYoH5o/6HDW63RxQE546+mc2I9tAAAAABJRU5ErkJggg==";
const char  IMG_50[] PROGMEM = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAPCAQAAABOkSfeAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAAHRSURBVCjPY2AAg3qWTFmbGJNpRgeMrhs9MnpkcsvoiMkcj9BoGQYkwJiiaFVofMbou/E/EDT6Y/zX+D8Y/jO5HlQTJw1WFc/hGmhyzPiH0T+T98anjWZbFLqkeCba5JnNNz5n/A2k3PSCu389E4NtttFj478mHyyWOQdHiKfxx4jFKMYoZoiFMlcIBzpZrTP+AVT82beAwegm0IJr1snJ4pEa5qkmq0wuGt83fmB02XyzU3qaXD2be6TpHeP/Jg8ZLJosV3rYR4pYJhmdN/4JchfQhf8gbjS9651ZzxWjY7rGrAfoyjSuGDHTKUYfgd54Y7rfpMc12yndrAPI+go06Zf5wly+VcxprGAPeTkafzD6ZbrfKTRDbKHwfIVZMlN4CjmjHUyPA83+4eoLD580EcvZJpMiVZfIHy86fODwrUP3Dx09VDZfIkbMYoL5nBB1lLBMY91icGjXoR+H/h36e/j/4f9A+tgmDQaGQk4GRgZUcLDh8J/Dnw+v31u4M+vQ1kM/D/073PEfXREIbDU5tPFwzUrRObzzOSayH88/tO2AJwN2UM82k3WtzqGVh7YuV2Zg6OUExgdusD3h0J9D/w5WMBACazQOHz10fLstujgAnT7FefT+Dw8AAAAASUVORK5CYII=";
const char  IMG_75[] PROGMEM = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAPCAQAAABOkSfeAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAAHVSURBVCjPY2AAg3qWTFmbGJNpRgeMrhs9MnpkcsvoiMkcj9BoGQYkwJiiaFVofMbou/E/EDT6Y/zX+D8Y/jO5HlQTJw1WFc/hGmhyzPiH0T+T98anjWZbFLqkeCba5JnNNz5n/A2k3PSCu389E4NtttFj478mHyyWOQcni3fwTxNbpLhIcZrYKuYK4UAnq3XGP4CKP/sWMBjdBFpwzTq5UnyNxt7Ug6sOXzx8/9CDQ5cPb96ZPlOuns090vSO8X+ThwwWTZYrPex7RPYmHTp/6Ofh/4f+Hfp76N/h/yB46O7+zJlcMTqma8x6gK5M45omdmjKoY9AJW8O7T/Usy97Z/rBjoP7D30FKv11aOFEvlXMaaxgD21wPPwBKLR/V+hasYXC8xVmyUzh6eXc7nDoONDsH9t84eEzU+Tw7MOTNquukT9edPjA4VuH7h86eqhsvgTQpgmH5qxURwrK/4wzWbcYHNp16AfIjWD3/Tt0bJMGA0Mv539GBlRwsOHwn8OfD6/fW7gz69DWQz+B1nZgKAKBrSaHNh6uWSk6h3c+x0T24/mHth3wZMAO6tlmsq7VObTy0NblyiBLgfGBG2xPOPTn0L+DFQyEwBqNw0cPHd9uiy4OAO645bTcCT0YAAAAAElFTkSuQmCC";
const char IMG_100[] PROGMEM = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAPCAQAAABOkSfeAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAAHESURBVBgZhcFNSJNxHAfw37SZk14O2mOQvYyIdvDQoWv0cusQEeEh6NArUkK1DtFBeKKTBwdZMnh08OQgY2PLls61Jx635/ttDip6IUiSwrJrkCHVlO3/D4mQ3ujzkR/MFSMbC0cZZRFTnOUsp/kQsUzHQJss075M0A3zCb9BQUGxiho1NTUUpkrdNzfIEruxeAiTrEDxk/eYg27YOeUcz5+jzaf4Sk2N52MHzTrxuvgBNc5x+MHh4VZrbdyIB+PBqJGsv9Gc3oc7rFBz/tEF4Wsovpo4mWm9H3JPe0m+4Aze4SVH853WJrMhdwRvqPFecBUJZ/etFvcEnmGBGgo1KGpqarwtnLGa0u1MoVdErKa4gX58hsJHFNA70ZXv9Hq8Ar5QYxFDfWuS9ZZfltzdyzksouB0pI2hZnvLQFv/qkggtwdlKFbGD8hPVgsHeX10W2pz+SKLnMYMSrhkr48auIZYYrss0z7LP7YDDipQqFFTQ2HyXkgkEtA++ZV3hVXOc8QN588iiwUo9mif/Cm7Exl2J9bFVtuNfSvL5zFe3C9/ZzZY/nQ7Esje3ioSCZh18m+5Y6hCeZflf1IhllDO7ZLffAffZw2HpXzU9QAAAABJRU5ErkJggg==";




const char HTML_HEADER[] PROGMEM =
"<!DOCTYPE html>"
"<html lang=\"en\">"
"<head>"
"<meta charset=\"UTF-8\">"
"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
"<title>Configuracion WiFi</title>"
"<style>"
"html{"
"background-color: rgb(11,11,11);"
"color: rgb(225,225,225);"
"font-family: Arial, Helvetica, sans-serif;"
"}"
".s1{"
"border-top: 3px solid rgb(225,225,225);"
"border-bottom: none;"
"border-radius: 10px;"
"}"
"table, th, td {"
"border-collapse: collapse;"
"}"
"th, td {"
"padding: 5px 5px;"
"}"
"th {"
"text-align: left;"
"}"
"table{"
"width: 100%;"
"}"
"</style>"
"<script type=\"text/javascript\">"
"function getSSID(id){"
"var element = document.getElementById(id);"
"var ssid = element.textContent;"
"var nameSSID = document.getElementById(\"ssid\");"
"var button = document.getElementById(\"btn\");"
"var h2SSID = document.getElementById(\"nameSSID\");"
"button.textContent = \"regresar\";"
"nameSSID.value = ssid;"
"h2SSID.textContent = ssid;"
"changeState();"
"} "
"function changeState(){"
"var div = document.getElementById(\"listSSID\");"
"var form = document.getElementById(\"form\");"
"if(div.style.display == \"none\") div.style.display = \"block\";"
"else div.style.display = \"none\";"
"if(form.style.display == \"none\") form.style.display = \"block\";"
"else form.style.display = \"none\";"
"}"
"</script>"
"</head>";

const char HTML_BODY[] PROGMEM =

"<body>"
"<h1>WiFi</h1>"
"<a href=\"/\"><button id =\"btn\">actualizar</button></a>"
"<hr class= \"s1\">"
"<div id = \"listSSID\" >"
"<table>"
"<tr>"
"<th>Nombre de red</th>"
"<th>Intensidad</th>"
"</tr>";


const char SSID_TEMPLATE[] PROGMEM = 
"<tr>"
"<td id = \"{i}\" onclick=\"getSSID(id)\" >{name}</td>"
"<td><img src={img} ></td>"
"</tr>";


const char HTML_TABLE_END[] PROGMEM =
"</table>"
"</div>";

const char HTML_FORM[] PROGMEM = 
"<form action = \"save\" id = \"form\" style=\"display: none;\" >"
"<h2 id=\"nameSSID\">df</h2>"
"<input type=\"password\" name=\"pass\" id=\"pass\">"
"<label for=\"pass\">ContraseÃ±a</label>"
"<br>"
"<input type=\"submit\" value=\"Conectar\">"
"<input id = \"ssid\" type=\"text\" name =\"ssid\" value = \"test\" hidden>"
"</form>";

const char HTML_END[] PROGMEM =
"</body>"
"</html>";


class Manager
{

    public:
        Manager();
        ~Manager();
        IPAddress getIP();
        boolean tryConnect();
        boolean configurateMode();
        String readEEPROM(int address, int maxAlloc);
        bool   writeEEPROM(int address, int maxAlloc, String toSave);
        boolean isConected;
        String getMAC();
        

        
       
    private:
        
        IPAddress _IP;
        String _MAC;
        int nAvailableNetworks;
        int8_t WiFiStaus;

        std::unique_ptr<DNSServer>        dnsServer;
        std::unique_ptr<ESP8266WebServer> server;
        void   saveCredentials();
        String RSSI2IMG(int index);

        void   serverOn();
        void   serverConnect();
        void   serverSearch();
        bool   setupWiFi();
};
#endif /*DynamicWiFi_h*/