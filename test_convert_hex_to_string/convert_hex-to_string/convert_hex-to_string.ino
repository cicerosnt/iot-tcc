#include <PN5180.h>
#include <PN5180ISO15693.h>

//#define NFCMEMSIZE = 320;  // 40 blocos de 8 bytes

// config pinagem esp <-> pn5180
const byte nssPin = 5;
const byte busyPin = 16;
const byte resetPin = 17;
//byte BFCMem[NFCMEMSIZE]

// objeto para o PN5180
PN5180ISO15693 nfc(nssPin, busyPin, resetPin);

// arra para guardar o ultimo leitor lido
uint8_t lastId[8];

void setup() {
  Serial.begin(115200);

  Serial.println("Inicializando o leitor...");
  nfc.begin();
  Serial.println("Redefinindo o leitor..");
  nfc.reset();
  Serial.println("Habilitando o campo NFC...");
  nfc.setupRF();

  Serial.println("Pronto para utilização...");
}

void loop() {
  uint8_t thisUid[8]; // guardao id de qq tag lida

  ISO15693ErrorCode rc = nfc.getInventory(thisUid); // tenta ler a tag

  if(rc == ISO15693_EC_OK){ //se o retorno for positivo
    Serial.print(F("Novo campo NFC detectado"));
      for(int j = 0; j < sizeof(thisUid); j++){
        Serial.print(thisUid[j], HEX);
        Serial.print(" ");
      }
    
      Serial.println();
      memcpy(lastId, thisUid, sizeof(lastId[0]) * 8); // atualiza o array da memoria para o ultimo lido

      readNfcData(thisUid);
    delay(100);
  }
}

  void readNfcData(uint8_t *uid) {
  // Número de blocos que queremos ler
  const int startBlock = 0;  // Bloco inicial
  const int numBlocks = 10;   // Número de blocos para ler (32 a 58)
  uint8_t blockData[8];
  uint8_t blockSize = sizeof(blockData); // Tamanho do bloco

  // Ler cada bloco de dados
  for (uint8_t block = startBlock; block < startBlock + numBlocks; block++) {
    // Tentar ler o bloco de dados
    ISO15693ErrorCode rc = nfc.readSingleBlock(uid, block, blockData, blockSize);
    if (rc == ISO15693_EC_OK) {
      Serial.print(F("Dados do Bloco #"));
      Serial.print(block);
      Serial.print(F(": "));
      for (int i = 0; i < blockSize; i++) {
        Serial.print(blockData[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
      // Interpretar os dados do bloco (se necessário)
      interpretData(blockData, blockSize);
      byteArrayToHex(blockData, blockSize);
      //char str[24];
      //convertToHexString(blockData, str, blockSize);
      //Serial.println("Dados do Bloco #"+String(block) + ": " + String(str));

      // podepa(blockData);
    } else {
      Serial.print(F("Erro ao ler o bloco "));
      Serial.println(block);
    }
    delay(1000);
  }
}

// Função de teste de leitura e interpretação de dados
void podepa(uint8_t *data){
  
  byte oneBlock[8];
  for(int i=0; i<8; i++){
    oneBlock[i] = data[i]+3;

    char str[24];
    unsigned char * pin = oneBlock;
    const char * hex = "0123456789ABCDEF";
    char * pout = str;
    for(; pin<oneBlock; pout+=2, pin++){
      pout[0] = hex[(*pin>>4) & 0xF];
      pout[1] = hex[*pin      & 0xF];
    }
    pout[0] = 0;
    Serial.println("Inventory : " + String(str));
  }
}

// Função de exemplo para interpretação de dados
void interpretData(uint8_t *data, int length) {
  // Exemplo de interpretação simples
  // Isso depende do formato específico dos dados do Freestyle Libre
  for (int i = 0; i < length; i += 2) {
    uint16_t value = (data[i] << 8) | data[i + 1];
    Serial.print(F("Valor interpretado: "));
    Serial.println(value);
  }

  //byteArrayToHex(data, length);
}

String byteArrayToHex(byte* arr, size_t len) {
  String hexStr;
  for (size_t i = 0; i < len; i++) {
    if (arr[i] < 0x10) {
      hexStr += "0";
    }
    hexStr += String(arr[i], HEX);
  }
  return hexStr;
}

void convertToHexString(byte* data, char* str, int length) {
  const char *hex = "0123456789ABCDEF";
  char *pout = str;

  for (int i = 0; i < length; i++, pout+=2) {
    pout[0] = hex[(data[i] >> 4) & 0xF];
    pout[1] = hex[data[i] & 0xF];
  }
  pout[0] = 0; // Termina a string com o caractere nulo
}