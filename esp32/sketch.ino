#include <DHT.h>

#define BTN_N 12
#define BTN_P 14
#define BTN_K 27
#define LDR_PIN 34
#define DHT_PIN 15
#define RELAY_PIN 26
#define LED_ALERTA 2

DHT dht(DHT_PIN, DHT22);

const float PH_MIN = 6.0;
const float PH_MAX = 6.5;
const float UMIDADE_MIN = 60.0;
const float UMIDADE_MAX = 80.0;
const float CHUVA_LIMITE = 5.0;
const int NUTRIENTES_MINIMOS = 2;

bool bombaLigada = false;
float chuvaPrevista = 0;
String entradaSerial = "";

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(BTN_N, INPUT_PULLDOWN);
  pinMode(BTN_P, INPUT_PULLDOWN);
  pinMode(BTN_K, INPUT_PULLDOWN);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_ALERTA, OUTPUT);

  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_ALERTA, LOW);

  Serial.println("Irrigacao inteligente - FarmTech (cultura: cafe)");
  Serial.println("Para simular previsao de chuva digite CHUVA:<mm>, ex: CHUVA:12");
}

void lerSerial() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n') {
      entradaSerial.trim();
      if (entradaSerial.startsWith("CHUVA:")) {
        chuvaPrevista = entradaSerial.substring(6).toFloat();
        Serial.print("Chuva prevista atualizada: ");
        Serial.print(chuvaPrevista);
        Serial.println(" mm");
      }
      entradaSerial = "";
    } else {
      entradaSerial += c;
    }
  }
}

void loop() {
  lerSerial();

  bool temN = digitalRead(BTN_N) == HIGH;
  bool temP = digitalRead(BTN_P) == HIGH;
  bool temK = digitalRead(BTN_K) == HIGH;
  int nutrientes = temN + temP + temK;
  bool nutrientesOk = nutrientes >= NUTRIENTES_MINIMOS;

  int leituraLdr = analogRead(LDR_PIN);
  float ph = leituraLdr / 4095.0 * 14.0;

  float umidade = dht.readHumidity();
  bool leituraOk = !isnan(umidade);

  bool phForaDaFaixa = ph < PH_MIN || ph > PH_MAX;
  bool vaiChover = chuvaPrevista >= CHUVA_LIMITE;

  if (leituraOk) {
    if (phForaDaFaixa) {
      bombaLigada = false;
      digitalWrite(LED_ALERTA, HIGH);
    } else {
      digitalWrite(LED_ALERTA, LOW);

      if (vaiChover || !nutrientesOk) {
        bombaLigada = false;
      } else if (umidade < UMIDADE_MIN) {
        bombaLigada = true;
      } else if (umidade >= UMIDADE_MAX) {
        bombaLigada = false;
      }
    }
  }

  digitalWrite(RELAY_PIN, bombaLigada ? HIGH : LOW);

  Serial.println("----------------------------");
  Serial.print("N: "); Serial.print(temN ? "ok" : "falta");
  Serial.print(" | P: "); Serial.print(temP ? "ok" : "falta");
  Serial.print(" | K: "); Serial.println(temK ? "ok" : "falta");

  if (!nutrientesOk) {
    Serial.println("Aviso: nutrientes insuficientes (minimo 2 de 3), irrigacao bloqueada.");
  }

  Serial.print("pH: "); Serial.println(ph, 2);
  if (phForaDaFaixa) {
    Serial.println("Alerta: pH fora da faixa do cafe (6.0 a 6.5)!");
  }

  if (leituraOk) {
    Serial.print("Umidade: "); Serial.print(umidade, 1); Serial.println("%");
  } else {
    Serial.println("Erro ao ler o DHT22.");
  }

  Serial.print("Chuva prevista: "); Serial.print(chuvaPrevista); Serial.println(" mm");
  Serial.print("Bomba: "); Serial.println(bombaLigada ? "LIGADA" : "DESLIGADA");

  delay(2000);
}
