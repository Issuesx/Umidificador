// INCLUSÃO DE BIBLIOTECAS
#include "DHT.h"

// DEFINIÇÕES DE PINOS
#define pinDHT 7
#define pinUmid 8
#define pinAr 9

// DEFINIÇÕES
#define UR_MAX 60    // Umidade Relativa Maxima
#define UR_IDEAL 55  // Umidade Alvo
#define UR_MIN 50    // Umidade Relativa Minima

#define INTERVALO_LEITURA 2000  // Intervalo minimo de leitura do DHT22 ~2s

// INSTANCIANDO OBJETOS
DHT dht(pinDHT, DHT22);

// DECLARAÇÃO DE VARIÁVEIS
float umidade;

int estado = 0;  // 0 = Ar ideal | -1 = Umidificador ligado | 1 = Ar condicionado Ligado
unsigned long controleLeitura;

void setup() {
  Serial.begin(9600);

  pinMode(pinAr, OUTPUT);
  pinMode(pinUmid, OUTPUT);

  dht.begin();

  Serial.println("Fim do setup()");
}

void loop() {

  if (millis() - controleLeitura > INTERVALO_LEITURA) {
    float leitura = dht.readHumidity();

    if (!isnan(leitura)) { // Confere se recebeu algum valor do sensor
      umidade = leitura;

      Serial.print(F("Umidade: "));
      Serial.print(umidade);
      Serial.println(F("%"));

      controleLeitura = millis();
    } else {
      Serial.println(F("Falha na leitura do DHT"));
    }
  }

  switch (estado) {
    case 0:                    // Leitura Anterior = Indicando Umidade ideal | Umidificador e Ar desligados
      if (umidade < UR_MIN) {  // Se Leitura Atual = Indicando Ar muito Seco
        estado = -1;
        digitalWrite(pinUmid, HIGH);
      } else if (umidade > UR_MAX) {  // Se Leitura Atual = Indicando Ar muito umido
        estado = 1;
        digitalWrite(pinAr, HIGH);
      }
      break;

    case -1:                       // Leitura Anterior = Indicando Umidade baixa | Umidificador ligado
      if (umidade >= UR_IDEAL) {  // Se Leitura Atual = Indicando Umidade subiu para o nível ideal
        estado = 0;
        digitalWrite(pinUmid, LOW);
      }
      break;

    case 1:                       // Leitura Anterior = Indicando Umidade alta | Ar ligado
      if (umidade <= UR_IDEAL) {  // Se Leitura Atual = Indicando Umidade abaixou para o nível ideal
        estado = 0;
        digitalWrite(pinAr, LOW);
      }
      break;
  }
}
