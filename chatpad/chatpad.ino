static const int SUCCESS = 0;
static const int FAILURE = -1;

int package_handle();
static const int led = 13;

void blink(int delay_ms){
  digitalWrite(led, HIGH);
  delay(delay_ms);
  digitalWrite(led, LOW);
  delay(delay_ms);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(19200);
  Keyboard.begin();
  
  Serial.println("Init");
  Serial1.setTimeout(20);
  pinMode(led, OUTPUT);
  blink(500);
  blink(500);
  delay(5000);
  Serial.println("Start");
}

void loop() {
  // put your main code here, to run repeatedly:
  int result = 0;
  
  result = package_handle();
  if(0 > result){ goto loopEnd; }
  
loopEnd:  
  delay(1);
}
