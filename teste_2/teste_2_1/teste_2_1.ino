#include <PN5180.h>
#include <PN5180ISO15693.h>

#define PN5180_NSS  5
#define PN5180_BUSY 16
#define PN5180_RST  17

#define MAX_NFC_READTRIES 10

PN5180ISO15693 nfc(PN5180_NSS, PN5180_BUSY, PN5180_RST);

uint8_t uid[8];
ISO15693ErrorCode rc;

// Buffer para armazenar dados recebidos via SPI
byte RXBuffer[24];

// variaveis de estado
byte nfcReady = 0;   // será usada para reastrear o estado do NFC
byte firstRun = 0;   // indicativo para primeira execução
int noDiffCount = 0; // contado de diferença do nivel glicose
int sensorMinutesElapse; // tempodecorrido do sensor em minutos
int lastGlucose; // ultimo valor de glicose lido
float trend[16]; // array para guardar tedencia de glico
bool errorFlag = false; // valida inicialização do dispositivo
const int BLOCK_SIZE = 8;

int NFCReady = 0;

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

void isDeviceOn(){
  if(errorFlag){
    uint32_t irqStatus = nfc.getIRQStatus();
    showIRQStatus(irqStatus);

    // valida detecção de dispositivo NFC
    if (0 == (RX_SOF_DET_IRQ_STAT & irqStatus)) {
      Serial.println(F("Campo NFC não detectado..."));
    }

    nfc.reset();
    nfc.setupRF();
    errorFlag = false;
  }
}

void showIRQStatus(uint32_t irqStatus) {
  Serial.print(F("IRQ-Status 0x"));
  Serial.print(irqStatus, HEX);
  Serial.print(": [ ");
  if (irqStatus & (1<< 0)) Serial.print(F("RQ "));
  if (irqStatus & (1<< 1)) Serial.print(F("TX "));
  if (irqStatus & (1<< 2)) Serial.print(F("IDLE "));
  if (irqStatus & (1<< 3)) Serial.print(F("MODE_DETECTED "));
  if (irqStatus & (1<< 4)) Serial.print(F("CARD_ACTIVATED "));
  if (irqStatus & (1<< 5)) Serial.print(F("STATE_CHANGE "));
  if (irqStatus & (1<< 6)) Serial.print(F("RFOFF_DET "));
  if (irqStatus & (1<< 7)) Serial.print(F("RFON_DET "));
  if (irqStatus & (1<< 8)) Serial.print(F("TX_RFOFF "));
  if (irqStatus & (1<< 9)) Serial.print(F("TX_RFON "));
  if (irqStatus & (1<<10)) Serial.print(F("RF_ACTIVE_ERROR "));
  if (irqStatus & (1<<11)) Serial.print(F("TIMER0 "));
  if (irqStatus & (1<<12)) Serial.print(F("TIMER1 "));
  if (irqStatus & (1<<13)) Serial.print(F("TIMER2 "));
  if (irqStatus & (1<<14)) Serial.print(F("RX_SOF_DET "));
  if (irqStatus & (1<<15)) Serial.print(F("RX_SC_DET "));
  if (irqStatus & (1<<16)) Serial.print(F("TEMPSENS_ERROR "));
  if (irqStatus & (1<<17)) Serial.print(F("GENERAL_ERROR "));
  if (irqStatus & (1<<18)) Serial.print(F("HV_ERROR "));
  if (irqStatus & (1<<19)) Serial.print(F("LPCD "));
  Serial.println("]");
}

float glucoseReading(unsigned int val){
  // define uma mascara de bits para extrair os 12 bits menos significativos
  int bitmask = 0x0FFF;
  // aplica a mascara ao valor para obter os 12 bits relevantes e divide o resultado po 8.5
  // o  valor resultado representa a leitura da glicose
  return ((val & bitmask)/8.5);
}

float readMemory() {
  
}


void loop() {
  //Serial.print("[2J");
  isDeviceOn();

  rc = nfc.getInventory(uid);
  if (ISO15693_EC_OK != rc) {
    Serial.print(F("Erro: "));
    Serial.println(nfc.strerror(rc));
    errorFlag = true;
    nfcReady = 2;
    delay(1000);
    return;
  }else{
    Serial.println("lendo dados");
    nfcReady = 1;
    
    byte oneBlock[8];
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

    for (int b = 3; b < 16; b++) {
    int readTry = 0;
    bool readError = true;
    do {
      byte blockBuffer[BLOCK_SIZE];
      rc = nfc.readSingleBlock(uid, b, blockBuffer, sizeof(blockBuffer));
      if (rc == ISO15693_EC_OK) {
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
    Serial.print("Read Memory Block Command FAIL");
    NFCReady = 0;
    return;
  }

  // Leitura do bloco de minutos
  elapsedMinutes = "";
  readTry = 0;
  readError = true;
  do {
    byte blockBuffer[BLOCK_SIZE];
    rc = nfc.readSingleBlock(uid, 39, blockBuffer, sizeof(blockBuffer));
    if (rc == 0) {
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
    unsigned long sensorMinutesElapse = strtoul(hexMinutes.c_str(), NULL, 16);
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
        currentGlucose = glucoseReading(strtoul(trendNow.c_str(), NULL, 16));
        trendOneGlucose = glucoseReading(strtoul(trendOne.c_str(), NULL, 16));
        trendTwoGlucose = glucoseReading(strtoul(trendTwo.c_str(), NULL, 16));

        if (firstRun) {
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
      trend[j] = glucoseReading(strtoul(t.c_str(), NULL, 16));
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

    nfcReady = 2;
    firstRun = false;

    if (noDiffCount > 5)
      return;
    else  
      //shownGlucose()/
      return; //shownGlucose();
  } else {
    Serial.print("Read Memory Block Command FAIL");
    nfcReady = 0;
    return; // 0;
  }
  
  }
  delay(1000);
}
