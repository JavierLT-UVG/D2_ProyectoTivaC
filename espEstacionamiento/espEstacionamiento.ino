
//************************************************************************************************
// Librerías
//************************************************************************************************
#include <WiFi.h>
#include <WebServer.h>
//************************************************************************************************
// Variables globales
//************************************************************************************************
// SSID & Password
const char* ssid = "DESKTOP-6QF5U72 6476";  // Enter your SSID here
const char* password = "machorro123";  //Enter your Password here
int parqueos[] = {0,0,0,0,0,0,0,0}; // para guardar si el parqueo esta ocupado o no
int dato = 0;
WebServer server(80);  // Object of WebServer(HTTP port, 80 is defult)


uint8_t LED1pin = 2;
bool LED1status = LOW;

void conexion(void);
//************************************************************************************************
// Configuración
//************************************************************************************************
void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
  Serial.println("Try Connecting to ");
  Serial.println(ssid);

  pinMode(LED1pin, OUTPUT);

  // Connect to your wi-fi modem
  WiFi.begin(ssid, password);

  // Check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected successfully");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());  //Show ESP32 IP on serial

  server.on("/", conexion); // Directamente desde e.g. 192.168.0.8

  
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");
  delay(100);
}
//************************************************************************************************
// loop principal
//************************************************************************************************
void loop() {
  server.handleClient();
  if(Serial2.available()){
    dato= Serial2.read();
    //Serial.println(dato);
  }
  if(dato&128){
    parqueos[0] = 1;
  }else parqueos[0] = 0;
  
  if(dato&64){
     parqueos[1] = 1;
  }else parqueos[1] = 0;
  
  if(dato&32){
    parqueos[2] = 1;
  }else parqueos[2] = 0;
  
  if(dato&16){
    parqueos[3] = 1;
  }else parqueos[3] = 0;
}



//************************************************************************************************
// Handler de Inicio página
//************************************************************************************************
void conexion(){  
  String text = "<!DOCTYPE html> <html>\n" ;
  text +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\"> <meta http-equiv=\"refresh\" content=\"1\" >\n";
  text +="<title>SISTEMA DE PARQUEOS</title>\n";
  text += "<h1 style=\"color: #5e9ca0; text-align: center;\">Machorro-Parqueos SA</h1>\n";
  text += "<h1 style=\"color: #5e9ca0; text-align: center;\">sistema de parqueos automaticos</h1>";
  if (parqueos[0]) text += "<h3 style=\"text-align: center;\">&nbsp;PARQUEO 1: disponible &#128664</h3> ";
  else text += "<h3 style=\"text-align: center;\">&nbsp;PARQUEO 1: ocupado</h3> ";
  
  if (parqueos[1]) text += "<h3 style=\"text-align: center;\">&nbsp;PARQUEO 2: disponible &#128664</h3> ";
  else text += "<h3 style=\"text-align: center;\">&nbsp;PARQUEO 2: ocupado</h3> ";
  
  if (parqueos[2]) text += "<h3 style=\"text-align: center;\">&nbsp;PARQUEO 3: disponible &#128664</h3> ";
  else text += "<h3 style=\"text-align: center;\">&nbsp;PARQUEO 3: ocupado</h3> ";
  
  if (parqueos[3]) text += "<h3 style=\"text-align: center;\">&nbsp;PARQUEO 4: disponible &#128664</h3> ";
  else text += "<h3 style=\"text-align: center;\">&nbsp;PARQUEO 4: ocupado</h3> ";

  text += "</html>";
    
  server.send(200, "text/html", text);
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}
