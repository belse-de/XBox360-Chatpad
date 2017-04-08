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

#### General Structure

| Type          | Header | Data  | Checksum |
|:--------------|:-------|:------|:---------|
| Length [Byte] | 2      | 0 - 5 | (1)      |
Checksum seams not to exist if data length is zero.

#### Checksum Calculation

```c
// calc checksum for compairision
byte sum = 0;
for( int i = 0; i < len - 1; i++){
  sum += buffer[2+i];
}
sum = (~sum + 1); // = sum * (-1)
```
#### Header Structure

length 2 bytes
h0 h1
1. h0 - Start Byte
2. h1 - Controlls
  * 0xF0 - unkown / maybe error code / return status
  * 0x0F - 4 bits length [0-5] observed

##### Observed Package Start Bytes

* 41 - Serial number / Firmware Version ???
* 87 - Command to ChatPad ???
* C3 - ???
* A5 - State of ChatPad
* B4 - Botton Change

#### Known Packages

41 02 08 11 01 11 7D 01 00 00 00 A1 - serial / version ???

A5 X5 d0 d1 d2 d3 d4 CS - status report

B4 C5 00 mk k0 k1 ?? CS - button change

87 02 8C 1F CC - init

87 02 8C 1B D0 - wake up
