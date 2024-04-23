#include <PN5180.h>
#include <PN5180ISO15693.h>


// ESP-32 <-> PN5180 pinout mapping
#define PN5180_NSS  16  // GPIO16
#define PN5180_BUSY 5  // GPIO5
#define PN5180_RST  17  // GPIO17


PN5180ISO15693 nfc(PN5180_NSS, PN5180_BUSY, PN5180_RST);


void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println(F("Uploaded: " __DATE__ " " __TIME__));

  Serial.println(F("PN5180 inicializando..."));
  nfc.begin();

  Serial.println(F("PN5180 limpa memoria/resquicio..."));
  nfc.reset();

  uint8_t productVersion[2];
  nfc.readEEprom(PRODUCT_VERSION, productVersion, sizeof(productVersion));
  if (0xff == productVersion[1]) { // if product version 255, the initialization failed
    Serial.println(F("Falha na inicialização NFC."));
    Serial.println(F("Pressione restart para recomeçar..."));
    Serial.flush();
    exit(-1); // halt
  }else{
    Serial.println(F("Campo NFC habilitado!"));
    nfc.setupRF();
  }
}

bool errorFlag = false;
uint8_t standardpassword[] = {0x0F, 0x0F, 0x0F, 0x0F};
uint8_t password[] = {0x12, 0x34, 0x56, 0x78};

void loop() {
  delay(1500);
  if (errorFlag){
    uint32_t irqStatus = nfc.getIRQStatus();
    showIRQStatus(irqStatus);

    if (0 == (RX_SOF_DET_IRQ_STAT & irqStatus)) { // no card detected
      Serial.println(F("*** Campo NFC não detectado!"));
    }
    nfc.reset();
    nfc.setupRF();

    errorFlag = false;
  }



  // code for unlocking an ICODE SLIX2 protected tag   
  ISO15693ErrorCode myrc = nfc.unlockICODESLIX2(password);
  if (ISO15693_EC_OK == myrc) {
    Serial.println("unlockICODESLIX2 successful");
  }

  uint8_t uid[8];
  // code for set a new SLIX2 privacy password
  nfc.getInventory(uid);
  Serial.println("set new password"); 
  
  ISO15693ErrorCode myrc2 = nfc.newpasswordICODESLIX2(password, standardpassword, uid);
  if (ISO15693_EC_OK == myrc2) { 
   Serial.println("sucess! new password set");    
  }else{
   Serial.println("fail! no new password set: "); 
   Serial.println(nfc.strerror(myrc2));  
   Serial.println(" "); 
  } 
 



  

  
  ISO15693ErrorCode rc = nfc.getInventory(uid);
  if (ISO15693_EC_OK != rc) {
    Serial.println(F("-----------------------" + ISO15693_EC_OK));
    Serial.println(F("Error in getInventory: "));
    Serial.println(nfc.strerror(rc));
    errorFlag = true;
    return;
  }

  Serial.print(F("Leitura do campo NFC bem sucedida, UID="));
  for (int i=0; i<8; i++) {
    Serial.print(uid[7-i], HEX); // LSB is first
    if (i < 2) Serial.print(":");
  }
  Serial.println();

  

  
  Serial.println();
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
