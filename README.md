# XBox 360 Chatpad

A try to reverseengineering the serial protocol, so the chatpad can be used in embedded projects

## External Information Sources

http://cliffle.com/project/chatpad/

https://github.com/fdufnews/Chatpad

https://github.com/vitormhenrique/xbox_chatpad_library

https://github.com/csBlueChip/chatpad360

## Gathered informations

### Pinout

| Pin | TestPoint | Signal         |
|:----|:----------|:---------------|
| 1   | 5         | +3.3V          |
| 2   |           | RX / In        |
| 3   | 9         | TX / Out       |
| 4   | 8         | Ground         |
| 5   | 6         | Tip (Audio)    |
| 6   | 7         | Ring (Audio)   |
| 7   |           | Shield (Audio) |

### Serial Config

3v3 UART, 19200, 8n1

* 3,3 Volt UART
* 8 Databits, 1 Stoppbits
* no parity
* no flow controll
* speed at 19200 boud

### Protocol

#### Checksum Calculation

'''c
// calc checksum for compairision
byte sum = 0;
for( int i = 0; i < len - 1; i++){
  sum += buffer[2+i];
}
sum = (~sum + 1); // = sum * (-1)
'''
