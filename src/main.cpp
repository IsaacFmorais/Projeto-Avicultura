/*
Autor: Isaac Fagundes Morais
Data: 24/06/2025
Nome: LCD de Avicultura
Descrição: Pega informações do MQTT em um Broker AWS 
*/

#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "internet.h"
#include "certificados.h"
#include <LiquidCrystal_I2C.h>

void conectaMqtt(void);
void Callback(char *, byte *, unsigned int);

const int mqtt_port = 8883;

const char *mqtt_id = "Isaac";
const char *mqtt_SUB = "senai134/mesa02";
const char *mqtt_PUB = "senai134/mesa02";

WiFiClientSecure espClient;
PubSubClient mqtt(espClient);
LiquidCrystal_I2C lcd(0x27, 20, 4);
JsonDocument doc;
String msg;
bool porta = 0;
int temperatura = 0;
int distancia;
bool portaAnterior=1;
bool estadoCheio = 0;
bool estadoMedia = 0;
bool estadoBaixo = 0;

void setup()
{
  Serial.begin(9600);
  conectaWiFi();
  espClient.setCACert(AWS_ROOT_CA);
  espClient.setCertificate(AWS_CERT);
  espClient.setPrivateKey(AWS_KEY);
  mqtt.setServer(AWS_BROKER, mqtt_port);
  lcd.init();
  lcd.backlight();
  mqtt.setCallback(Callback);
  lcd.setCursor(7, 0);
  lcd.print ("SENAI");
  lcd.setCursor(0, 1);
  lcd.print("Graus:");
  lcd.setCursor(0, 2);
  lcd.print("Agua:");
  lcd.setCursor(0, 3);
  lcd.print("Portas: Fechado");


}

void loop()
{
  checkWiFi();

  if (!mqtt.connected())
  {
    conectaMqtt();
  }

  mqtt.loop();

  //CODIGO DE PUBLICAÇÃO
  // static unsigned long tempo_pub = 0;
  // unsigned long agora = millis();

  // if (agora - tempo_pub > 2000)
  // {
  //   JsonDocument doc;

  //   doc["msg"] = "Eai compade";

  //   String mensagem;
  //   serializeJson(doc, mensagem);
  //   mqtt.publish(mqtt_PUB, mensagem.c_str());
  //   tempo_pub=agora;
  // }
}
void Callback(char *topic, byte *payload, unsigned int lenght)
{
  String msg((char *)payload, lenght);
  Serial.printf("Menesagem recebida (topico: [%s]): %s \n\r", topic, msg.c_str());

  deserializeJson(doc, msg);

  if (!doc.isNull())
  {
    if (!doc["porta"].isNull())
      porta = doc["porta"];

    if (!doc["temperatura"].isNull())
      temperatura = doc["temperatura"];

    if (!doc["distancia"].isNull())
      distancia = doc["distancia"];
    
    if (temperatura > 9)
    {
      lcd.setCursor(7, 1);
      lcd.printf("%d", temperatura);
    }
    else if (temperatura < 10 && temperatura > 0)
    {
      lcd.setCursor(8, 1);
      lcd.print(" ");
      lcd.setCursor(7, 1);
      lcd.printf("%d ", temperatura);
    }
    // lcd.setCursor(0, 2);
    // lcd.print("            ");
    // lcd.setCursor(0, 2);
    // lcd.print(distancia);

    if (distancia >= 80)
    {
      if (estadoBaixo == 0)
      {
      lcd.setCursor(6, 2);
      lcd.print("            ");
      lcd.setCursor(6, 2);
      lcd.print("Alrt.Baixo");
      estadoBaixo = 1;
      estadoCheio = 0;
      estadoMedia = 0;
    }
    }

   else if (distancia<= 79 && distancia>=40)
    {
       if (estadoMedia == 0)
      {
      lcd.setCursor(6, 2);
      lcd.print("            ");
      lcd.setCursor(6, 2);
      lcd.print("Media");
      estadoBaixo = 0;
      estadoCheio = 0;
      estadoMedia = 1;
      }
      }

   else if (distancia <=39)
    {
       if (estadoCheio == 0)
       {
      lcd.setCursor(6, 2);
      lcd.print("            ");
      lcd.setCursor(6, 2);
      lcd.print("Cheio");
      estadoBaixo = 0;
      estadoCheio = 1;
      estadoMedia = 0;
      }
    }

    if (porta == 1 && !portaAnterior)
    {
      lcd.setCursor(7, 3);
      lcd.print(" Aberta     ");
      portaAnterior = porta;
    }
    else if (!porta && portaAnterior)
    {
      lcd.setCursor(7, 3);
      lcd.print(" Fechada   ");
      portaAnterior = porta;
    }
  }

  }


void conectaMqtt()
{
  while (!mqtt.connected())
  {
    Serial.print("Conectando ao AWS IoT Core.....");

    if (mqtt.connect(mqtt_id))
    {
      Serial.println("CONENCTADO!!!!");
      mqtt.subscribe(mqtt_SUB);
    }

    else
    {
      Serial.printf("Falhou (%d), tentando de novo em 5s", mqtt.state());
      delay(5000);
    }
  }
}