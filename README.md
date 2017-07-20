This is a library for Arduino, that makes easy to use AT25F512 external flash memory chips.

>## \! Warning - Library tested only on Arduino Uno R3 boards \!

The fundamental functions are
```arduino
AT25F512(const byte& cs,const byte& wp); // Constructor. Arguments are $CS$ pin number and $WP$ pin number
void clearSector(const unsigned long& addr); // Clearing sector at address addr
void clearChip(); // Clearing chip
void program(const unsigned long& addr,const byte* buf,const byte& size); // Writing buffer buf with length size at address addr
void statusSet(const byte& status); // Sets status register by value status (!Note! with mask 0b10001100)
byte read(const unsigned long& addr); // Returns byte at address addr
byte status(); // Returns status register value
int info(); // Returns device IDs
```

For use, connect AT25F512 to your Arduino board as showing below
```
AT25F512  /^\  Arduino
     VCC  <->  3.3V
     SCK  <->  Pin 13
     SO   <->  Pin 12
     SI   <->  Pin 11
  $HOLD$  <->  3.3V
    $WP$  <->  Set by Arduino
    $CS$  <->  Set by Arduino
```
Now let's code.
This program sample demonstrates working with flash memory.

```arduino
#include <AT25F512.h>
#include <SoftwareSerial.h>
#include <hex.h>

AT25F512 mem(10,9);
String command;

void setup(){
  Serial.begin(57600);
  Serial.println("Device ID is |\t"+hex(mem.info()));
  Serial.print(">");
}

void loop() {
  if(Serial.available()){
    byte ch = Serial.read();
    if(ch==8)command.remove(command.length()-1);
    else command+=char(ch);
  }
  if(command.indexOf(char(13))>-1){
    Serial.println("Command string | "+command);
    command.toLowerCase();
    if(command.indexOf("write")>-1){
      int addr = command.substring(command.indexOf("(")+1,command.indexOf(",")).toInt();
      byte val[256];
      byte i=0;
      command = command.substring(command.indexOf(",")+1);
      while(command.indexOf(",")>-1){
        val[i++] = command.substring(0,command.indexOf(",")).toInt();
        command = command.substring(command.indexOf(",")+1);
      }
      val[i++] = command.substring(0,command.indexOf(")")).toInt();
      Serial.print("Got byte array [");
      for(int j=0;j<i;++j)
        if(j)
          Serial.print(","+hex(val[j]));
        else
          Serial.print(hex(val[j]));
      Serial.println("]");
      Serial.print("Programming...\t");
      mem.program(addr,val,i);
      Serial.println("Done.");
    }else if(command.indexOf("read")>-1){
      int addr = command.substring(command.indexOf("(")+1,command.indexOf(")")).toInt();
      Serial.println("Byte form address 0x"+hex(addr)+" |\t"+hex(mem.read(addr)));
    }else if(command.indexOf("info")>-1)
      Serial.println("Device ID is |\t"+hex(mem.info()));
    else if(command.indexOf("statusset")>-1){
      byte st = command.substring(command.indexOf('(')+1,command.indexOf(')')).toInt();
      Serial.println("Writing register state |\t"+bin(st));
      mem.statusSet(st);
    }else if(command.indexOf("status")>-1)
      Serial.println("Status register |\t"+bin(mem.status()));
    else if(command.indexOf("clearsector")>-1){
      int addr = command.substring(command.indexOf("(")+1,command.indexOf(")")).toInt();
      Serial.print("Clearing sector 0x"+hex(addr)+"...\t");
      mem.clearSector(addr);
      Serial.println("Clear.");
    }else if(command.indexOf("clear")>-1){
      Serial.print("Clearing chip...\t");
      mem.clearChip();
      Serial.println("Clear.");
    }else if(command.indexOf("printstr")>-1){
      Serial.println("Printing chip memory...\t");
      Serial.println("Press any key to stop...\t");
      Serial.println();
      Serial.print("         | ");
      for(byte i=0;i<32;++i)
        Serial.print(hex(i)+" ");
      Serial.println();
      Serial.print("---------+");
      for(byte i=0;i<32;++i)
        Serial.print("---");
      for(unsigned long i=0;i<0xFFFF;++i){
        if(i%32==0){
          Serial.println();
          Serial.print(hex(i)+" | ");
        }
        Serial.print(" "+String((char)mem.read(i))+" ");
        if(Serial.available())break;
      }
      Serial.println();
      if(Serial.available())
        Serial.println("Print process was terminated.");
      else
        Serial.println("That's all.");
      Serial.println();
    }else if(command.indexOf("print")>-1){
      Serial.println("Printing chip memory...\t");
      Serial.println("Press any key to stop...\t");
      Serial.println();
      Serial.print("         | ");
      for(byte i=0;i<32;++i)
        Serial.print(hex(i)+" ");
      Serial.println();
      Serial.print("---------+");
      for(byte i=0;i<32;++i)
        Serial.print("---");
      for(unsigned long i=0;i<0xFFFF;++i){
        if(i%32==0){
          Serial.println();
          Serial.print(hex(i)+" | ");
        }
        Serial.print(hex(mem.read(i))+" ");
        if(Serial.available())break;
      }
      Serial.println();
      if(Serial.available())
        Serial.println("Print process was terminated.");
      else
        Serial.println("That's all.");
      Serial.println();
    }
    command = "";
    Serial.print(">");
  }  
}
```
> #### This code needs library <hex.h>, that you can find at [here](https://github.com/arsdever/HEX)</br>
> #### But <AT25F512.h> not requires <hex.h> library
When we run code, the first output will be:

