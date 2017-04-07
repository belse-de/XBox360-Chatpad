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



