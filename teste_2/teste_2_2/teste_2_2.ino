#include <PN5180.h>
#include <PN5180ISO15693.h>
#include <WiFi.h>

#define PN5180_NSS  5
#define PN5180_BUSY 16
#define PN5180_RST  17

PN5180ISO15693 nfc(PN5180_NSS, PN5180_BUSY, PN5180_RST);

const int MAX_NFC_READTRIES = 5;
const int BLOCK_SIZE = 8;
float trend[16];
float lastGlucose = 0;
int noDiffCount = 0;
bool FirstRun = true;
int NFCReady = 0;
uint8_t uid[8];
ISO15693ErrorCode rc;
int sensorMinutesElapse;

float Glucose_Reading(unsigned long value) {
  // Implementar a função de leitura de glicose aqui
  int bitmask = 0x0FFF;
  return ((value & bitmask)/8.5);
}

float Read_Memory() {
  byte oneBlock[BLOCK_SIZE];
  String hexPointer = "";
  String trendValues = "";
  String hexMinutes = "";
  String elapsedMinutes = "";
  float trendOneGlucose;
  float trendTwoGlucose;
  float currentGlucose;
  float shownGlucose;
  float averageGlucose = 0;
  int glucosePointer;
  int validTrendCounter = 0;
  float validTrend[16];
  
  byte readError = 0;
  int readTry;
  
  rc = nfc.getInventory(uid);

  for (int b = 3; b < 16; b++) {
    readTry = 0;
    do {
      readError = 0;
      byte blockBuffer[BLOCK_SIZE];
      rc = nfc.readSingleBlock(uid, b, blockBuffer, sizeof(blockBuffer));
      if (ISO15693_EC_OK == rc) {
        Serial.print("Block ");
        Serial.print(b);
        Serial.print(": ");
        for (int i = 0; i < BLOCK_SIZE; i++) {
          Serial.print(blockBuffer[i], HEX);
        }
        Serial.println();
        
        // Converter buffer para string hexadecimal
        char str[2 * BLOCK_SIZE + 1];
        for (int i = 0; i < BLOCK_SIZE; i++) {
          sprintf(&str[2 * i], "%02X", blockBuffer[i]);
        }
        trendValues += str;
        readError = false;
      } else {
        readError = true;
      }
      readTry++;
    } while (readError && readTry < MAX_NFC_READTRIES);
  }

  if (readError) {
    Serial.println("Read Memory Block Command FAIL");
    NFCReady = 0;
    return 0;
  }

  elapsedMinutes = "";
  readTry = 0;
  readError = true;
  do {
    byte blockBuffer[BLOCK_SIZE];
    rc = nfc.readSingleBlock(uid, 39, blockBuffer, sizeof(blockBuffer));
    if (ISO15693_EC_OK == rc) {
      char str[2 * BLOCK_SIZE + 1];
      for (int i = 0; i < BLOCK_SIZE; i++) {
        sprintf(&str[2 * i], "%02X", blockBuffer[i]);
      }
      elapsedMinutes += str;
      readError = false;
    } else {
      readError = true;
    }
    readTry++;
  } while (readError && readTry < MAX_NFC_READTRIES);

  if (!readError) {
    hexMinutes = elapsedMinutes.substring(10, 12) + elapsedMinutes.substring(8, 10);
    hexPointer = trendValues.substring(4, 6);
    sensorMinutesElapse = strtoul(hexMinutes.c_str(), NULL, 16);
    glucosePointer = strtoul(hexPointer.c_str(), NULL, 16);
    
    Serial.println("");
    Serial.print("Glucose pointer: ");
    Serial.print(glucosePointer);
    Serial.println("");

    int ii = 0;
    for (int i = 8; i <= 200; i += 12) {
      if (glucosePointer == ii) {
        String trendNow = trendValues.substring(i - 10, i - 8) + trendValues.substring(i - 12, i - 10);
        String trendOne = trendValues.substring(190, 192) + trendValues.substring(188, 190);
        String trendTwo = trendValues.substring(178, 180) + trendValues.substring(176, 178);
        currentGlucose = Glucose_Reading(strtoul(trendNow.c_str(), NULL, 16));
        trendOneGlucose = Glucose_Reading(strtoul(trendOne.c_str(), NULL, 16));
        trendTwoGlucose = Glucose_Reading(strtoul(trendTwo.c_str(), NULL, 16));

        if (FirstRun) {
          lastGlucose = currentGlucose;
        }

        if (((lastGlucose - currentGlucose) > 50) || ((currentGlucose - lastGlucose) > 50)) {
          if (((lastGlucose - trendOneGlucose) > 50) || ((trendOneGlucose - lastGlucose) > 50)) {
            currentGlucose = trendTwoGlucose;
          } else {
            currentGlucose = trendOneGlucose;
          }
        }
      }
      ii++;
    }

    for (int i = 8, j = 0; i < 200; i += 12, j++) {
      String t = trendValues.substring(i + 2, i + 4) + trendValues.substring(i, i + 2);
      trend[j] = Glucose_Reading(strtoul(t.c_str(), NULL, 16));
    }

    for (int i = 0; i < 16; i++) {
      if (((lastGlucose - trend[i]) > 50) || ((trend[i] - lastGlucose) > 50)) {
        continue;
      } else {
        validTrend[validTrendCounter] = trend[i];
        validTrendCounter++;
      }
    }

    if (validTrendCounter > 0) {
      for (int i = 0; i < validTrendCounter; i++)
        averageGlucose += validTrend[i];
        
      averageGlucose /= validTrendCounter;
      
      if (((lastGlucose - currentGlucose) > 50) || ((currentGlucose - lastGlucose) > 50))
        shownGlucose = averageGlucose;
      else
        shownGlucose = currentGlucose;
    } else {
      shownGlucose = currentGlucose;
    }

    if ((lastGlucose == currentGlucose) && (sensorMinutesElapse > 21000)) {
      noDiffCount++;
    }

    if (lastGlucose != currentGlucose) {
      noDiffCount = 0;
    }

    if (currentGlucose != 0)
      lastGlucose = currentGlucose;

    NFCReady = 2;
    FirstRun = false;

    if (noDiffCount > 5)
      return 0;
    else  
      return shownGlucose;
  } else {
    Serial.print("Read Memory Block Command FAIL");
    NFCReady = 0;
    return 0;
  }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Iniciando o leitor");
    nfc.begin(); // inicia pn5180
    Serial.println("Redefinindo pn5180");
    nfc.reset();
    Serial.println("Habilitando o campo NFC para leitura");
    nfc.setupRF();
    Serial.println("Configurações ne inicialização finalizada!");
    Serial.println("Trazendo a versão do firmware...");
  
  
    uint8_t fVersion[2];
    nfc.readEEprom(FIRMWARE_VERSION, fVersion, sizeof(fVersion));
    Serial.println("Versão do Firmware: " + String(fVersion[1]) + ". " + String(fVersion[0]));
  
    if(fVersion[0] == 0){
      Serial.println("Não foi possivel coletar dados da versão")    ;
      while(1){};
    }
  
    Serial.println("Validando inicialização do dispositivo.");
    if(0xff == fVersion[1]){
      Serial.println("Opa! Não foi possivel inicializar. \n Pressione reboot no ESP32.");
      Serial.flush();
      exit(-1);
    }
}

