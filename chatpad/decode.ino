void decodeKeypress(byte mode, byte k1, byte k2){
  static byte lastMode = 0;
  static byte lastK1 = 0;
  static byte lastK2 = 0;
  
  if(0 == mode && 0 == k1 && 0 == k2){
    Keyboard.releaseAll();
    lastMode = lastK1 = lastK2 = 0;
  } else {
    if(lastMode != mode){
      decodeMode(lastMode, mode);
      lastMode = mode;
    }
    if(lastK1 != k1 || lastK2 != k2){
      decodeKeys(lastK1, lastK2, k1, k2);
      lastK1 = k1;
      lastK2 = k2;
    }
  }
}

void decodeMode(byte lastMode, byte mode){
  static const byte shift  = 0b0001;
  static const byte square = 0b0010; // strg
  static const byte circle = 0b0100; // altGr
  static const byte people = 0b1000; // alt
  
  if( (lastMode & shift) != (mode & shift) ){
    if(mode & shift){
      Keyboard.press(KEY_LEFT_SHIFT);
    } else {
      Keyboard.release(KEY_LEFT_SHIFT);
    }
  }
  
  if( (lastMode & square) != (mode & square) ){
    if(mode & square){
      Keyboard.press(KEY_LEFT_CTRL);
    } else {
      Keyboard.release(KEY_LEFT_CTRL);
    }
  }
  
  if( (lastMode & circle) != (mode & circle) ){
    if(mode & circle){
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_LEFT_ALT);
    } else {
      Keyboard.release(KEY_LEFT_CTRL);
      Keyboard.release(KEY_LEFT_ALT);
    }
  }
  
  if( (lastMode & people) != (mode & people) ){
    if(mode & people){
      Keyboard.press(KEY_LEFT_ALT);
    } else {
      Keyboard.release(KEY_LEFT_ALT);
    }
  }
}

void decodeKeys(byte lastK1, byte lastK2, byte k1, byte k2){
  decodeKey(0, lastK1, byte k1);
  decodeKey(0, lastK2, byte k2);
}

void decodeKey(byte mode, byte lastK, byte k){
  if(lastK != k){
    uint8_t row = 0;
    uint8_t col = 0;
    if(k1){
      row =  k & 0x0f;
      col = (k & 0xf0) >> 4;
    } else {
      row =  lastK & 0x0f;
      col = (lastK & 0xf0) >> 4;
    }
    
    row -= 1;
    col -= 1;
    char c = keyMap[mode][row][col];
    
    if(k1){
      Keyboard.press(c);
    }else{
      Keyboard.release(c);
    }
  }
}
