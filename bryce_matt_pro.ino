


// find library here github.com/adafruit/Adafruit_NFCShield_I2C
// project must be done on arduino app
#include <Wire.h>
#include <Adafruit_NFCShield_I2C.h>
#include <Servo.h>
 
Servo myservo;  // create servo object to control a servo
                // a maximum of eight servo objects can be created
int test = 2; 
int pos = 0;    // variable to store the servo position
uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
#define IRQ   (2)
#define RESET (3)  // Not connected by default on the NFC Shield

Adafruit_NFCShield_I2C nfc(IRQ, RESET);

boolean doorOpen;

void setup(void) {
  pinMode(11, OUTPUT);
  Serial.begin(9600);
  Serial.println("Hello!");
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  myservo.write(180);
  delay(500);
 myservo.detach();
  doorOpen = false;
 
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
 
  // configure board to read RFID tags
  nfc.SAMConfig();
 
  Serial.println("Waiting for an ISO14443A Card ...");
 
}


void loop(void)
{
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
   
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
    Serial.print(".");
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 10);
 
  if (success)
  {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");
   
    if (uidLength == 4)
    {
      // We probably have a Mifare Classic card ...
      Serial.println("Seems to be a Mifare Classic card (4 byte UID)");
    
      // Now we need to try to authenticate it for read/write access
      // Try with the factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
      Serial.println("Trying to authenticate block 4 with default KEYA value");
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    
     // Start with block 4 (the first block of sector 1) since sector 0
     // contains the manufacturer data and it's probably better just
     // to leave it alone unless you know what you're doing
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);
    } 
    //------------------------------------------------------------------------ opens door
    OpenDoor();
  
  }
  
  else if (success == nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya))  {
      Serial.println("No Card Detected"); 
         CloseDoor();    
      
      Serial.println("Yeeteroni");
  }
 
  
}

void OpenDoor()
{
  if (doorOpen)
    return;
  else
  {
    digitalWrite(11,HIGH);
    myservo.attach(9);
    Serial.println("Open Door");

    for(pos = 180; pos>=0; pos-=1)     // goes from 180 degrees to 0 degrees
    {                               
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
                           // waits 15ms for the servo to reach the position
    }
    doorOpen = true;
    
  }
  
}

void CloseDoor()
{
  if (!doorOpen)
    return;
  else
  {
    Serial.println("Close Door");
   
    for(pos = 0; pos < 180; pos += 1)  // goes from 0 degrees to 180 degrees
    {                                  // in steps of 1 degree
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(5);                       // waits 15ms for the servo to reach the position
    }
    digitalWrite(11,LOW);
    doorOpen = false;
    myservo.detach();
  }
  
}
