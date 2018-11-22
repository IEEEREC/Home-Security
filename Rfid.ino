//////////////////////////////////////////////////////////////////////////////////
//                             Setup for MFRC522-RFID                           //
//////////////////////////////////////////////////////////////////////////////////

/*
 * Typical pin layout used:
 * -------------------------------------------------
 *             MFRC522      Arduino       Feather   
 *             Reader/PCD   Uno/101       Huzzah ESP8266      
 * Signal      Pin          Pin           Pin       
 * -------------------------------------------------
 * RST/Reset   RST          9             5         
 * SPI SS      SDA(SS)      10            15        
 * SPI MOSI    MOSI         11 / ICSP-4   13        
 * SPI MISO    MISO         12 / ICSP-1   12        
 * SPI SCK     SCK          13 / ICSP-3   14        
 */

#include <SPI.h>
#include <MFRC522.h>

constexpr uint8_t RST_PIN = 5;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = 15;     // Configurable, see typical pin layout above
 
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key; 

// Init array that will store new NUID 
byte nuidPICC[4];



//////////////////////////////////////////////////////////////////////////////////
//                             Setup for CAYENNE IOT                            //
//////////////////////////////////////////////////////////////////////////////////

//#define CAYENNE_DEBUG
#define CAYENNE_PRINT Serial
#include <CayenneMQTTESP8266.h>

// WiFi network info.
char ssid[] = "ssid";
char wifiPassword[] = "pass";

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "username";
char password[] = "pass";
char clientID[] = "client ID";

unsigned long lastMillis = 0;
static int tagToCompare;




//////////////////////////////////////////////////////////////////////////////////
//                                      Setup                                   //
//////////////////////////////////////////////////////////////////////////////////


void setup() { 
  Serial.begin(115200);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 
  
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  
  Cayenne.begin(username, password, clientID, ssid, wifiPassword);
  pinMode(0,OUTPUT);
}


//////////////////////////////////////////////////////////////////////////////////
//                                      Loop                                    //
//////////////////////////////////////////////////////////////////////////////////
void loop() {
  Cayenne.loop();

  // Look for new cards
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    //Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

   // Store NUID into nuidPICC array
   for (byte i = 0; i < 4; i++) {
     nuidPICC[i] = rfid.uid.uidByte[i];
   }
   
   // Salt and Hash the Unique ID
   tagToCompare = (nuidPICC[0]*11) + (nuidPICC[1]*12) + (nuidPICC[2]*13) + (nuidPICC[3]*14);
   
   
   // If the Unique ID is correct send it as a positive integer, otherwise as a negative integer
   if(tagToCompare == yourTagNumber)
      Cayenne.virtualWrite(7, tagToCompare);
   else
      Cayenne.virtualWrite(7, -tagToCompare);
     

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}
