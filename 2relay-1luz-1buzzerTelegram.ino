#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

//Reemplzar por datos de la red
const char* ssid = "TP-LINK_4AB6";  //Nombre de la red de WiFi
const char* password ="Teresita18"; // //Contraseña de la red de WiFi

//Inicializacion del Bot de Telegram
#define BOTtoken "5007800283:AAHL-w43iiQogfWuzw4eOd4T-4bOmH9FJ3E"  //Escribir token

//ID de chat
#define CHAT_ID "1107422980"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

//Chequea los mensajes cada 1 segundo
const unsigned long tiempo = 1000; //tiempo medio entre mensajes de escaneo
unsigned long tiempoAnterior; //última vez que se realizó el análisis de mensajes

//definicion de los pines
const int luz1 = 14;
const int relay1Afuera = 12;
const int relay2Adentro = 13;
const int buzzer = 15;

//estado inicial del pin
bool estadoLuz1 = LOW;
bool estadoRelay1Afuera = LOW;
bool estadoRelay2Adentro = LOW;

int inicio = 1;


void setup() {
  Serial.begin(115200);

  client.setInsecure();

  pinMode(luz1, OUTPUT);
  pinMode(relay1Afuera, OUTPUT);
  pinMode(relay2Adentro, OUTPUT);
  pinMode(buzzer, OUTPUT);
  digitalWrite(luz1, LOW);
  digitalWrite(relay1Afuera, HIGH); //logica inversa para el relay
  digitalWrite(relay2Adentro, HIGH); //logica inversa para el relay
  
  //Conexion a red de WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi.. \n");
  }
  Serial.print("Conectado a la red wifi. Dirección IP: ");
  Serial.println(WiFi.localIP());
  if(inicio == 1){
    Serial.println("Sistema preparado");
    bot.sendMessage(CHAT_ID, "Sistema preparado!!!, escribe /Ayuda para ver las opciones", "");//Enviamos un mensaje a telegram para informar que el sistema está listo
    inicio = 0;
    }
}

void loop() {
  
  //Verifica si hay datos nuevos en telegram cada 1 segundo
  if (millis() - tiempoAnterior > tiempo)
  {
    int numerosMensajes = bot.getUpdates(bot.last_message_received + 1);

    while (numerosMensajes)
    {
      Serial.println("Comando recibido");
      handleNewMessages(numerosMensajes);
      numerosMensajes = bot.getUpdates(bot.last_message_received + 1);
    }

    tiempoAnterior = millis();
  }
}

//Maneja lo que sucede cada vez q recibe un mensaje
void handleNewMessages(int numNewMessages) {

  for (int i=0; i<numNewMessages; i++) {
 
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Usuario no autorizado", "");
      continue;
    }
    
    //Imprime el mensaje recibido
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;


    //encender reflector afuera
    if (text == "/afuera_on") {
      bot.sendMessage(chat_id, "El reflector exterior está encendido", "");
      estadoRelay1Afuera = LOW;
      digitalWrite(relay1Afuera, estadoRelay1Afuera);
      digitalWrite(buzzer, HIGH);
      delay(500);
      digitalWrite(buzzer, LOW);
    }

    //apagar reflector afuera
    if (text == "/afuera_off") {
      bot.sendMessage(chat_id, "El reflector exterior está apagado", "");
      estadoRelay1Afuera = HIGH;
      digitalWrite(relay1Afuera, estadoRelay1Afuera);
      digitalWrite(buzzer, HIGH);
      delay(500);
      digitalWrite(buzzer, LOW);
    }

     //encender luz adentro
    if (text == "/adentro_on") {
      bot.sendMessage(chat_id, "La luz principal interior está encendida", "");
      estadoRelay2Adentro = LOW;
      digitalWrite(relay2Adentro, estadoRelay2Adentro);
      digitalWrite(buzzer, HIGH);
      delay(500);
      digitalWrite(buzzer, LOW);
    }

    //apagar luz adentro
    if (text == "/adentro_off") {
      bot.sendMessage(chat_id, "La luz principal interior está apagada", "");
      estadoRelay2Adentro = HIGH;
      digitalWrite(relay2Adentro, estadoRelay2Adentro);
      digitalWrite(buzzer, HIGH);
      delay(500);
      digitalWrite(buzzer, LOW);
    }

     //encender luz auxiliar
    if (text == "/luz1_on") {
      bot.sendMessage(chat_id, "La luz auxiliar interior está encendida", "");
      estadoLuz1 = HIGH;
      digitalWrite(luz1, estadoLuz1);
      digitalWrite(buzzer, HIGH);
      delay(500);
      digitalWrite(buzzer, LOW);
    }

    //apagar luz auxiliar
    if (text == "/luz1_off") {
      bot.sendMessage(chat_id, "La luz auxiliar interior está apagada", "");
      estadoLuz1 = LOW;
      digitalWrite(luz1, estadoLuz1);
      digitalWrite(buzzer, HIGH);
      delay(500);
      digitalWrite(buzzer, LOW);
    }

    //Estado del led
    if (text == "/Estado") {
      String mensaje = "El estado de las luces es el siguiente: \n";
      if (digitalRead(relay1Afuera)){
        mensaje += "El reflector exterior está apagado\n";
      }
      else{
        mensaje += "El reflector exterior está encendido\n";
      }
      if (digitalRead(relay2Adentro)){
        mensaje += "La luz principal interior está apagada\n";
      }
      else{
        mensaje += "La luz principal interior está encendida\n";
      }
      if (digitalRead(luz1)){
        mensaje += "La luz auxiliar interior está encendida\n";
      }
      else{
        mensaje += "La luz auxiliar interior está apagada\n";
      }
      bot.sendMessage(chat_id, mensaje, "");
    }

    if (text == "/Ayuda") {
      String welcome = "Bienvenido al sistema de control de luces de Mony, " + from_name + ".\n";
      welcome += "Los siguientes comandos permiten controlar las tres luces disponibles de la casa.\n\n";
      welcome += "/afuera_on para encender el reflector de afuera \n";
      welcome += "/afuera_off para apagar el reflector de afuera \n";
      welcome += "/adentro_on para encender la luz principal interior \n";
      welcome += "/adentro_off para apagar la luz principal interior \n";
      welcome += "/luz1_on para encender la luz auxiliar interior \n";
      welcome += "/luz1_off para apagar la luz auxiliar interior \n";
      welcome += "/Estado muestra el estado de todas las luces \n";
      welcome += "/Ayuda accede a este menú \n";
      bot.sendMessage(chat_id, welcome, "");
    }
  }
}
