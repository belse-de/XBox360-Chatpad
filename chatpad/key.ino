int key_decode_mode(byte preMode, byte curMode);

static const byte MODS = 3;
static const byte ROWS = 7; //four rows
static const byte COLS = 7; //three columns

static byte key_mode = 0;
static char keyMap[MODS][ROWS][COLS] = {
  {
    {'7', 'u', 'j', 'n', '*', '*', '*'},
    {'6', 'z', 'h', 'b', 'm', ',', 'l'},
    {'5', 't', 'g', 'v', '.', '*', '*'},
    {'4', 'r', 'f', 'c', ' ', 'p', '*'},
    {'3', 'e', 'd', 'x', '*', '0', 'o'},
    {'2', 'w', 's', 'y', '*', '9', 'i'},
    {'1', 'q', 'a', '*', '*', '8', 'k'}
  }
};

int key_check(byte* buffer, int len){
  if(8 != len 
    || 0xC5 != buffer[1] 
    || 0x00 != buffer[2])
  { return FAILURE; }
  return SUCCESS;
}

char key_map(byte key){
  if(0x00 == key){ return '\0'; }
  
  byte row =  key & 0x0f;
  byte col = (key & 0xf0) >> 4;
  byte mode = 0;
  
  row--;
  col--;
  
  if(ROWS <= row){ return '\0'; }
  if(COLS <= col){ return '\0'; }
  
  char c = keyMap[mode][row][col];
  return c;
}

int key_decode(byte* buffer, int len){
  //Serial.println("<<<key press");
  //k1, k2
  //0x51 right
  //0x55 left
  //0x63 enter
  //0x71 backspace
  
  static byte preMod = 0;
  static byte  preK1 = 0;
  static byte  preK2 = 0;
  
  byte mod, k1, k2;
  mod = buffer[0];
  k1  = buffer[1];
  k2  = buffer[2];
  
  if(preK1 != k1 || preK2 != k2){
    //test routine for press
    if(k1){ Keyboard.press('a'); }
    else  { Keyboard.release('a'); }
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
  static const byte circle = 0b0100; // altGr
  static const byte people = 0b1000; // alt
  
  static const byte masks[4] = {shift, square, circle, people};
  
  static const int  click_max_ms = 200;
  static const int dclick_max_ms = 500;
  static const int  press_min_ms = 500;
  static const int  press_max_ms = 2000;
  static const int lpress_min_ms = 2000;
  static const int lpress_max_ms = 4000;
  
  static unsigned long times[4][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
  static boolean pressed[4] = {false, false, false, false};
  
  byte key_mode_state[4];
  
  unsigned long time = millis();
  
  for(int i = 0; i < 4; i++){
    key_mode_state[i] = mode & masks[i];
    
    if( key_mode_state[i] && !pressed[i]){
      pressed[i] = true;
      times[i][1] = times[i][0]; // save last press time
      times[i][0] = time;        // save start time
    } else if (!key_mode_state[i] && pressed[i]){
      Serial.print(i);
      pressed[i] = false;
      long delta = time - times[i][0];
      if(click_max_ms > delta){
        // click
        long delta_dclick = time - times[i][1];
        if(dclick_max_ms > delta_dclick){
          // double click
          Serial.println("*D");
        } else {
          Serial.println("*C");
          
        }
      }else if(press_min_ms < delta && press_max_ms > delta){
        // press
        Serial.println("*P");
      }else if(lpress_min_ms < delta && lpress_max_ms > delta){
        // long press
        Serial.println("*L");
      }else if(lpress_max_ms < delta){
        // Holded
        Serial.println("*H");
      }
    }
  }
  
  
  return SUCCESS;
}
