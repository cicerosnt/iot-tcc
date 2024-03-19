
#include <PN5180.h>
#include <PN5180ISO15693.h>


#define PN5180_NSS  2
#define PN5180_BUSY 5
#define PN5180_RST  4

PN5180ISO15693 nfc(PN5180_NSS, PN5180_BUSY, PN5180_RST);

void setup(void)
{
  Serial.begin(115200);
  Serial.println("*** NFC Reading Example ***");

  bool teste = nfc.begin();

  if (!teste)
  {
    Serial.println("*** PN5180 initialization failed ***");
    while (1);
  }

  Serial.println("*** PN5180 initialized ***");
}

void loop(void)
{
  if (nfc.isCard())
  {
    byte uid[8];
    uint8_t uidLength = nfc.readCardSerial(uid);

    Serial.print("*** NFC tag detected ***");
    Serial.print(" UID: ");

    for (int i = 0; i < uidLength; i++)
    {
      Serial.print(uid[i], HEX);
      Serial.print(" ");
    }

    Serial.println();
    nfc.halt();
  }

  delay(100);
}
