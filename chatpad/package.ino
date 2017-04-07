int package_check(byte* buffer, int len);
int package_analyse(byte* buffer, int len);

void printHex(byte* buffer, int len);
int status_check(byte* buffer, int len);
int status_decode(byte* buffer, int len);
int key_check(byte* buffer, int len);
int key_decode(byte* buffer, int len);



int package_handle(){
  const static byte package_start_len = 16;
  const static byte package_start[package_start_len] = 
      { 0x0F, 0x1E, 0x2D, 0x3C, 
        0x4B, 0x5A, 0x69, 0x78, 
        0x87, 0x96, 0xA5, 0xB4, 
        0xC3, 0xD2, 0xE1, 0xF0};
  const static byte start_version = 0x41;
        
  // throw away bytes until we hit a start byte
  while( Serial1.available() ){
    byte front = Serial1.peek();
    byte masked_pos = ((front >> 4) & 0x0F);
    if( package_start[masked_pos] == front or start_version == front ) break;
    Serial1.read();
  }

  static const byte in_buffer_size = 16;
  static byte in_buffer_len = 0;
  static byte in_buffer[in_buffer_size];
  
  // detect legth and read the rest of the package
  static int timeouts = 0;
  byte read_bytes = 0;
  byte sum;
  digitalWrite(led, HIGH);
  if( Serial1.available() ){
    in_buffer[0] = Serial1.peek();
  
    if( in_buffer[0] == start_version){
        in_buffer_len = Serial1.readBytes(in_buffer, 12);
        Serial.print("<<<"); printHex( in_buffer, in_buffer_len); Serial.println();
    } else {
      byte masked_pos = ((in_buffer[0] >> 4) & 0x0F);
      if( package_start[masked_pos] == in_buffer[0] ){
        in_buffer[0] = Serial1.read();
        in_buffer[1] = Serial1.read();
        byte package_len = in_buffer[1] & 0x0F;
        if(package_len > 0){
          read_bytes = Serial1.readBytes(&in_buffer[2], package_len+1);
          
          // timeout
          if( read_bytes == 0 ) {
            // re init chatpad if 1 sec no communication happend; (Serial1.setTimeout(20) * 50) = 1000 ms 
            if((1000/timeout_ms) <= ++timeouts){ package_init(); timeouts = 0; }
            return FAILURE; 
          } else { 
            timeouts = 0;
            in_buffer_len = read_bytes + 2;
          }

          // calc checksum
          for( int i = 0; i < package_len; i++){
            sum += in_buffer[2+i];
          }
          sum = (~sum + 1); // = sum * (-1)
          if( sum == in_buffer[in_buffer_len-1]) return FAILURE;
          Serial.print("<<<"); printHex( in_buffer, in_buffer_len); Serial.println();
        }
      } else {
        in_buffer_len = Serial1.readBytes(in_buffer, in_buffer_size);
        Serial.print("<<<"); printHex( in_buffer, in_buffer_len); Serial.println();
      }
    }
  }
  digitalWrite(led, LOW);

  return SUCCESS;
}

int package_init(){
  static const char* msg_init = "\x87\x02\x8C\x1F\xCC";
  Serial.println(">>>msg init");
  int len = Serial1.write((uint8_t*)msg_init, 5);
  if(5 != len){return FAILURE;}
  Serial1.flush();
  return SUCCESS; 
}

int package_wake(){
  static const char* msg_wake = "\x87\x02\x8C\x1B\xD0";
  Serial.println(">>>msg wake");
  int len = Serial1.write((uint8_t*)msg_wake, 5);
  if(5 != len){return FAILURE;}
  Serial1.flush();
  return SUCCESS;
}

int package_analyse(byte* buffer, int len){
  switch(buffer[0]){
    case 0xA5: // status
      if(0 > status_check(buffer, len)){ 
        Serial.println("check failed - status");
        goto error; 
      }
      if(0 > status_decode(&buffer[3], len - 4)){ goto error; }
      break;
    case 0xB4: // key press
      if(0 > key_check(buffer, len)){ 
        Serial.println("check failed - key"); 
        goto error;
      }
      if(0 > key_decode(&buffer[3], len - 4)){ goto error; }
      break;
    default:  // unknown
      Serial.println("unknown package type");
      goto error;
      break;
  }
  
  return SUCCESS;
error:  
  return FAILURE;
}
