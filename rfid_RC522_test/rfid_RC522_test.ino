/*------------------------------------------------------------------------------
  PROJECT:      Module tests
  FILE:         rfid_RC522_test.ino
  AUTHOR:       SSQ66648
  VERSION:      v1.0.1
  DESCRIPTION:  Testing and experimentation of Arduino RFID module: RC522
  ------------------------------------------------------------------------------
  NOTES:
    + library used notes that "Data will be written to the PICC, in sector #1 (blocks #4 to #7)."
    + testing of average read times did not account for errors (card was placed until read completed)
  TESTING RESULTS:
    v1.0:
      Implementation of lib example code for experimentation: editing for use began.
      moved write into own method to allow single line commenting-out.
      detection of card occurrs around 3- 3.5cm.
      distance for reliable reading of card data is < 3cm.
      time required for read has been found to be:
        Average UUID read time: 80
        Average Auth read time: 426       //note: this value does not represent the observed values: to be reviewed for v1.1
        Average Block read time: 440      //note: this value does not represent the observed values: to be reviewed for v1.1
        Average Total read time: 615
      No problem at all if only UUID is required, however keeping chip within 3cm for around half a second may be difficult.
    v1.0.1:
      issues printing the UID byte[10] data: printing contents of [i] from for-loop results in 11(not 10) numbers with unknown relation to the 4-byte UID (8 hex numbers)?
        hex numbers     : 3A 91 FF 3F
        byte[i] numbers : 89213190178
      able to identify card by UID hex numbers: unsure what block data is or why it is required if UID is so much faster?
  TODO:
    + manually check averages data as current version looks to be inaccurate (ie block reading only takes around 100ms, not 400.)
    + fix average calculations completely: values printed for each iteration are not changing
    + tidy average time finding code to own method
    + test demo code (read/write)
    + move variable to member variables (that clear on each use/failed use) -as far too many variables to pass to methods
    + create variable population/erasure methods
  (lesser) TODO:
    + find more information regarding block data and its contents/purpose
  ----------------------------------------------------------------------------*/

//Header files------------------------------------------------------------------
#include <SPI.h>
#include <MFRC522.h>


//Definitions-------------------------------------------------------------------
#define RST_PIN         9           // Configurable
#define SS_PIN          10          // Configurable


//Member Variables--------------------------------------------------------------
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
MFRC522::MIFARE_Key key;

//arrays for testing results
int avRead[10];
int avAuth[10];
int avBlock[10];
int avTot[10];
//counter for testing iterations
int counter = 0;

//Setup-------------------------------------------------------------------------
void setup() {
  //feedback LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(9600);
  //Specify which program is currently running on Arduino
  Serial.println(F("START " __FILE__ " from " __DATE__ "\r\n"));

  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card

  // Prepare the key (used both as key A and as key B)
  // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("Scan a MIFARE Classic PICC to demonstrate read and write."));
  Serial.print(F("Using key (for A and B):"));
  dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);
  Serial.println();

  Serial.println(F("BEWARE: Data will be written to the PICC, in sector #1"));

  Serial.println("Setup complete");
  Serial.println();
}


