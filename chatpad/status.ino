int package_wake();
// A5 45 F0 04 00 04 04 1A
//          03 - standby
//          04 - active
//             00 - light off
//             80 - light on
//                0F 0F - init count

int status_check(byte* buffer, int len){
  if(8 != len 
    || 0x45 != buffer[1] 
    || 0xF0 != buffer[2])
  { return -1; }
  return 0;
}

int status_decode(byte* buffer, int len){
  //Serial.print("<<<status"); 
  int result = 0;
  
  switch(buffer[0]){
    case 0x03:
      //Serial.print(" - standby");
      result += package_wake();
      break;
    case 0x04:
      //Serial.print(" - awake  ");
      break;
    default:
      //Serial.print(" - ???  ");
      //printHex( &buffer[0], 1);
      break;
  }
  
  switch(buffer[1]){
    case 0x00:
      //Serial.print(" - light off");
      break;
    case 0x80:
      //Serial.print(" - light on ");
      break;
    default:
      //Serial.print(" - ???    ");
      //printHex( &buffer[0], 1);
      break;
  }
  
  //Serial.print(" - activations: ");
  //Serial.print((int) buffer[2]);
  if(buffer[2] != buffer[3]){ result--; }
  
  //Serial.println("");
  return result;
} 

