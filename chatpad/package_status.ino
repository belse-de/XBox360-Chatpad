int package_wake();
// A5 45 F0 04 00 04 04 1A
//          03 - standby
//          04 - active
//             00 - light off
//             80 - light on
//                0F 0F - init count

int status_decode(byte* buffer, int len){

  
  Serial.print("---status"); 
  Serial.print(" - awake ");
  Serial.print( ((buffer[3] & 0x0F) == 0x03) ? "no " : ((buffer[3] & 0x0F) == 0x04) ? "yes" : "???" );
  Serial.print(" - light ");
  Serial.print( ((buffer[4] & 0xF0) == 0x00) ? "off" : ((buffer[4] & 0xF0) == 0x80) ? "on " : "???" );
  Serial.print(" - activations: ");
  Serial.print((int) buffer[5]);
  Serial.println("");

  // assert(buffer[5] != buffer[6])
  
  switch(buffer[3] & 0x0F){
    case 0x03:
      package_wake();
      break;
  }

  return SUCCESS;
} 

