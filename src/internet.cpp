#include <Arduino.h>
#include <WiFi.h>
#include "internet.h"
#include "senhas.h"


//----------- Configurações do Wi-fi -----------
const unsigned long tempoEsperaConexao = 15000;
const unsigned long tempoEsperaReconexao = 5000;


void conectaWiFi()
{
  Serial.printf("Conectando ao Wi-fi: %s", SSID);

  WiFi.begin(SSID, SENHA); // Tenta se conectar ao saber o nome e a senha do wi-fi

  unsigned long tempoInicial = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - tempoInicial < tempoEsperaConexao)
  {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nWifi conectado com sucesso");
    Serial.println("Endereco IP: ");
    Serial.println(WiFi.localIP()); // Mostrar o Ip do wifi
  }
  else
  {
    Serial.println("\nERROR! Verifique o nome e a senha");
  }
}

void checkWiFi()
{
  unsigned long tempoAtual = millis();
  static unsigned long tempoUltimaConexao = 0;

  if (tempoAtual - tempoUltimaConexao < tempoEsperaConexao)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("\n Coneexao perdida! Tentando conectar Wifi");
      conectaWiFi();
    }
    tempoUltimaConexao = tempoAtual;
  }
}