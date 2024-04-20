/*
Bu projede ESP32uC ve PN5180 RF Sensoru ile NFC kart okuma sistemi programliyoruz.
Sadece 1 kart isigi acar, Sadece baska 1 kart isigi kapatir.
Her acilip kapanmada kullaniciya bilgi verilir.

Bu sistem yemekhane kartlari, bir ortama giris cikis kartlari , turnike sistemleri gibi yerlerde kullanilir.
Kullandigim kart Mifare 1K Card. 16 sector ve her sector'de 4 block bulunmakta.
Her sectorun 4 blogunun 3'une veriler yukleyebiliriz (Isim, giris-cikis saati vb.)
Kartın datasheet'i icin -> https://www.nxp.com/docs/en/data-sheet/MF1S50YYX_V1.pdf 

PN5180                 ESP32     baglantilari;
5V        ------       5V
3V        ------       Baglamadim.
RST       ------       14 (Istege bagli, uygun bir GPIO secilebilir)
NSS       ------       12 (Istege bagli, uygun bir GPIO secilebilir)
MOSI      ------       23
MISO      ------       19
SCK       ------       18
BUSY      ------       13 (Istege bagli, uygun bir GPIO secilebilir)
GND       ------       GND 
*/

#include "PN5180.h"
#include <PN5180ISO14443.h>
#define PN5180_NSS  12  
#define PN5180_BUSY 13 
#define PN5180_RST  14
#define LED 2

PN5180ISO14443 nfc(PN5180_NSS, PN5180_BUSY, PN5180_RST);
void showIRQStatus(uint32_t irqStatus);
void setup() {
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);
  Serial.begin(115200);
  Serial.println(F("=================================="));
  Serial.println(F("Uploaded: " __DATE__ " " __TIME__));
  Serial.println(F("PN5180 ISO14443 Demo Sketch"));

  nfc.begin();

  Serial.println(F("----------------------------------"));
  Serial.println(F("PN5180 Hard-Reset..."));
  nfc.reset();

  Serial.println(F("----------------------------------"));
  Serial.println(F("Reading product version..."));
  uint8_t productVersion[2];
  nfc.readEEprom(PRODUCT_VERSION, productVersion, sizeof(productVersion));
  Serial.print(F("Product version="));
  Serial.print(productVersion[1]);
  Serial.print(".");
  Serial.println(productVersion[0]);

  if (0xff == productVersion[1]) { // if product version 255, the initialization failed
    Serial.println(F("Initialization failed!?"));
    Serial.println(F("Press reset to restart..."));
    Serial.flush();
    exit(-1); // halt
  }
  
  Serial.println(F("----------------------------------"));
  Serial.println(F("Reading firmware version..."));
  uint8_t firmwareVersion[2];
  nfc.readEEprom(FIRMWARE_VERSION, firmwareVersion, sizeof(firmwareVersion));
  Serial.print(F("Firmware version="));
  Serial.print(firmwareVersion[1]);
  Serial.print(".");
  Serial.println(firmwareVersion[0]);

  Serial.println(F("----------------------------------"));
  Serial.println(F("Reading EEPROM version..."));
  uint8_t eepromVersion[2];
  nfc.readEEprom(EEPROM_VERSION, eepromVersion, sizeof(eepromVersion));
  Serial.print(F("EEPROM version="));
  Serial.print(eepromVersion[1]);
  Serial.print(".");
  Serial.println(eepromVersion[0]);

  Serial.println(F("----------------------------------"));
  Serial.println(F("RF Field is active!"));
  //nfc.setupRF();
}
bool flag1=true;
bool flag2=true; 

// ISO 14443 loop      
void loop() 
{
  Serial.print(F("\num teste")); 
  uint8_t uid[8];
  
  String data = "";
  if (!nfc.readCardSerial(uid)) 
  {
    
    Serial.print(F("\ncard serial successful, UID="));   
    
    for (int i=0; i<8; i++) {
      Serial.print(uid[i], HEX);      //Kartın UID 'sini okumak-gormek isterseniz yorum satiri kaldirin. 
                                        //Ben ilk UID okuduktan sonra acip kapama esitliginde kullandim.
      if (i < 2) Serial.print(":");
      
      data += String(uid[i],HEX);
    }
    
  }
  if(data.equals("f3d57ec0000"))   //Isik acan kartimizin UID'si.
  {
    if(flag1)
    {
    Serial.print("\nISIK ACILDI");
    Serial.print("\n-----------------------");
    digitalWrite(LED,HIGH);
    flag1=false;
    }
    flag2=true;
    return;
  }
  if(data.equals("b03310000"))    ////Isik kapatan kartimizin UID'si.
  {
    if(flag2)
    {
    Serial.print("\nISIK KAPANDI");
    Serial.print("\n-----------------------");  
    digitalWrite(LED,LOW);
    flag2=false;
    }
    flag1=true;
    return;
  }
  nfc.reset();
  nfc.setupRF();
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
