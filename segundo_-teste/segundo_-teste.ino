#include <PN5180ISO15693.h>

// ESP-32    <--> PN5180 pin mapping:
// 3.3V      <--> 3.3V
// GND       <--> GND
// SCLK, 18   --> SCLK
// MISO, 19  <--  MISO
// MOSI, 23   --> MOSI
// SS, 16     --> NSS (=Not SS -> active LOW)
// BUSY, 5   <--  BUSY
// Reset, 17  --> RST

PN5180ISO15693 nfc(12, 13, 14);

void setup() {
  Serial.begin(115200);
  nfc.begin();
  nfc.reset();
  nfc.setupRF();
}

void loop() {
  uint8_t uid[8];
  ISO15693ErrorCode rc = nfc.getInventory(uid);
  if (ISO15693_EC_OK != rc) {
    Serial.print("Error in getInventory: ");
    Serial.println(nfc.strerror(rc));
    return;
  }
  Serial.print("Inventory successful, UID=");
  for (int i=0; i<8; i++) {
    Serial.print(uid[7-i], HEX); // LSB is first
    if (i < 2) Serial.print(":");
  }
  Serial.println();
  delay(1500);
}
