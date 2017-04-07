void printHex(uint8_t* buffer, int len){
  for(int i = 0; i < len; i++){
    uint8_t nr = buffer[i];
    if(nr < 0x10) { Serial.print("0"); }
    Serial.print(nr, HEX); 
  }
}

void initWait(){
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  delay(500);
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  delay(5500);
  digitalWrite(13, HIGH);
}
