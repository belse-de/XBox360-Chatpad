int key_decode_mode(byte preMode, byte curMode);
enum Event { 
      EV_NONE=0,
      EV_DOWN,
      EV_CLICK,
      EV_DOUBLE_CLICK, 
      EV_PRESS, 
      EV_LONGPRESS 
};

static const byte MODS = 2;
static const byte ROWS = 7; //four rows
static const byte COLS = 7; //three columns
static byte glob_mode = 0;
/*
    KEY_LEFT_GUI	0x83	131
    KEY_INSERT	        0xD1	209
    KEY_PAGE_UP	        0xD3	211
    KEY_PAGE_DOWN	0xD6	214
    KEY_HOME	        0xD2	210
    KEY_END	        0xD5	213
    KEY_CAPS_LOCK
*/
static int keyMap[MODS][ROWS][COLS] = {
  {
    {'7', 'u', 'j', 'n', KEY_RIGHT_ARROW, ' ', KEY_BACKSPACE},
    {'6', 'z', 'h', 'b', 'm',             ',', 'l'},
    {'5', 't', 'g', 'v', '.',             KEY_RETURN, ' '},
    {'4', 'r', 'f', 'c', ' ',             'p', ' '},
    {'3', 'e', 'd', 'x', KEY_LEFT_ARROW,  '0', 'o'},
    {'2', 'w', 's', 'y', ' ',             '9', 'i'},
    {'1', 'q', 'a', ' ', ' ',             '8', 'k'}
  },
  {
    {'7', 'ü', 'j', 'n', KEY_UP_ARROW,    ' ', KEY_BACKSPACE},
    {'6', '^', '´', '+', 'm',             '#', 'l'},
    {'5', 't', 'g', '-', 'ß',             KEY_RETURN, ' '},
    {'4', 'r', 'f', 'c', ' ',             'p', ' '},
    {'3', 'e', 'd', 'x', KEY_DOWN_ARROW , KEY_DELETE, 'ö'},
    {'2', 'w', 's', '<', ' ',             '9', 'i'},
    {KEY_ESC, KEY_TAB, 'ä', ' ', ' ',             '8', 'k'}
  }
};

int key_check(byte* buffer, int len){
  if(8 != len 
    || 0xC5 != buffer[1] 
    || 0x00 != buffer[2])
  { return FAILURE; }
  return SUCCESS;
}

int key_map(byte key){
  if(0x00 == key){ return '\0'; }
  if(MODS <= glob_mode){ return '\0'; }
  
  byte row = (key & 0x0f)         -1;
  byte col = ((key & 0xf0) >> 4)  -1;  
  if(ROWS <= row){ return '\0'; }
  if(COLS <= col){ return '\0'; }
  
  int     c = keyMap[glob_mode][row][col];
  return c;
}

int key_decode(byte* buffer, int len){
  //Serial.println("<<<key press");
  //k1, k2
  static byte preMod = 0;
  static byte  preK1 = 0;
  static byte  preK2 = 0;
  
  byte mod, k1, k2;
  mod = buffer[0];
  k1  = buffer[1];
  k2  = buffer[2];
  
  boolean changeK1 = preK1 != k1;
  boolean changeK2 = preK2 != k2;
  boolean diffPK1K2 = preK1 != k2;
  boolean diffPK2K1 = preK2 != k1;
  
  
  if( changeK1 || changeK2 ){
    //test routine for press
    if(k1 && changeK1 && diffPK2K1){ 
      Keyboard.press(key_map(k1)); 
    }
    if(k2 && changeK2 && diffPK1K2){ 
      Keyboard.press(key_map(k2)); 
    }
    if(preK1 && changeK1 && diffPK1K2){ 
      Keyboard.release(key_map(preK1)); 
    }
    if(preK2 && changeK2 && diffPK2K1){
      Keyboard.release(key_map(preK2));
    }
  }
  
  if(preMod != mod){ key_decode_mode(preMod, mod); }
 
  preMod = mod;
  preK1 = k1;
  preK2 = k2;
  
  return SUCCESS;
}

int key_decode_mode(byte preMode, byte mode){
  static const byte shift  = 0b0001;
  static const byte square = 0b0010; // strg
  static const byte circle = 0b0100; // alt
  static const byte people = 0b1000; // 
  
  static ButtonHandler handler_shift  = ButtonHandler(0, shift);
  static ButtonHandler handler_square = ButtonHandler(1, square);
  static ButtonHandler handler_circle = ButtonHandler(2, circle);
  static ButtonHandler handler_people = ButtonHandler(3, people);
  
  static ButtonHandler handlers[4] = {handler_shift, handler_square, handler_circle, handler_people};
  //static const byte masks[4] = {shift, square, circle, people}; // not used yet
  static const int keys[4] = {KEY_LEFT_SHIFT, KEY_LEFT_CTRL, KEY_LEFT_ALT, 0};
  static const char* names[4] = {"SHIFT", "CTRL", "ALT", "MODE change"};
  //static byte mode_state[4];
  byte key_mode_state[4];
  unsigned long time = millis();
  
  for(int i = 0; i < 4; i++){
    key_mode_state[i] = handlers[i].handle(time, mode);
    if(key_mode_state[i] == EV_CLICK || key_mode_state[i] == EV_DOWN){
      Serial.print("+++");
      Serial.print("press ");
      Serial.println(names[i]);
      int key = keys[i];
      if(key){ Keyboard.press(key); }
      else { glob_mode = 1; }
    }else if(key_mode_state[i] == EV_DOUBLE_CLICK || key_mode_state[i] == EV_PRESS || key_mode_state[i] == EV_LONGPRESS){
      Serial.print("+++");
      Serial.print("release ");
      Serial.println(names[i]);
      int key = keys[i];
      if(key){ Keyboard.release(key); }
      else { glob_mode = 0;}
    }
  }
  return SUCCESS;
}
