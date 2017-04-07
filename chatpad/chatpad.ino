static const byte MODS = 3;
static const byte ROWS = 7; //four rows
static const byte COLS = 7; //three columns
static char keyMap[MODS][ROWS][COLS] = {
  {
    {'7', 'u', 'j', 'n', '*', '*', '*'},
    {'6', 'z', 'h', 'b', 'm', ',', 'l'},
    {'5', 't', 'g', 'v', '.', '*', '*'},
    {'4', 'r', 'f', 'c', ' ', 'p', '*'},
    {'3', 'e', 'd', 'x', '*', '0', 'o'},
    {'2', 'w', 's', 'y', '*', '9', 'i'},
    {'1', 'q', 'a', '*', '*', '8', 'k'}
  }
};

void printHex(uint8_t* buffer, int len);
void initWait();
void decodeKeypress(byte mode, byte k1, byte k2);
void decodeMode(byte lastMode, byte mode);
void decodeKeys(byte lastK1, byte lastK2, byte k1, byte k2);
void decodeKey(byte lastK, byte k);

static const int inBuffer_size = 16;
static byte inBuffer[16];

static const char* msg_init = "\x87\x02\x8C\x1F\xCC";
static const char* msg_wake = "\x87\x02\x8C\x1B\xD0";

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600);
  initWait();  
  Serial.println("Init");
  Serial1.begin(19200);
  Serial1.setTimeout(20); // signal with 10Hz -> 0.1 secs
  Keyboard.begin();        // initialize control over the keyboard:
  Serial.println(">>>msg init");
  Serial1.write((uint8_t*)msg_init, 5);
  Serial.println("Start");
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(13, LOW);
  int readBytes = Serial1.readBytes(inBuffer, inBuffer_size);
  digitalWrite(13, HIGH);
  
  byte mode = inBuffer[3];
  byte key1 = inBuffer[4];
  byte key2 = inBuffer[5];
  byte sum = 0;
  static int timeout = 0;
  
  if( 0 == readBytes ) {
    timeout++;
    if(timeout > 100){
      timeout = 0;
      Serial.println(">>>msg init");
      Serial1.write((uint8_t*)msg_init, 5);
    }
    goto loopEnd; 
  } // timeout
  Serial.print("<");
  Serial.print(readBytes, DEC);
  Serial.print(":");
  printHex(inBuffer, readBytes);
  Serial.println("");
  
  // calc checksum
  for( int i = 0; i < readBytes - 1; i++){ sum += inBuffer[i]; }
  sum = (~sum + 1) % 256;
  if( sum != inBuffer[readBytes - 1] ) { goto loopEnd; } // checksum does not match
  
  switch(inBuffer[0]){
    case 0xA5: // status msg;
      if( 8 != readBytes ) { goto loopEnd; } // unknown package format
      if(0x45 != inBuffer[1]){ goto loopEnd; }
      if(0xF0 != inBuffer[2]){ goto loopEnd; }
      Serial.println("<<<status"); 
      if(0x03 == inBuffer[3]){
        // A5 45 F0 04 00 04 04 1A
        //          03 - standby
        //          04 - active
        //             00 - light off
        //             80 - light on
        //                0F 0F - init count
        Serial.println(">>>msg wake");
        Serial1.write((uint8_t*)msg_wake, 5);
      } // standy msg
      break;
      
    case 0xB4:
      if( 8 != readBytes ) { goto loopEnd; } // unknown package format
      if(0xC5 != inBuffer[1]){ goto loopEnd; }
      if(0x00 != inBuffer[2]){ goto loopEnd; } 
      Serial.println("<<<key press");
      decodeKeypress(mode, key1, key2);
      break;
      
    default:
      Serial.println("unknown msg");
      break;
  }
 
loopEnd:
  delay(1);
}
