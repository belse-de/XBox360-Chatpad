int package_check(byte* buffer, int len);
int package_analyse(byte* buffer, int len);

void printHex(byte* buffer, int len);
int status_check(byte* buffer, int len);
int status_decode(byte* buffer, int len);
int key_check(byte* buffer, int len);
int key_decode(byte* buffer, int len);

static const int inBuffer_size = 16;

int package_handle(){
  static byte inBuffer[inBuffer_size];
  static int timeouts = 0;
  int result = 0;
  
  digitalWrite(led, HIGH);
  int len = Serial1.readBytes(inBuffer, inBuffer_size);
  digitalWrite(led, LOW);
   
  // timeout
  if( 0 == len ) {
    // re init chatpad if 1 sec no communication happend; (Serial1.setTimeout(20) * 50) = 1000 ms 
    if(50 < ++timeouts){ package_init(); timeouts = 0; }
    return FAILURE; 
  } else { 
    timeouts = 0;
  }
  
  result = package_check(inBuffer, len);
  if(0 > result){ return FAILURE; }
  // else new package with matching checksum
  
  Serial.print("<<<");
  printHex( inBuffer, len);
  Serial.println("");
  
  result = package_analyse(inBuffer, len);
  if(0 > result){ return FAILURE; } 
  
  return SUCCESS;
}

int package_init(){
  static const char* msg_init = "\x87\x02\x8C\x1F\xCC";
  Serial.println(">>>msg init");
  int len = Serial1.write((uint8_t*)msg_init, 5);
  if(5 != len){return FAILURE;}
  return SUCCESS; 
}

int package_wake(){
  static const char* msg_wake = "\x87\x02\x8C\x1B\xD0";
  Serial.println(">>>msg wake");
  int len = Serial1.write((uint8_t*)msg_wake, 5);
  if(5 != len){return FAILURE;}
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
