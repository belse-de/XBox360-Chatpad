int package_check(byte* buffer, int len);
int package_analyse(byte* buffer, int len);

void printHex(byte* buffer, int len);
int status_check(byte* buffer, int len);
int status_decode(byte* buffer, int len);
int key_check(byte* buffer, int len);
int key_decode(byte* buffer, int len);

static const int inBuffer_size = 16;

int package_handle(){
  const static byte package_start_len = 17;
  static byte package_start[package_start_len] = 
      { 0x0F, 0x1E, 0x2D, 0x3C, 
        0x4B, 0x5A, 0x69, 0x78, 
        0x87, 0x96, 0xA5, 0xB4, 
        0xC3, 0xD2, 0xE1, 0xF0, 0x41};
  static byte inBuffer[inBuffer_size];
  byte* inBuffer_pos;
  static int timeouts = 0;
  int result = 0;
  byte sum;
  
  digitalWrite(led, HIGH);
  int len = Serial1.readBytes(inBuffer, inBuffer_size);
  digitalWrite(led, LOW);
   
  // timeout
  if( 0 == len ) {
    // re init chatpad if 1 sec no communication happend; (Serial1.setTimeout(20) * 50) = 1000 ms 
    if((1000/timeout_ms) < ++timeouts){ package_init(); timeouts = 0; }
    return FAILURE; 
  } else { 
    timeouts = 0;
  }
  Serial.print("<<<"); printHex( inBuffer_pos, len); Serial.println();
  inBuffer_pos = inBuffer;

pack_rehandle:
  byte start_pos = len;
  byte start_id = package_start_len;
  bool found_id = false;
  // find start
  for( byte i=0; i<len; ++i){
    for(byte h=0; h<package_start_len; ++h){
      if(inBuffer_pos[i] == package_start[h]){
        start_pos = i; start_id = h;
        i = len; h = package_start_len;
        found_id = true;
      }
    }
  }


  int real_len = len;
  bool found_sum = false;
  if(found_id){
    Serial.print("   ");
    for(int i=0; i<start_pos; ++i) Serial.print("  ");
    Serial.print("^^");

    real_len = len-start_pos;
    if(real_len >= 2){
      int expect_len = inBuffer_pos[start_pos+1] & 0x0F;

      if( real_len >= (expect_len+3) ){
        real_len = (expect_len+3);
        found_sum = true;
      }
    }
  }

  if(found_sum){
    for(int i=1; i<real_len-1; ++i) Serial.print("  ");
    Serial.print("##");

    // calc checksum and safe copy for next compairision
    for( int i = 0; i < real_len - 1; i++){
      sum += inBuffer_pos[start_pos+i];
    }
    sum = (~sum + 1); // = sum * (-1)
  }

   Serial.println();

  if(len > start_pos + real_len) {
    inBuffer_pos = &inBuffer_pos[start_pos + real_len];
    goto pack_rehandle;
  }
  
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

int package_check(byte* buffer, int len){
  static byte oldBuffer[inBuffer_size];
  static int oldLen = 0;
  byte sum = 0;
  boolean buf_equal = true;
  if(oldLen != len){ buf_equal = false; }
  oldLen = len;
  
  // calc checksum and safe copy for next compairision
  for( int i = 0; i < len - 1; i++){
    if(buf_equal && buffer[i] != oldBuffer[i]){ buf_equal = false; }
    oldBuffer[i] = buffer[i];
    sum += buffer[i];
  }
  
  if(buf_equal){ return FAILURE; } // repeated msg
  
  
  sum = (~sum + 1); // = sum * (-1)
  
  // if checksum does not match
  if( sum != buffer[len - 1] ) { Serial.println("checksum does not match");  return FAILURE; } 
  
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
