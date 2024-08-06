#include <ArduinoJson.h>



// Inclui as bibliotecas necessárias para comunicação com o chip PN5180
#include <PN5180.h>
#include <PN5180ISO15693.h>
#include <WiFi.h>
#include <IOXhop_FirebaseESP32.h>

// Define os pinos utilizados para comunicação com o chip PN5180
#define PN5180_NSS  5   // Pino de seleção do chip
#define PN5180_BUSY 16  // Pino de status ocupado
#define PN5180_RST  17  // Pino de reset

#define FIREBASE_HOST "https://glucose-now-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "AIzaSyAVpDSYVBYGwMHvcrNsOGXo7vN6HAhMybs"

#include "Ada.h"

// Cria uma instância do objeto PN5180ISO15693 com os pinos definidos
PN5180ISO15693 nfc(PN5180_NSS, PN5180_BUSY, PN5180_RST);

// Array para armazenar o UID do sensor
uint8_t uid[8];

// Variável para armazenar o código de erro das operações ISO15693
ISO15693ErrorCode rc;

String GURIA = "ada";
String URL_GURIA = "/glucose/" + GURIA;
String URL_HISTORY_GURIA = "/glucose/" + GURIA + "/history";

// Constantes para a estrutura de dados FRAM do sensor
const int TREND_START      = 28;         // Início dos dados de tendência
const int HISTORY_START    = 124;        // Início dos dados históricos
const int FRAM_RECORD_SIZE = 6;          // Tamanho de cada registro

const char* WIFI_SSID      = "Unknow"; // WiFi rede Digitize/Unknow
const char* WIFI_PASSWORD  = "00987654321"; // WiFi senha 01735486/00987654321
bool isWifiConected = false;

// Array para armazenar os dados lidos do sensor (43 blocos * 8 bytes por bloco)
uint8_t sensorData[344];

// Estrutura para armazenar os dados de leitura do sensor
struct ReadingData {
  unsigned long rawTimestamp;  // Timestamp bruto da leitura
  int rawGlucose;              // Valor bruto da glicose
  unsigned long sensorTime;    // Tempo do sensor
  int trend;                   // Tendência da glicose
};

void setup() {
  // Inicializa a comunicação serial
  Serial.begin(115200); 
  Serial.println("ESP32 FreeStyle Libre Leitor.\n\nConectado a rede. ");

  setupWiFi();

  cleanMonitor();
  Serial.println("Rede conectada!");

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  //Serial.println(Firebase.getString("/ada/"));
  //Firebase.setString("/glucose/ada/dextro", "125");
  //delay(5000);

  // Inicializa o leitor NFC
  Serial.println("Inicializando o leitor...");
  nfc.begin();
  Serial.println("Campo NFC redefinido.");
  nfc.reset();
  Serial.println("Campo NFC Habilitando.");
  nfc.setupRF();

  Serial.println("Pronto...");

  // Lê e exibe a versão do firmware
  uint8_t firmwareVersion[2];
  nfc.readEEprom(FIRMWARE_VERSION, firmwareVersion, sizeof(firmwareVersion));
  Serial.print(F("VS Firmware: "));
  Serial.print(firmwareVersion[1]);
  Serial.print(".");
  Serial.println(firmwareVersion[0]);

  // Verifica se a inicialização foi bem-sucedida
  if (firmwareVersion[0] == 0) {
    Serial.println(F("Falha na inicialização!"));
    while (1) {};  // Loop infinito se falhar
  }
}

void loop() {

  
  // Solicita aproximação do sensor
  Serial.println("Aproxime o sensor FreeStyle Libre...");
  
  // Tenta obter o inventário (UID) do sensor
  rc = nfc.getInventory(uid);
  if (rc == ISO15693_EC_OK) {
    Serial.println("Sensor encontrado. Lendo dados...");

    // Lê o bloco de validação para verificar se o sensor está ativo
    uint8_t blockValidadeActive[8];
    nfc.readSingleBlock(uid, 0x00, blockValidadeActive, sizeof(blockValidadeActive));
    if(isSensorActive(blockValidadeActive[4])){
      // Se o sensor estiver ativo, lê os dados
      if (readSensorData()) {
        ReadingData reading = parseData();
        printReadingData(reading);
      }
    } else {
      Serial.println("Sensor inativo!");
    }
  } else {
    // Exibe erro se não conseguir ler o inventário
    Serial.print("Erro na leitura do inventário: ");
    Serial.println(nfc.strerror(rc));
  }
  
  // Aguarda 1,5 segundos antes da próxima tentativa
  delay(1500);
}

