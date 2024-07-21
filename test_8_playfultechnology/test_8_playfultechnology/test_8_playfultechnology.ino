#include <PN5180.h>
#include <PN5180ISO15693.h>

// CONSTANTES
// Quantas máquinas de leitura estão conectadas
const byte numReaders = 4;
// Qual é o "código correto" que cada leitor deve detectar
uint8_t correctUid[][8] = {
  {0xD1,0xD2,0x48,0x2A,0x50,0x1,0x4,0xE0},
  {0xB,0x8A,0xC6,0x6A,0x0,0x1,0x4,0xE0}
};
// Este pino será baixo quando o quebra-cabeças for resolvido
const byte relayPin = A0;

// GLOBAIS
// Cada leitor PN5180 precisa de pinos únicos NSS, BUSY e RESET,
// conforme definido abaixo
PN5180ISO15693 nfc[] = {
  PN5180ISO15693(5,16,17),
  PN5180ISO15693(2,15,0), // funciona
  PN5180ISO15693(16,4,17), // funciona
  PN5180ISO15693(33,25,32), // funciona
  PN5180ISO15693(21,5,22), // funciona
};

// Array para registrar o valor do último UID lido por cada leitor
uint8_t lastUid[numReaders][8];

void setup() {
  // Configurar o pino da relé
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);
  
  // Inicializar conexão serial
  Serial.begin(115200);

  for(int i=0; i<numReaders; i++){
    Serial.print("Leitor #");
    Serial.println(i);
    Serial.println(F("Iniciando..."));
    nfc[i].begin();
    Serial.println(F("Reiniciando..."));
    nfc[i].reset();
    Serial.println(F("Habilitando campo RF..."));
    nfc[i].setupRF();
  }
  Serial.println(F("Configuração Completa"));
}

void loop() {

  for(int i=0; i<numReaders; i++) {
    // Variável para armazenar o ID de qualquer tag lida por este leitor
    uint8_t thisUid[8];
    // Tentar ler um ID de tag ("obter inventário" em linguagem ISO15693)
    ISO15693ErrorCode rc = nfc[i].getInventory(thisUid);
    // Se o código de resultado foi que uma carta tinha sido lida
    if(rc == ISO15693_EC_OK) {
      // Se este é o mesmo ID que lemos na quadra anterior
      if(memcmp(thisUid, lastUid[i], 8) == 0) {
        // Nada a fazer - passar para o próximo leitor
        continue;
      }
      // Se é um ID diferente
      else {
        Serial.print(F("Nova Carta Detectada no Leitor "));
        Serial.print(i);
        Serial.print(F("... "));
        for (int j=0; j<sizeof(thisUid); j++) {
          Serial.print(thisUid[j],HEX);
          Serial.print(" ");
        }
        Serial.println();
        // Atualizar o array que mantém o ID mais recente
        memcpy(lastUid[i], thisUid, sizeof(lastUid[i][0])*8);

        // Conseguimos colocar esta carta? Resolvemos o quebra-cabeças?
        checkIfPuzzleSolved();
      }
    }
    // Se uma carta não pode ser lida
    else {
      // Testar se sabíamos sobre uma carta antes (caso em que ela só foi removida
      // O byte mais significativo (último) de um UID válido sempre deve ser 0xE0. Por exemplo, E007C4A509C247A8
      if(lastUid[i][7] == 0xE0){
        Serial.print("Carta ");
        for (int j=0; j<sizeof(lastUid[i]); j++) {
          Serial.print(lastUid[i][j], HEX);
        }
        Serial.print(" removida do Leitor ");
        Serial.println(i);
        // Atualizar o array que mantém o ID conhecido por último
        memset(lastUid[i], 0, sizeof(lastUid[i][0])*8);
      }
      
      #ifdef DEBUG
        Serial.print(F("Erro em getInventory: "));
        Serial.println(nfc[i].strerror(rc));
      #endif
    }

    // Um pequeno atraso antes de verificar o próximo leitor
    delay(10);
  }
}

// Ação a tomar quando o quebra-cabeças é resolvido
void onPuzzleSolved() {
  // Ativar a relé
  digitalWrite(relayPin, LOW);
  // Ficar preso neste loop infinitamente
  while(true) { delay(1000); }
}

// Verificar se todos os PN5180 detectaram a tag correta
void checkIfPuzzleSolved() {
  // Testar cada leitor por vez
  for(int i=0; i<numReaders; i++){
    // Se este leitor não detectou a tag correta
    if(memcmp(lastUid[i], correctUid[i], 8)!= 0){
      // Saída
      return;
    }
  }
  onPuzzleSolved();
}
