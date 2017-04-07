void printHex(byte* buffer, int len){
  for(int i = 0; i < len; i++){
    uint8_t nr = buffer[i];
    if(nr < 0x10) { Serial.print("0"); }
    Serial.print(nr, HEX); 
  }
}