// Função para ler todos os dados do sensor
bool readSensorData() {
  uint8_t blockBuffer[8];
  for (int i = 0; i < 43; i++) {  // Lê todos os 43 blocos
    rc = nfc.readSingleBlock(uid, i, blockBuffer, sizeof(blockBuffer));
    if (rc != ISO15693_EC_OK) {
      // Exibe erro se falhar na leitura de um bloco
      Serial.print("Erro na leitura do bloco ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(nfc.strerror(rc));
      return false;
    }
    // Copia os dados lidos para o array sensorData
    memcpy(sensorData + (i * 8), blockBuffer, 8);
  }
  return true;
}

// Função para analisar os dados lidos do sensor
ReadingData parseData() {
  ReadingData reading;

  // Extrai o tempo do sensor dos bytes 317-320
  reading.sensorTime = (unsigned long)sensorData[317] | ((unsigned long)sensorData[318] << 8) | 
                       ((unsigned long)sensorData[319] << 16) | ((unsigned long)sensorData[320] << 24);
  
  // Calcula índices para dados de tendência e histórico
  int indexTrend = reading.sensorTime % 16;
  int indexHistory = reading.sensorTime / 15;

  // Obtém o valor de glicose, timestamp e tendência
  reading.rawGlucose = getGlucoseValue(indexTrend);
  reading.rawTimestamp = millis();
  reading.trend = calculateTrend(indexTrend);

  return reading;
}

// Função para obter o valor de glicose de um índice específico
int getGlucoseValue(int index) {
  int trendIndex = TREND_START + (index * FRAM_RECORD_SIZE);
  return (int)sensorData[trendIndex] | ((int)sensorData[trendIndex + 1] << 8);
}

// Função para calcular a tendência da glicose
int calculateTrend(int currentIndex) {
  int recentValues[3];
  for (int i = 0; i < 3; i++) {
    int index = (currentIndex - i + 16) % 16;
    recentValues[i] = getGlucoseValue(index);
  }

  // Compara valores recentes para determinar a tendência
  if (recentValues[0] > recentValues[2]) {
    return 1;  // Subindo
  } else if (recentValues[0] < recentValues[2]) {
    return -1;  // Descendo
  } else {
    return 0;  // Estável
  }
}

// Função para verificar se o sensor está ativo
bool isSensorActive(uint8_t data){
  return (data == 0x03);
}

// Função para imprimir os dados de leitura
void printReadingData(ReadingData reading) {

  //char json[100];
  //snprintf(json, sizeof(json), "{\"sensor\":\"%s\",\"time\":%s,\"data\":%d}", reading.rawGlucose, reading.rawTimestamp, reading.trend);


  //StaticJsonBuffer<300> JSONBuffer;
  //JsonObject& parsed = JSONBuffer.parseObject(JSONMessage);
  //Ada ada(String(reading.rawGlucose), reading.trend, String(reading.rawTimestamp));
  //Firebase.setString("/glucose/ada/", json);
  //Firebase.setInt("/glucose/ada/trend", reading.trend);
  //Firebase.setString("/glucose/ada/time", String(reading.rawTimestamp));

  
  //Firebase.pushString("/glucose/ada", String(json));




  Serial.println("Dados da Leitura:");
  Serial.print("Timestamp: ");
  Serial.println(reading.rawTimestamp);
  Serial.print("Glicose: ");
  Serial.println(reading.rawGlucose);
  Serial.print("Tempo do Sensor: ");
  Serial.println(reading.sensorTime);
  Serial.print("Tendência: ");
  Serial.println(reading.trend);
  Serial.println();

  setDatabase(reading);
}

void setDatabase(ReadingData reading){
  Firebase.setInt(URL_GURIA + "/dextro", getFilteredDigits(String(reading.rawGlucose)));
  Firebase.setInt(URL_GURIA + "/time", reading.rawTimestamp);
  Firebase.setInt(URL_GURIA + "/trend", reading.trend);

  for(int f=4; f>0; f--){
    String url = String(URL_HISTORY_GURIA) + "/" + f;
    int dextro = Firebase.getInt(url + "/dextro");
    int time = Firebase.getInt(url + "/time");
    int trend = Firebase.getInt(url + "/trend");

    Serial.println(url);

    delay(10);

    String nextUrl = String(URL_HISTORY_GURIA) + "/" + (f + 1);
    Firebase.setInt(nextUrl + "/dextro", dextro);
    Firebase.setInt(nextUrl + "/time", time);
    Firebase.setInt(nextUrl + "/trend", trend);
    Serial.println(nextUrl);
  }

  Firebase.setInt(URL_HISTORY_GURIA + "/1/dextro", getFilteredDigits(String(reading.rawGlucose)));
  Firebase.setInt(URL_HISTORY_GURIA + "/1/time", reading.rawTimestamp);
  Firebase.setInt(URL_HISTORY_GURIA + "/1/trend", reading.trend);
}

int getFilteredDigits(String input) {
    int length = input.length();

    if (length == 4) {
        String substring = input.substring(0, 3);
        return substring.toInt();
    } else if (length == 3) {
        String substring = input.substring(0, 2);
        return substring.toInt();
    } else {
        return 0;
    }
}

void cleanMonitor() {
  for (int i = 0; i < 10; i++) { // Envia 10 linhas em branco para o monitor serial
    Serial.println();
  }
}

void setupWiFi() {
  Serial.print("Conectando-se a " + String(WIFI_SSID));
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("."); delay(300);
  }
  Serial.println("Conectado ao Wi-Fi, IP: " + String(WiFi.localIP()));
}