//Loop--------------------------------------------------------------------------
void loop() {
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent())
    //cannot include output here as is infinite loop
    return;

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
    //cannot include output here as is infinite loop
    return;

  //time reading duration
  unsigned long readTimerStart = millis();

  //clarity print
  Serial.println("****************************************");
  Serial.println("Card found: Read and authenticate...");
  // Show some details of the PICC (that is: the tag/card)
  Serial.print(F("Card UID:"));
  dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);

  //test: find if expected UID:
  Serial.println();
  if (mfrc522.uid.uidByte[0] == 0x59 &&
      mfrc522.uid.uidByte[1] == 0xD5 &&
      mfrc522.uid.uidByte[2] == 0xBE &&
      mfrc522.uid.uidByte[3] == 0xB2) {
    // confirm match:
    Serial.println("UID MATCH");
  } else {
    Serial.println("NOT a match");
  }





  Serial.println();
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  // Check for compatibility
  if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
          &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
          &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("This sample only works with MIFARE Classic cards."));
    return;
  } else {
    Serial.println("card is compatible type");
  }

  unsigned long readTimerDone = millis();

  // In this sample we use the second sector,
  // that is: sector #1, covering block #4 up to and including block #7
  byte sector         = 1;
  byte blockAddr      = 4;
  byte dataBlock[]    = {
    0x01, 0x02, 0x03, 0x04, //  1,  2,   3,  4,
    0x05, 0x06, 0x07, 0x08, //  5,  6,   7,  8,
    0x09, 0x0a, 0xff, 0x0b, //  9, 10, 255, 11,
    0x0c, 0x0d, 0x0e, 0x0f  // 12, 13, 14, 15
  };
  byte trailerBlock   = 7;
  MFRC522::StatusCode status;
  byte buffer[18];
  byte size = sizeof(buffer);

  // Authenticate using key A
  Serial.println(F("Authenticating using key A..."));
  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // Show the whole sector as it currently is
  Serial.println(F("Current data in sector:"));
  mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
  Serial.println();

  unsigned long readTimerAuthenticated = millis();

  // Read data from the block
  Serial.print(F("Reading data from block ")); Serial.print(blockAddr);
  Serial.println(F(" ..."));
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  Serial.print(F("Data in block ")); Serial.print(blockAddr); Serial.println(F(":"));
  dump_byte_array(buffer, 16); Serial.println();
  Serial.println();

  unsigned long readTimerBlock = millis();


  //write data to card
  //  writeCard(status, trailerBlock, blockAddr, dataBlock, buffer, size, sector);



  Serial.println("----------------------------------------");
  Serial.println("Check complete: halt PICC and encryption on PCD...");
  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
  Serial.println("----------------------------------------");
  Serial.print("Milliseconds taken to read UUID: ");
  Serial.println(readTimerDone - readTimerStart);
  Serial.print("Milliseconds taken to authenticate: ");
  Serial.println(readTimerAuthenticated - readTimerDone);
  Serial.print("Milliseconds taken to read block: ");
  Serial.println(readTimerBlock - readTimerAuthenticated);
  Serial.print("Total Milliseconds taken to complete read: ");
  Serial.println(readTimerBlock - readTimerStart);
  Serial.println("----------------------------------------");
  Serial.println();
  Serial.println();



  //add iteration to averages (quick copy implementation: replace with method later)
  avRead[counter] = readTimerDone - readTimerStart;
  avAuth[counter] = readTimerAuthenticated - readTimerDone;
  avBlock[counter] = readTimerBlock - readTimerAuthenticated;
  avTot[counter] = readTimerBlock - readTimerStart;

  //get average values and print to screen
  Serial.print("Counter: ");
  Serial.println(counter);
  counter++;

  if (counter == 10) {
    Serial.println("========================================================================================================");
    Serial.println("testing iterations complete. Results:");
    //  Serial.println(sizeof(avTot)/2);
    int printTotal = 0;
    for (int i = 0; i < (sizeof(avRead) / sizeof(avRead[0])); i++) {
      printTotal += avRead[i];
    }
    Serial.print("Average UUID read time: ");
    Serial.println(printTotal / (sizeof(avTot) / sizeof(avTot[0])));

    printTotal = 0;
    for (int i = 0; i < sizeof(avAuth); i++) {
      printTotal += avAuth[i];
    }
    Serial.print("Average Auth read time: ");
    Serial.println(printTotal / (sizeof(avTot) / 2));

    printTotal = 0;
    for (int i = 0; i < sizeof(avBlock); i++) {
      printTotal += avBlock[i];
    }
    Serial.print("Average Block read time: ");
    Serial.println(printTotal / (sizeof(avTot) / 2));

    printTotal = 0;
    for (int i = 0; i < sizeof(avTot); i++) {
      printTotal += avTot[i];
    }
    Serial.print("Average Total read time: ");
    Serial.println(printTotal / (sizeof(avTot) / 2));

  }

}





//Functions---------------------------------------------------------------------
//TBC: methods to handle creation and destruction of global variables used in reading and/or writing of PICC
//(allows variables to be global and accessed by any method, destruction method on fail/complete ensures no erronious data will be left over from last reading/failure
void createVariables() {

}

void destroyVariables() {

}


//write and validate
void writeCard(MFRC522::StatusCode status, byte trailerBlock, byte blockAddr , byte dataBlock[] , byte buffer[18], byte size, byte sector  ) {
  Serial.println("----------------------------------------");
  Serial.println("Card found: authenticate and write...");

  // Authenticate using key B
  Serial.println(F("Authenticating again using key B..."));
  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // Write data to the block
  Serial.print(F("Writing data into block ")); Serial.print(blockAddr);
  Serial.println(F(" ..."));
  dump_byte_array(dataBlock, 16); Serial.println();
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr, dataBlock, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  Serial.println();

  Serial.println("----------------------------------------");
  Serial.println("Write complete: check accuracy of write...");
  // Read data from the block (again, should now be what we have written)
  Serial.print(F("Reading data from block ")); Serial.print(blockAddr);
  Serial.println(F(" ..."));
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  Serial.print(F("Data in block ")); Serial.print(blockAddr); Serial.println(F(":"));
  dump_byte_array(buffer, 16); Serial.println();

  // Check that data in block is what we have written
  // by counting the number of bytes that are equal
  Serial.println(F("Checking result..."));
  byte count = 0;
  for (byte i = 0; i < 16; i++) {
    // Compare buffer (= what we've read) with dataBlock (= what we've written)
    if (buffer[i] == dataBlock[i])
      count++;
  }
  Serial.print(F("Number of bytes that match = ")); Serial.println(count);
  if (count == 16) {
    Serial.println(F("Success :-)"));
  } else {
    Serial.println(F("Failure, no match :-("));
    Serial.println(F("  perhaps the write didn't work properly..."));
  }
  Serial.println();

  // Dump the sector data
  Serial.println(F("Current data in sector:"));
  mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
  Serial.println();
}



/**
   Helper routine to dump a byte array as hex values to Serial.
*/
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
