#include <PN5180.h>
#include <PN5180ISO15693.h>
#include <WiFi.h>

// ESP-32 <-> PN5180 pinout mapping
#define PN5180_NSS  12  // GPIO12
#define PN5180_BUSY 13  // GPIO13
#define PN5180_RST  14  // GPIO14

int LED_BUILTIN = 1;

PN5180ISO15693 nfc(PN5180_NSS, PN5180_BUSY, PN5180_RST);
uint8_t lastUid[8];

void setup() {
  Serial.begin(115200);
  nfc.begin();
  nfc.reset();

  Serial.println(F("----------------------------------"));
  Serial.println(F("Reading IRQ pin config..."));
  uint8_t irqConfig;
  nfc.readEEprom(IRQ_PIN_CONFIG, &irqConfig, 1);
  Serial.print(F("IRQ_PIN_CONFIG=0x"));
  Serial.println(irqConfig, HEX);

  Serial.println(F("----------------------------------"));
  Serial.println(F("Reading IRQ_ENABLE register..."));
  uint32_t irqEnable;
  nfc.readRegister(IRQ_ENABLE, &irqEnable);
  Serial.print(F("IRQ_ENABLE=0x"));
  Serial.println(irqConfig, HEX);

  Serial.println(F("----------------------------------"));
  Serial.println(F("Enable RF field..."));
  nfc.setupRF();

}

uint32_t loopCnt = 0;
bool errorFlag = false;

//SLIX2 Passwords, first is manufacture standard
uint8_t standardpassword[] = {0x0F, 0x0F, 0x0F, 0x0F};
//New Password
uint8_t password[] = {0x12, 0x34, 0x56, 0x78};

void loop() {

  if (errorFlag) {
    uint32_t irqStatus = nfc.getIRQStatus();
    showIRQStatus(irqStatus);

    if (0 == (RX_SOF_DET_IRQ_STAT & irqStatus)) { // no card detected
      Serial.println(F("*** No card detected!"));
    }

    nfc.reset();
    nfc.setupRF();

    errorFlag = false;
  }
  
  Serial.println(F("----------------------------------"));
  Serial.print(F("Loop #"));
  Serial.println(loopCnt++);

  delay(1500);
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