void loop() {
  float glucose = Read_Memory();
  Serial.print("Glucose Level: ");
  Serial.println(glucose);

  Build_Packet(glucose);

  Serial.println("+++++++++++++++++++++++++++++++++++++++");

  delay(2000); // Aguarda 5 segundos antes da próxima leitura
}

String Build_Packet(float glucose) {
  
  // Let's build a String which xDrip accepts as a BTWixel packet
  unsigned long raw = glucose*1000; // raw_value
  String packet = "";
  packet = String(raw);
  //packet += ' ';
  //packet += "216";
  //packet += ' ';
  //packet += String(batteryPcnt);
  packet += ' ';
  packet += String(sensorMinutesElapse);
  Serial.println("");
  Serial.print("Glucose level: ");
  Serial.print(glucose);
  Serial.println("");
  Serial.print("15 minutes-trend: ");
  Serial.println("");
  for (int i=0; i<16; i++)
  {
    Serial.print(trend[i]);
    Serial.println("");
  }
  //Serial.print("Battery level: ");
  //Serial.print(batteryPcnt);
  //Serial.print("%");
  //Serial.println("");
  //Serial.print("Battery mVolts: ");
  //Serial.print(batteryMv);
  //Serial.print("mV");
  Serial.println("");
  Serial.print("Sensor lifetime: ");
  Serial.print(sensorMinutesElapse);
  Serial.print(" minutes elapsed");
  Serial.println("");
  return packet;
}
