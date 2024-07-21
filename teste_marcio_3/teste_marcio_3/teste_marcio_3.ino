#include <SPI.h>
#include <PN5180.h>
#include <PN5180ISO15693.h>

#define PN5180_NSS  10
#define PN5180_BUSY 9
#define PN5180_RST  7

PN5180ISO15693 nfc(PN5180_NSS, PN5180_BUSY, PN5180_RST);

void setup() {
  Serial.begin(115200);
  Serial.println("==================================");
  Serial.println("PN5180 ISO15693 Demo Sketch");

  nfc.begin();

  Serial.println("----------------------------------");
  Serial.println("PN5180 Hard-Reset...");
  nfc.reset();

  Serial.println("----------------------------------");
  Serial.println("Reading product version...");
  uint8_t productVersion[2];
  nfc.readEEprom(PRODUCT_VERSION, productVersion, sizeof(productVersion));
  Serial.print("Product version=");
  Serial.print(productVersion[1]);
  Serial.print(".");
  Serial.println(productVersion[0]);

  if (0xff == productVersion[1]) { // if product version 255, the initialization failed
    Serial.println("Initialization failed!?");
    Serial.println("Press reset to restart...");
    Serial.flush();
    while (1);
  }
  
  Serial.println("----------------------------------");
  Serial.println("Reading firmware version...");
  uint8_t firmwareVersion[2];
  nfc.readEEprom(FIRMWARE_VERSION, firmwareVersion, sizeof(firmwareVersion));
  Serial.print("Firmware version=");
  Serial.print(firmwareVersion[1]);
  Serial.print(".");
  Serial.println(firmwareVersion[0]);

  Serial.println("----------------------------------");
  Serial.println("Reading EEPROM version...");
  uint8_t eepromVersion[2];
  nfc.readEEprom(EEPROM_VERSION, eepromVersion, sizeof(eepromVersion));
  Serial.print("EEPROM version=");
  Serial.print(eepromVersion[1]);
  Serial.print(".");
  Serial.println(eepromVersion[0]);

  Serial.println("----------------------------------");
  Serial.println("Enable RF field...");
  nfc.setupRF();
}

void loop() {
  delay(1000);

  Serial.println("----------------------------------");
  uint8_t uid[8];
  ISO15693ErrorCode rc = nfc.getInventory(uid);
  if (ISO15693_EC_OK != rc) {
    Serial.print("Error in getInventory: ");
    Serial.println(nfc.strerror(rc));
    return;
  }
  Serial.print("Inventory successful, UID=");
  for (int i = 0; i < 8; i++) {
    Serial.print(uid[7 - i], HEX); // LSB is first
    if (i < 7) Serial.print(":");
  }
  Serial.println();

  Serial.println("----------------------------------");
  uint8_t blockSize, numBlocks;
  rc = nfc.getSystemInfo(uid, &blockSize, &numBlocks);
  if (ISO15693_EC_OK != rc) {
    Serial.print("Error in getSystemInfo: ");
    Serial.println(nfc.strerror(rc));
    return;
  }
  Serial.print("System Info retrieved: blockSize=");
  Serial.print(blockSize);
  Serial.print(", numBlocks=");
  Serial.println(numBlocks);

  Serial.println("----------------------------------");
  uint8_t readBuffer[blockSize];
  for (int no = 0; no < numBlocks; no++) {
    rc = nfc.readSingleBlock(uid, no, readBuffer, blockSize);
    if (ISO15693_EC_OK != rc) {
      Serial.print("Error in readSingleBlock #");
      Serial.print(no);
      Serial.print(": ");
      Serial.println(nfc.strerror(rc));
      return;
    }
    Serial.print("Read block #");
    Serial.print(no);
    Serial.print(": ");
    for (int i = 0; i < blockSize; i++) {
      if (readBuffer[i] < 16) Serial.print("0");
      Serial.print(readBuffer[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    decodeData(readBuffer, blockSize);
  }
}

void decodeData(uint8_t* rawData, int length) {
  Serial.println("Decodificando os dados...");
  
  String serialNumber = "Número de série: ";
  for (int i = 0; i < 8; i++) {
    if (rawData[i] < 16) serialNumber += "0";
    serialNumber += String(rawData[i], HEX) + " ";
  }
  Serial.println(serialNumber);

  String manufactureDate = "Data de fabricação: ";
  for (int i = 8; i < 12; i++) {
    if (rawData[i] < 16) manufactureDate += "0";
    manufactureDate += String(rawData[i], HEX) + " ";
  }
  Serial.println(manufactureDate);

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
  Serial.print("IRQ-Status 0x");
  Serial.print(irqStatus, HEX);
  Serial.print(": [ ");
  if (irqStatus & (1 << 0)) Serial.print("RQ ");
  if (irqStatus & (1 << 1)) Serial.print("TX ");
  if (irqStatus & (1 << 2)) Serial.print("IDLE ");
  if (irqStatus & (1 << 3)) Serial.print("MODE_DETECTED ");
  if (irqStatus & (1 << 4)) Serial.print("CARD_ACTIVATED ");
  if (irqStatus & (1 << 5)) Serial.print("STATE_CHANGE ");
  if (irqStatus & (1 << 6)) Serial.print("RFOFF_DET ");
  if (irqStatus & (1 << 7)) Serial.print("RFON_DET ");
  if (irqStatus & (1 << 8)) Serial.print("TX_RFOFF ");
  if (irqStatus & (1 << 9)) Serial.print("TX_RFON ");
  if (irqStatus & (1 << 10)) Serial.print("RF_ACTIVE_ERROR ");
  if (irqStatus & (1 << 11)) Serial.print("TIMER0 ");
  if (irqStatus & (1 << 12)) Serial.print("TIMER1 ");
  if (irqStatus & (1 << 13)) Serial.print("TIMER2 ");
  if (irqStatus & (1 << 14)) Serial.print("RX_SOF_DET ");
  if (irqStatus & (1 << 15)) Serial.print("RX_SC_DET ");
  if (irqStatus & (1 << 16)) Serial.print("TEMPSENS_ERROR ");
  if (irqStatus & (1 << 17)) Serial.print("GENERAL_ERROR ");
  if (irqStatus & (1 << 18)) Serial.print("HV_ERROR ");
  if (irqStatus & (1 << 19)) Serial.print("LPCD ");
  Serial.println("]");
}