```
Device ID is |FFFF
>
```
Then console waiting for commands. The commands are:
```arduino
write(address,bytes[1..255]) // Number of bytes recommended maximum 128. Witing bytes at address
:: OUTPUT
  Got array [#{bytes}]
  Programming... #{delay_programming_time}Done.
  > #{wayting for new command}
read(address)                // Prints byte from address
:: OUTPUT
  Byte form address 0x#{address_in_hex_format} | #{readed_byte_in_hex_format}
  > #{wayting for new command}
info                         // Prints device's manufacturerID + deviceID in hex format
:: OUTPUT
  Device ID is | #{device_id_in_hex_format}
  > #{wayting for new command}
status                       // Prints device's status register in binary format
:: OUTPUT
  Status register | #{status_register_in_bin_format}
  > #{wayting for new command}
statusSet                    // Sets device's status register (!NOTE! with mask 0b10001100)
:: OUTPUT
  Writing register state | #{required_status_in_bin_format}
  > #{wayting for new command}
clearSector(address)         // Clear sector from address
:: OUTPUT
  Clearing sector 0x#{address_in_hex_format}... #{delay_clearing_time}Clear.
  > #{wayting for new command}
clear                        // Clears whole device
:: OUTPUT
  Clearing chip... #{delay_clearing_time}Clear.
  > #{wayting for new command}
printStr                     // Prints contain of device as string. Terminates if another command entered
:: OUTPUT
  Printing chip memory...
  Press any key to stop...
  
         | 00 01 02 03 04 05 .. .. 1F
  -------+---------------------------
  000000 | ** ** ** ** ** ** .. .. **
  000020 | ** ** ** ** ** ** .. .. **
  000040 | ** ** ** ** ** ** .. .. **
  000060 | ** ** ** ** ** ** .. .. **
  .. .. .. .. .. .. .. .. .. .. .. ..
  00FFF0 | ** ** ** ** ** ** .. .. **
  
  #{if_terminated}Print process was terminated.
  #{else}That's all.
  >
  // !Note! the ** are bytes readed from address (column_value + row_value) converted to char
print                        // Prints every byte of device in hex format
:: OUTPUT
  #{same_as_printStr}
  // !Note! difference is here ** are bytes are printed in hex format
```
If my code isn't working please contact to me to fix any error in that. Email: arsen.gharagyozyn.96@gmail.com
