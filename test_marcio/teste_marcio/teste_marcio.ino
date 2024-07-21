#include <PN5180.h>
#include <PN5180ISO15693.h>

#if defined(ARDUINO_ARCH_ESP32)

#define PN5180_NSS  5
#define PN5180_BUSY 16
#define PN5180_RST  17

#else
#error Defina seu pinout aqui!
#endif

PN5180ISO15693 nfc(PN5180_NSS, PN5180_BUSY, PN5180_RST);

void setup() {
  Serial.begin(115200);
  Serial.println(F("=================================="));
  Serial.println(F("Enviado: " __DATE__ " " __TIME__));
  Serial.println(F("PN5180 ISO15693 Demo Sketch"));

  nfc.begin();

  Serial.println(F("----------------------------------"));
  Serial.println(F("Reinicialização Hard do PN5180..."));
  nfc.reset();

  Serial.println(F("----------------------------------"));
  Serial.println(F("Lendo versão do produto..."));
  uint8_t productVersion[2];
  nfc.readEEprom(PRODUCT_VERSION, productVersion, sizeof(productVersion));
  Serial.print(F("Versão do produto="));
  Serial.print(productVersion[1]);
  Serial.print(".");
  Serial.println(productVersion[0]);

  if (0xff == productVersion[1]) { // se a versão do produto for 255, a inicialização falhou
    Serial.println(F("Falha na inicialização!?"));
    Serial.println(F("Pressione reset para reiniciar..."));
    Serial.flush();
    exit(-1); // parada
  }
  
  Serial.println(F("----------------------------------"));
  Serial.println(F("Lendo versão do firmware..."));
  uint8_t firmwareVersion[2];
  nfc.readEEprom(FIRMWARE_VERSION, firmwareVersion, sizeof(firmwareVersion));
  Serial.print(F("Versão do firmware="));
  Serial.print(firmwareVersion[1]);
  Serial.print(".");
  Serial.println(firmwareVersion[0]);

  Serial.println(F("----------------------------------"));
  Serial.println(F("Lendo versão da EEPROM..."));
  uint8_t eepromVersion[2];
  nfc.readEEprom(EEPROM_VERSION, eepromVersion, sizeof(eepromVersion));
  Serial.print(F("Versão da EEPROM="));
  Serial.print(eepromVersion[1]);
  Serial.print(".");
  Serial.println(eepromVersion[0]);

  Serial.println(F("----------------------------------"));
  Serial.println(F("Habilitando campo RF..."));
  nfc.setupRF();
}

uint32_t loopCnt = 0;
bool errorFlag = false;

void loop() {
  delay(1000);
  if (errorFlag) {
    uint32_t irqStatus = nfc.getIRQStatus();
    showIRQStatus(irqStatus);

    if (0 == (RX_SOF_DET_IRQ_STAT & irqStatus)) { // nenhum cartão detectado
      Serial.println(F("*** Nenhum cartão detectado!"));
    }

    nfc.reset();
    nfc.setupRF();

    errorFlag = false;
  }

  Serial.println(F("----------------------------------"));
  Serial.print(F("Loop #"));
  Serial.println(loopCnt++);
  
  uint8_t uid[8];
  ISO15693ErrorCode rc = nfc.getInventory(uid);
  if (ISO15693_EC_OK != rc) {
    Serial.print(F("Erro em getInventory: "));
    Serial.println(nfc.strerror(rc));
    errorFlag = true;
    return;
  }
  Serial.print(F("Inventário bem-sucedido, UID="));
  for (int i=0; i<8; i++) {
    Serial.print(uid[7-i], HEX); // LSB é primeiro
    if (i < 2) Serial.print(":");
  }
  Serial.println();

  Serial.println(F("----------------------------------"));
  uint8_t blockSize, numBlocks;
  rc = nfc.getSystemInfo(uid, &blockSize, &numBlocks);
  if (ISO15693_EC_OK != rc) {
    Serial.print(F("Erro em getSystemInfo: "));
    Serial.println(nfc.strerror(rc));
    errorFlag = true;
    return;
  }
  Serial.print(F("Informações do sistema recuperadas: blockSize="));
  Serial.print(blockSize);
  Serial.print(F(", numBlocks="));
  Serial.println(numBlocks);

  Serial.println(F("----------------------------------"));
  uint8_t readBuffer[blockSize];
  for (int no=0; no<numBlocks; no++) {
    rc = nfc.readSingleBlock(uid, no, readBuffer, blockSize);
    if (ISO15693_EC_OK != rc) {
      Serial.print(F("Erro em readSingleBlock #"));
      Serial.print(no);
      Serial.print(": ");
      Serial.println(nfc.strerror(rc));
      errorFlag = true;
      return;
    }
    Serial.print(F("Leu bloco #"));
    Serial.print(no);
    Serial.print(": ");
    for (int i=0; i<blockSize; i++) { // int i=0; i<blockSize; i++
      if (readBuffer[i] < 16) Serial.print("0");
      Serial.print(readBuffer[i], HEX);
      Serial.print(" ");
    }
    Serial.print(" ");
    for (int i=0; i<numBlocks + 3 - 4; i++) { // int i=0; i<blockSize; i++
      if (isprint(readBuffer[i])) {
        Serial.print((char)readBuffer[i]);
      }
      else Serial.print(".");
    }
    Serial.println();
    decodeData(readBuffer, numBlocks * 4);
    delay(1000);
  }

  delay(1000);
}

void decodeData(uint8_t* rawData, int length) {
  Serial.println("Decodificando os dados...");
  
  // Exemplo de decodificação simples
  Serial.print("Número de série: ");
  for (int i = 0; i < 8; i++) {
    Serial.print(rawData[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  
  Serial.print("Data de fabricação: ");
  for (int i = 8; i < 12; i++) {
    Serial.print(rawData[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  
  Serial.println("Histórico de dados de glicose:");
  for (int i = 12; i < length; i += 6) {
    int timestamp = rawData[i] | (rawData[i + 1] << 8);
    int glucose_level = rawData[i + 2] | (rawData[i + 3] << 8);
    Serial.print("Timestamp: ");
    Serial.print(timestamp);
    Serial.print(", Nível de glicose: ");
    Serial.println(glucose_level);
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
