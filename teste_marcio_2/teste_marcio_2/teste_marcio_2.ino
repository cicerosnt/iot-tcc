#include <PN5180.h>
#include <SPI.h>

// Definição dos pinos
#define PN5180_NSS  5
#define PN5180_BUSY 16
#define PN5180_RST  17

PN5180 nfc(PN5180_NSS, PN5180_BUSY, PN5180_RST);

void setup() {
  Serial.begin(115200);
  SPI.begin();
  nfc.begin();

  // Inicializa o leitor
  nfc.reset();
  Serial.println("PN5180 inicializado com sucesso.");
  nfc.setupRF(); // Configuração da comunicação RF
}

void loop() {
  // Detecção de tag
  uint8_t uid[10] = {0};
  uint8_t uidLength;

  if (nfc.listenISO15693(uid, &uidLength)) {
    Serial.println("Tag ISO 15693 detectada!");
    
    // Leitura do histórico
    uint8_t block = 0;
    const int numBlocks = 43;  // Número de blocos a serem lidos
    uint8_t data[numBlocks * 4];  // Cada bloco tem 4 bytes
    for (int i = 0; i < numBlocks; i++) {
      if (!nfc.readBlock(block++, data + i * 4)) {
        Serial.println("Falha na leitura de um bloco");
        break;
      }
    }
    if (i == numBlocks) {
      Serial.println("Dados lidos com sucesso:");
      for (int j = 0; j < numBlocks * 4; j++) {
        Serial.print(data[j], HEX);
        Serial.print(" ");
      }
      Serial.println();
      
      // Decodificação dos dados
      decodeData(data, numBlocks * 4);
    }
  } else {
    Serial.println("Nenhuma tag ISO 15693 detectada.");
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
