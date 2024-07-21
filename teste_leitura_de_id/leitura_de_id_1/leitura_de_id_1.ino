#include <PN5180.h>
#include <PN5180ISO15693.h>

// Pinos para o único leitor PN5180
const byte nssPin = 5;
const byte busyPin = 16;
const byte resetPin = 17;

// Criar o objeto para o leitor PN5180
PN5180ISO15693 nfc(nssPin, busyPin, resetPin);

// Array para registrar o valor do último UID lido
uint8_t lastUid[8];

void setup() {
  // Inicializar a conexão serial
  Serial.begin(115200);

  // Inicializar o leitor
  Serial.println(F("Inicializando o leitor..."));
  nfc.begin();
  Serial.println(F("Redefinindo o leitor..."));
  nfc.reset();
  Serial.println(F("Habilitando campo RF..."));
  nfc.setupRF();
  
  Serial.println(F("Configuração Completa"));
}

void loop() {
  // Variável para armazenar o ID de qualquer tag lida
  uint8_t thisUid[8];
  // Tente ler um ID de tag (ou "obter inventário" no linguajar ISO15693)
  ISO15693ErrorCode rc = nfc.getInventory(thisUid);
  // Se o código de retorno foi que um cartão foi lido
  if(rc == ISO15693_EC_OK) {
    // Se este for o mesmo ID que lemos na última verificação
    if(memcmp(thisUid, lastUid, 8) == 0) {
      // Nada a fazer - prossiga
      return;
    }
    // Se for um ID diferente
    else {
      Serial.print(F("Novo Cartão Detectado... "));
      for (int j = 0; j < sizeof(thisUid); j++) {
        Serial.print(thisUid[j], HEX);
        Serial.print(" ");
      }
      Serial.println();
      // Atualize o array que mantém o controle do ID mais recente
      memcpy(lastUid, thisUid, sizeof(lastUid[0]) * 8);

      // Ler os dados da tag
      readTagData(thisUid);
    }
  }
  // Pequeno atraso antes de verificar novamente
  delay(10);
}

void readTagData(uint8_t *uid) {
  // Número de blocos que queremos ler
  const int numBlocks = 4;
  uint8_t blockData[4];

  // Ler cada bloco de dados
  for (uint8_t block = 0; block < numBlocks; block++) {
    // Tentar ler o bloco de dados
    ISO15693ErrorCode rc = nfc.readSingleBlock(uid, block, blockData, numBlocks);
    if (rc == ISO15693_EC_OK) {
      Serial.print(F("Dados do Bloco "));
      Serial.print(block);
      Serial.print(F(": "));
      for (int i = 0; i < sizeof(blockData); i++) {
        Serial.print(blockData[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
    } else {
      Serial.print(F("Erro ao ler o bloco "));
      Serial.println(block);
    }
  }
}
