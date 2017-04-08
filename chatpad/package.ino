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
  
  static const byte in_buffer_size = 32;
  byte in_buffer_len = 0;
  static byte in_buffer[in_buffer_size];

  static int timeouts = 0;
  byte read_bytes = 0;
  byte sum;

  // throw away bytes until we hit a start byte
  while( Serial1.available() ){
    byte front = Serial1.peek();
    byte masked_pos = ((front >> 4) & 0x0F);
    if( package_start[masked_pos] == front or start_version == front ) break;
    int read_front = Serial1.read();
    if(read_front == -1) return FAILURE;
  }
  
  // detect legth and read the rest of the package
  digitalWrite(led, HIGH);
  if( Serial1.available() ){
    in_buffer[0] = Serial1.peek();
    
  
    if( in_buffer[0] == start_version){
        in_buffer_len = Serial1.readBytes(in_buffer, 12);
    } else {
      byte masked_pos = ((in_buffer[0] >> 4) & 0x0F);
      if( package_start[masked_pos] == in_buffer[0] ){
        in_buffer_len = Serial1.readBytes(in_buffer, 2);
        // timeout
        if( in_buffer_len == 0 ) {
          // re init chatpad if 1 sec no communication happend; (Serial1.setTimeout(20) * 50) = 1000 ms 
          if((1000/timeout_ms) <= ++timeouts){ package_init(); timeouts = 0; }
          return FAILURE; 
        } else { 
          timeouts = 0;
        }
          
        byte package_len = in_buffer[1] & 0x0F;
        if(package_len > 0){
          in_buffer_len += Serial1.readBytes(&in_buffer[2], package_len+1);

          // calc checksum
          for( int i = 0; i < package_len; i++){
            sum += in_buffer[2+i];
          }
          sum = (~sum + 1); // = sum * (-1)
          if( sum == in_buffer[in_buffer_len-1]) return FAILURE;
        }

      } else {
        in_buffer_len = Serial1.readBytes(in_buffer, in_buffer_size);
      }
    }
  }
  digitalWrite(led, LOW);
  if( in_buffer_len != 0 ) {
    Serial.print("<<<"); printHex( in_buffer, in_buffer_len); Serial.println();
    package_analyse(in_buffer, in_buffer_len);
    return SUCCESS;
  }

  return FAILURE;
}

int package_init(){
  static const char* msg_init = "\x87\x02\x8C\x1F\xCC";
  int len = Serial1.write((uint8_t*)msg_init, 5);
  if(5 != len){return FAILURE;}
  Serial1.flush();
  Serial.print(">>>"); printHex( (uint8_t*)msg_init, 5); Serial.println();
  Serial.println("---msg init");
  return SUCCESS; 
}

int package_wake(){
  static const char* msg_wake = "\x87\x02\x8C\x1B\xD0";
  int len = Serial1.write((uint8_t*)msg_wake, 5);
  if(5 != len){return FAILURE;}
  Serial1.flush();
  Serial.print(">>>"); printHex( (uint8_t*)msg_wake, 5); Serial.println();
  Serial.println("---msg wake");
  return SUCCESS;
}

int package_analyse(byte* buffer, int len){
  switch(buffer[0]){
    case 0xA5: // status
    Serial.println("---status");
      status_decode(buffer, len);
      break;
    case 0xB4: // key press
      Serial.println("---key press");
      break;
    default:  // unknown
      Serial.println("---unknown package type");
      break;
  }
  
  return SUCCESS;
}
