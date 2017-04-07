static const int SUCCESS = 0;
static const int FAILURE = -1;

int package_handle();
int package_init();
int package_wake();

void printHex(byte* buffer, int len);

static const int led = 13;

static const int timeout_ms = 4;

void blink(int delay_ms){
  digitalWrite(led, HIGH);
  delay(delay_ms);
  digitalWrite(led, LOW);
  delay(delay_ms);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  Serial1.begin(19200);
  Keyboard.begin();
  
  Serial.println("Init");
  Serial1.setTimeout(timeout_ms); // 15 to big 10 to smale  13                                                                                                                                    20 -> checksum error 4 fist packages
  pinMode(led, OUTPUT);
  blink(500);
  blink(500);
  delay(5000);
  Serial.println("Start");

  byte inBuffer[32];
  while( Serial1.readBytes(inBuffer, 32) ) delay(1);
  
  package_init();
  delay(1);
  while( package_handle() == SUCCESS ) delay(1);
  
  package_wake();
  delay(1);
  while( package_handle() == SUCCESS ) delay(1);

  
}

void loop() {
  // put your main code here, to run repeatedly:
  static int result = 0;
  static char msg_brut[] = "\x87\x02\x8C\x00\x00";
  static unsigned long last_millis = 0;
  if( (millis() - last_millis) > 1000 ) { package_wake(); last_millis = millis(); delay(10); while( package_handle() == SUCCESS ) delay(1);}


  if( result == 0 ){
    if( msg_brut[3] == 0xFF ) result = 1;

    if( Serial.available() ){
      String line = Serial.readStringUntil('\n'); 
      Serial.print("<<<"); Serial.println(line);
    }
    // calc checksum
    msg_brut[4] = ~(0x15 + msg_brut[3]) +1; // = sum * (-1)
    Serial.print(">>>msg brut "); printHex((uint8_t*)&msg_brut[3], 1); Serial.println("");
    Serial1.write((uint8_t*)msg_brut, 5);
    Serial1.flush();
    
    
    while( not Serial.available() ) delay(1);
    while( package_handle() == SUCCESS ) delay(1);
    
    if(msg_brut[3] <= 254) ++msg_brut[3];
    delay(3);
  }
  
  while( package_handle() == SUCCESS ) delay(1);

  
}
