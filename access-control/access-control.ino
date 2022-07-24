// Biblioteca responsável pela comunicação com o display LCD
#include <LiquidCrystal.h>
// Biblioteca responsável pela comunicação com o módulo RFID - RC522
#include <MFRC522.h>
// Biblioteca responsável pela comunicação pela porta SPI
#include <SPI.h>

// Conexão RFID - ESP32 (SS - 05, SCK - 18, MOSI - 23, MISO - 19, RST - 22)

// Defines para utilização do RFID
#define SS_PIN          5
#define RST_RFID        22
#define GREEN_LED       16
#define RED_LED         17
#define BUZZER          13

// Define os pinos para o módulo RC522
MFRC522 rfid(SS_PIN, RST_RFID);

// Cria um objeto do tipo LCD: Sinais: [RS, EN, D4, D5, D6, D7]
LiquidCrystal My_LCD(32, 33, 25, 26, 27, 14);

String UID = "";

void setup() {
  Serial.begin(9600);

  // Inicia o barramento serial 
  SPI.begin();
  
  // Define os pinos como entrada ou saída
  pinMode(BUZZER,    OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED,   OUTPUT);

  // Inicializa o módulo MFRC522 (RFID)
  rfid.PCD_Init();
  
  Serial.println("Aproxime o seu cartão de leitura...");
  Serial.println();
   
  // Inicializa os parametros do LCD, [Colunas, Linhas]
  My_LCD.begin(16, 2);
  // Limpa a tela do LCD
  My_LCD.clear();
  // Escreve a mensagem no display na posição do cursor (0,0)
  My_LCD.print("Aguardando");
  // Coloca o cursor na posição [Col 5, Lin 1]
  My_LCD.setCursor(5,1); 
  // Mostra a segunda mensagem na posição (5, 1)
  My_LCD.print("cartao...");

  // Limpa os dados do cartão RFID
  UID = "";
}

void loop() {
  // Verifica se há algum cartão a ser lido
  if (rfid.PICC_IsNewCardPresent())
  {
    // Verifica se o cartão foi lido
    if (rfid.PICC_ReadCardSerial())
    {
      // Recupera o tipo de cartão RFID
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
      Serial.print("RFID/NFC Tag Type: ");
      Serial.println(rfid.PICC_GetTypeName(piccType));

      // Faz o print do ID do cartão em formato hexadecimal
      Serial.print("UID: ");

      My_LCD.clear();
      My_LCD.setCursor(0, 0);
      My_LCD.print("UID Tag: ");
      My_LCD.setCursor(4, 1);
      
      for(int i = 0; i < rfid.uid.size; i++) 
      {
        Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(rfid.uid.uidByte[i], HEX);
        
        My_LCD.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        My_LCD.print(rfid.uid.uidByte[i], HEX);

        UID.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
        UID.concat(String(rfid.uid.uidByte[i], HEX));
      }
      delay(2000);
      
      My_LCD.clear();
      My_LCD.setCursor(0,0);
      UID.toUpperCase();
      
      if (UID.substring(1) == "41 35 F2 27") 
      {
        My_LCD.print("Autorizado");
        digitalWrite(BUZZER, HIGH);
        digitalWrite(GREEN_LED, HIGH);
        delay(500);
        digitalWrite(BUZZER, LOW);
        delay(500);
        digitalWrite(GREEN_LED, LOW);
      }
      else
      {
        My_LCD.print("Acesso Negado!");
        digitalWrite(BUZZER, HIGH);
        digitalWrite(RED_LED, HIGH);
        delay(250);
        digitalWrite(BUZZER, LOW);
        delay(100);
        digitalWrite(BUZZER, HIGH);
        delay(250);
        digitalWrite(BUZZER, LOW);
        delay(400);
        digitalWrite(RED_LED, LOW);
      }
      
      Serial.println();

      // Instrui o PICC quando no estado ATIVO e seguir para um estado de PARADA
      rfid.PICC_HaltA();
      // Para a encripitação do PCD, deve ser chamado depois da comunicação com autenticação, de forma que novas comunicações possam ser realizadas
      rfid.PCD_StopCrypto1();

      setup();
    }
  }
}
