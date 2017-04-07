class ButtonHandler{
  public:
    // Constructor
    ButtonHandler(byte id, byte keyMask, boolean isBitMask = true,
                  unsigned int clickMax_ms = 200,
                  unsigned int pressMin_ms = 500,
                  unsigned int longPressMin_ms = 2000);    
    
    enum Event { 
      EV_NONE=0, 
      EV_CLICK,
      EV_DOUBLE_CLICK, 
      EV_PRESS, 
      EV_LONGPRESS 
    };
    
    //void init(){} // not needed
    Event handle(unsigned long timeStamp, byte data);
    
    boolean isDebug = false;
    
  private:
    const byte id;
    const int  click_max_ms;
    const int  press_min_ms;
    const int lpress_min_ms;
    
    byte mask;
    boolean isBitMask;
    
    boolean pressed;
    unsigned long pressStart;
    unsigned long lastPressStart;
};

ButtonHandler::ButtonHandler(byte id, byte keyMask, boolean isBitMask,
                             unsigned int clickMax_ms, unsigned int pressMin_ms, unsigned int longPressMin_ms):
      id(id),
      click_max_ms(clickMax_ms), press_min_ms(pressMin_ms), lpress_min_ms(longPressMin_ms),
      mask(keyMask), isBitMask(isBitMask), pressed(false){}

ButtonHandler::Event ButtonHandler::handle(unsigned long timeStamp, byte data){
  boolean state = (isBitMask ? (data & mask) : (data == mask)) ? true : false;
  
  if(state && !pressed){
    pressed = true;
    lastPressStart = pressStart;  // save last press time
    pressStart = timeStamp;       // save start time
  } else if (!state && pressed){
    pressed = false;
    long delta = timeStamp - pressStart;
    
    if(click_max_ms > delta){
      long delta_dclick = timeStamp - lastPressStart;
      if(press_min_ms <= delta_dclick){
        Serial.print(id);
        Serial.println("*C"); // click
        return EV_CLICK;
      } else {
        Serial.print(id);
        Serial.println("*D"); // double click
        return EV_DOUBLE_CLICK;
      }
   
    }else if(press_min_ms < delta && lpress_min_ms > delta){
      Serial.print(id);
      Serial.println("*P");  // press
      return EV_PRESS;
      
    }else if(lpress_min_ms < delta){
      Serial.print(id);
      Serial.println("*L");  // long press
      return EV_LONGPRESS;
    }
  }  
  return EV_NONE;
}
