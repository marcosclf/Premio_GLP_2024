#include <MQUnifiedsensor.h>  // Biblioteca para sensores MQ
#include <DHT.h>  // Biblioteca para o sensor de temperatura e umidade DHT

// Definições para o sensor MQ-7
#define Placa ("ESP-8266")
#define Resolucao_Tensao 3.3 // A tensão da ESP8266 é de 3.3V
#define pino A0  // Pino analógico onde o sensor MQ-7 está conectado
#define tipo "MQ-7" // Tipo de sensor
#define Resolucao_ADC 10 // Resolução ADC da ESP8266 (10 bits)
#define RazaoMQ7ArLimpo 27 // Razão do sensor no ar limpo, para o MQ-7
MQUnifiedsensor MQ7(Placa, Resolucao_Tensao, Resolucao_ADC, pino, tipo);

// Definições para o sensor DHT11
#define pinoDHT D5  // Pino ao qual o DHT11 está conectado
#define tipoDHT DHT11 // Tipo do sensor (DHT11)
DHT dht(pinoDHT, tipoDHT);

// Definições dos LEDs e Buzzer
const int pinoLedVerde = D1;  // LED Verde
const int pinoLedAmarelo = D2; // LED Amarelo
const int pinoLedVermelho = D3; // LED Vermelho
const int pinoBuzzer = D4;    // Buzzer

void setup() {
  Serial.begin(115200);  // Inicializa a comunicação serial a 115200 bps
  pinMode(pinoLedVerde, OUTPUT);   // Configura o pino do LED verde como saída
  pinMode(pinoLedAmarelo, OUTPUT); // Configura o pino do LED amarelo como saída
  pinMode(pinoLedVermelho, OUTPUT); // Configura o pino do LED vermelho como saída
  pinMode(pinoBuzzer, OUTPUT);     // Configura o pino do buzzer como saída

  // Inicializa o sensor MQ-7
  MQ7.setRegressionMethod(1); // Define o método de regressão (1 = exponencial)
  MQ7.setA(99.042); // Coeficiente de regressão para CO (consulte o datasheet do sensor utilizado)
  MQ7.setB(-1.518); // Coeficiente de regressão para CO (consulte o datasheet do sensor utilizado)
  MQ7.init(); // Inicializa o sensor

  // Calibração do sensor MQ-7
  Serial.print("Calibrando o sensor MQ-7...");
  MQ7.calibrate();  // Calibra o sensor com base nas leituras do ar limpo
  Serial.println("Calibração concluída");
  
  // Definir a resistência de carga (RL) do sensor MQ-7
  MQ7.setRL(10); // RL = 10k ohms (consulte o datasheet do sensor utilizado)

  // Inicializa o sensor DHT11
  dht.begin();
  Serial.println("Sensor DHT11 inicializado");
}

void loop() {
  // Leitura do sensor MQ-7 e cálculo de PPM
  MQ7.update(); // Atualiza as leituras do sensor MQ-7
  float ppm = MQ7.readSensor(); // Leitura da concentração de CO em PPM (consulte o datasheet do sensor utilizado)

  // Leitura da temperatura e umidade do sensor DHT11
  float temperatura = dht.readTemperature();  // Leitura da temperatura em Celsius
  float umidade = dht.readHumidity();         // Leitura da umidade em porcentagem
  
  // Exibe as leituras no monitor serial
  Serial.print("Concentração de CO: ");
  Serial.print(ppm);
  Serial.println(" PPM");
  
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println(" °C");

  Serial.print("Umidade: ");
  Serial.print(umidade);
  Serial.println(" %");

  // Lógica de alerta com LEDs e Buzzer para o sensor MQ-7
  if (ppm >= 0 && ppm <= 15) {
    digitalWrite(pinoLedVerde, HIGH);
    digitalWrite(pinoLedAmarelo, LOW);
    digitalWrite(pinoLedVermelho, LOW);
    digitalWrite(pinoBuzzer, LOW); // Buzzer desligado
  } 
  else if (ppm >= 16 && ppm <= 32) {
    digitalWrite(pinoLedVerde, LOW);
    digitalWrite(pinoLedAmarelo, HIGH);
    digitalWrite(pinoLedVermelho, LOW);
    digitalWrite(pinoBuzzer, LOW); // Buzzer desligado
  } 
  else if (ppm >= 33 && ppm <= 40) {
    digitalWrite(pinoLedVerde, LOW);
    digitalWrite(pinoLedAmarelo, LOW);
    digitalWrite(pinoLedVermelho, HIGH);
    digitalWrite(pinoBuzzer, LOW); // Buzzer desligado
  } 
  else if (ppm > 40) {
    digitalWrite(pinoLedVerde, LOW);
    digitalWrite(pinoLedAmarelo, LOW);
    digitalWrite(pinoLedVermelho, HIGH);
    digitalWrite(pinoBuzzer, HIGH); // Buzzer ligado
  }

  // Lógica de alarme para temperatura e umidade com o sensor DHT11
  if (temperatura > 35 || umidade > 80) {
    // Se a temperatura for maior que 35°C ou a umidade maior que 80%, ativa o buzzer
    digitalWrite(pinoBuzzer, HIGH);
    Serial.println("ALERTA: Temperatura ou umidade elevada!");
  } else {
    digitalWrite(pinoBuzzer, LOW); // Buzzer desligado caso não haja alerta
  }

  delay(1000);  // Aguarda 1 segundo antes de fazer a próxima leitura
}

// Este código faz parte do projeto submetido ao prêmio GLP.
// Ele visa difundir a ideia sobre o uso de dispositivos IoT de baixo custo. 
// Para aplicação em ambientes reais, o sistema deve ser testado em um ambiente controlado, 
// junto com equipamentos certificados e calibrados para validar a usabilidade do conjunto.
// Pode ser melhorado e utilizado por terceiros, apenas cite os autores no documento e consulte o datasheet onde aplicável.
