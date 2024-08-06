#include <PN5180.h>
#include <PN5180ISO15693.h>

#define PN5180_NSS  5
#define PN5180_BUSY 16
#define PN5180_RST  17

PN5180ISO15693 nfc(PN5180_NSS, PN5180_BUSY, PN5180_RST);
uint8_t uid[8];
ISO15693ErrorCode rc;

// Constants for FRAM data structure
const int TREND_START = 28;
const int HISTORY_START = 124;
const int FRAM_RECORD_SIZE = 6;

uint8_t sensorData[344];

struct ReadingData {
  unsigned long rawTimestamp;
  int rawGlucose;
  unsigned long sensorTime;
  int trend;
};

void setup() {
  Serial.begin(115200);
  Serial.println(F("ESP32 FreeStyle Libre Reader"));

  Serial.println("Inicializando o leitor...");
  nfc.begin();
  Serial.println("Redefinindo o leitor..");
  nfc.reset();
  Serial.println("Habilitando o campo NFC...");
  nfc.setupRF();

  Serial.println("Pronto para utilização...");

  uint8_t firmwareVersion[2];
  nfc.readEEprom(FIRMWARE_VERSION, firmwareVersion, sizeof(firmwareVersion));
  Serial.print(F("Firmware version: "));
  Serial.print(firmwareVersion[1]);
  Serial.print(".");
  Serial.println(firmwareVersion[0]);

  if (firmwareVersion[0] == 0) {
    Serial.println(F("Falha na inicialização!"));
    while (1) {};
  }
}

void loop() {
    Serial.println("Procurando por sensor FreeStyle Libre...");
    rc = nfc.getInventory(uid);
    if (rc == ISO15693_EC_OK) {
      Serial.println("Sensor encontrado. Lendo dados...");
      if (readSensorData()) {
        ReadingData reading = parseData();
        printReadingData(reading);
      }
    } else {
      Serial.print("Erro na leitura do inventário: ");
      Serial.println(nfc.strerror(rc));
    }
    
  
  delay(1500);  // Wait for 1 second before next attempt
}

bool readSensorData() {
  uint8_t blockBuffer[8];
  for (int i = 0; i < 43; i++) {  // Reading all 43 blocks
    rc = nfc.readSingleBlock(uid, i, blockBuffer, sizeof(blockBuffer)); //0x00 | i
    if (rc != ISO15693_EC_OK) {
      Serial.print("Erro na leitura do bloco ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(nfc.strerror(rc));
      return false;
    }
    memcpy(sensorData + (i * 8), blockBuffer, 8);
  }
  return true;
}

ReadingData parseData() {
  ReadingData reading;

  reading.sensorTime = (unsigned long)sensorData[317] | ((unsigned long)sensorData[318] << 8) | 
                       ((unsigned long)sensorData[319] << 16) | ((unsigned long)sensorData[320] << 24);
  
  int indexTrend = reading.sensorTime % 16;
  int indexHistory = reading.sensorTime / 15;

  reading.rawGlucose = getGlucoseValue(indexTrend);
  reading.rawTimestamp = millis();
  reading.trend = calculateTrend(indexTrend);

  return reading;
}

int getGlucoseValue(int index) {
  int trendIndex = TREND_START + (index * FRAM_RECORD_SIZE);
  return (int)sensorData[trendIndex] | ((int)sensorData[trendIndex + 1] << 8);
}

int calculateTrend(int currentIndex) {
  int recentValues[3];
  for (int i = 0; i < 3; i++) {
    int index = (currentIndex - i + 16) % 16;
    recentValues[i] = getGlucoseValue(index);
  }

  if (recentValues[0] > recentValues[2]) {
    return 1;  // Rising
  } else if (recentValues[0] < recentValues[2]) {
    return -1;  // Falling
  } else {
    return 0;  // Steady
  }
}

void printReadingData(ReadingData reading) {
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
}