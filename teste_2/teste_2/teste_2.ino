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
    Serial.println("int b = 3; b < 16; b++");
    readTry = 0;
    do {
      readError = 0;
      nfc.readSingleBlock(uid, b, RXBuffer, sizeof(RXBuffer));
      delay(10);

      while (uid[0] != 8) {
        Serial.println("RXBuffer[0] != 8");
        RXBuffer[0] = nfc.readSingleBlock(uid, 0x03, RXBuffer, sizeof(RXBuffer));
        RXBuffer[0] = RXBuffer[0] & 0x08;
      }
      delay(10);

      Serial.println("saindo do while");

      RXBuffer[0] = RXBuffer[0];  // Código de resposta
      RXBuffer[1] = RXBuffer[0];  // Comprimento dos dados
      for (byte i = 0; i < RXBuffer[1]; i++) {
        RXBuffer[i + 2] = RXBuffer[0];  // Dados recebidos
      }

      if (RXBuffer[0] != 128) {
        readError = 1;
      }
      delay(10);

      // Converte os dados recebidos em uma string hexadecimal
      for (int i = 0; i < 8; i++) {
        oneBlock[i] = RXBuffer[i + 3];
      }

      char str[24];
      unsigned char *pin = oneBlock;
      const char *hex = "0123456789ABCDEF";
      char *pout = str;
      for (; pin < oneBlock + 8; pout += 2, pin++) {
        pout[0] = hex[(*pin >> 4) & 0xF];
        pout[1] = hex[*pin & 0xF];
      }
      pout[0] = 0;
      if (!readError) {
        Serial.println(str);
        trendValues += str;
      }
      readTry++;
    } while ((readError) && (readTry < MAX_NFC_READTRIES));
  }

  // Leitura de um bloco de memória específico
  readTry = 0;
  do {
    readError = 0;
    rc = nfc.readSingleBlock(uid, 39, RXBuffer, sizeof(RXBuffer));
    //rc2 = nfc.readSingleBlock(uint8_t *uid, uint8_t blockNo, uint8_t *blockData, uint8_t blockSize)
    delay(10);

    while (RXBuffer[0] != 8) {
      RXBuffer[0] = RXBuffer[0x03];  // Envia 3 até
      RXBuffer[0] = RXBuffer[0] & 0x08;
    }
    delay(10);

    RXBuffer[0] = RXBuffer[0];  // Código de resposta
    RXBuffer[1] = RXBuffer[0];  // Comprimento dos dados
    for (byte i = 0; i < RXBuffer[1]; i++) {
      RXBuffer[i + 2] = RXBuffer[0];
    }
    if (RXBuffer[0] != 128) {
      readError = 1;
    }
    delay(10);

    for (int i = 0; i < 8; i++) {
      oneBlock[i] = RXBuffer[i + 3];
    }

    char str[24];
    unsigned char *pin = oneBlock;
    const char *hex = "012345678ABCDEF";
    char *pout = str;
    for (; pin < oneBlock + 8; pout += 2, pin++) {
      pout[0] = hex[(*pin >> 4) & 0xF];
      pout[1] = hex[*pin & 0xF];
    }
    pout[0] = 0;

    if (!readError) {
      elapsedMinutes += str;
    }
    readTry++;
  } while ((readError) && (readTry < MAX_NFC_READTRIES));

  if (!readError) {
    // Extrai e converte os minutos e o ponteiro de glicose em valores inteiros
    hexMinutes = elapsedMinutes.substring(10, 12) + elapsedMinutes.substring(8, 10);
    hexPointer = trendValues.substring(4, 6);
    sensorMinutesElapse = strtoul(hexMinutes.c_str(), NULL, 16);
    glucosePointer = strtoul(hexPointer.c_str(), NULL, 16);

    Serial.println("");
    Serial.println("Ponteiro de glicose: " + String(glucosePointer));
    Serial.println("");

    int ii = 0;
    // Avalia os blocos de tendência e calcula o valor de glicose atual
    for (int i = 8; i <= 200; i += 12) {
      if (glucosePointer == ii) {
        if (glucosePointer == 0) {
          String trendNow = trendValues.substring(190, 192) + trendValues.substring(188, 190);
          String trendOne = trendValues.substring(178, 180) + trendValues.substring(176, 178);
          String trendTwo = trendValues.substring(166, 168) + trendValues.substring(164, 166);
          currentGlucose = glucoseReading(strtoul(trendNow.c_str(), NULL, 16));
          trendOneGlucose = glucoseReading(strtoul(trendOne.c_str(), NULL, 16));
          trendTwoGlucose = glucoseReading(strtoul(trendTwo.c_str(), NULL, 16));

          if (firstRun == 1) {
            lastGlucose = currentGlucose;
          }
        
          // Verifica a validade do valor de glicose atual
          if (((lastGlucose - currentGlucose) > 50) || ((currentGlucose - lastGlucose) > 50)) {
            if (((lastGlucose - trendOneGlucose) > 50) || ((trendOneGlucose - lastGlucose) > 50)) {
              currentGlucose = trendTwoGlucose;
            } else {
              currentGlucose = trendOneGlucose;
            }
          }
        } else if (glucosePointer == 1) {
          String trendNow = trendValues.substring(i - 10, i - 8) + trendValues.substring(i - 12, i - 10);
          String trendOne = trendValues.substring(190, 192) + trendValues.substring(188, 190);
          String trendTwo = trendValues.substring(178, 180) + trendValues.substring(176, 178);
          currentGlucose = glucoseReading(strtoul(trendNow.c_str(), NULL, 16));
          trendOneGlucose = glucoseReading(strtoul(trendOne.c_str(), NULL, 16));
          trendTwoGlucose = glucoseReading(strtoul(trendTwo.c_str(), NULL, 16));

          if (firstRun == 1) {
            lastGlucose = currentGlucose;
          }
                
          // Verifica a validade do valor de glicose atual
          if (((lastGlucose - currentGlucose) > 50) || ((currentGlucose - lastGlucose) > 50)) {
            if (((lastGlucose - trendOneGlucose) > 50) || ((trendOneGlucose - lastGlucose) > 50)) {
              currentGlucose = trendTwoGlucose;
            } else {
              currentGlucose = trendOneGlucose;
            }
          }
        } else {
          String trendNow = trendValues.substring(i - 10, i - 8) + trendValues.substring(i - 12, i - 10);
          String trendOne = trendValues.substring(i - 22, i - 20) + trendValues.substring(i - 24, i - 22);
          String trendTwo = trendValues.substring(i - 34, i - 32) + trendValues.substring(i - 36, i - 34);
          currentGlucose = glucoseReading(strtoul(trendNow.c_str(), NULL, 16));
          trendOneGlucose = glucoseReading(strtoul(trendOne.c_str(), NULL, 16));
          trendTwoGlucose = glucoseReading(strtoul(trendTwo.c_str(), NULL, 16));
              
          // Verifica a validade do valor de glicose atual
          if (firstRun == 1) {
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
      }  
      ii++;
    }

    // processa os valores de tendência e calcula a média
    for (int i = 8, j = 0; i < 200; i += 12, j++) {
      String t = trendValues.substring(i + 2, i + 4) + trendValues.substring(i, i + 2);
      trend[j] = glucoseReading(strtoul(t.c_str(), NULL, 16));
    }

    // filtra tendências inválidas e calcula a média
    for (int i = 0; i < 16; i++) {
      if (((lastGlucose - trend[i]) > 50) || ((trend[i] - lastGlucose) > 50)) {
        // verifica tendência inválida
        continue;
      } else {
        validTrend[validTrendCounter] = trend[i];
        validTrendCounter++;
      }
    }

    if (validTrendCounter > 0) {
      for (int i = 0; i < validTrendCounter; i++) {
        averageGlucose += validTrend[i];
      }

      averageGlucose = averageGlucose / validTrendCounter;

      // decide o valor a ser mostrado com base na validade da glicose atual
      if (((lastGlucose - currentGlucose) > 50) || ((currentGlucose - lastGlucose) > 50)) {
        shownGlucose = averageGlucose; // Se currentGlucose for inválido, usa o valor médio
      } else {
        shownGlucose = currentGlucose; // Tudo ocorreu bem. Mostra o valor atual
      }
    } else {
      shownGlucose = currentGlucose; // Se tudo estiver errado, mostra o valor atual
    }

    // Verifica se o sensor expirou
    if ((lastGlucose == currentGlucose) && (sensorMinutesElapse > 21000)) {
      noDiffCount++;
    }

    // Reinicia o contador se o valor de glicose mudar
    if (lastGlucose != currentGlucose) {
      noDiffCount = 0;
    }

    if (currentGlucose != 0) {
      lastGlucose = currentGlucose;
    }

    nfcReady = 2;
    firstRun = 0;

    // Retorna 0 se o sensor parece expirado, caso contrário, retorna o valor mostrado
    if (noDiffCount > 5) {
      Serial.println("Sensor expirado...");
      return; // return 0;
    } else {
      Serial.println("Show Glicose: " + String(shownGlucose));
      return; //return shownGlucose;
    }

  } else {
    Serial.print("Falha ao ler bloco de memória");
    nfcReady = 0;
    readError = 0;
  }
    Serial.println("Parou aqui, deveria retornar 0");
    return; // return 0
  }

  delay(1000);
}
