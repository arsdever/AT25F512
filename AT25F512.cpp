#include <AT25F512.h>

AT25F512::AT25F512(const byte& cs,const byte& wp):_cs(1<<(cs-8)),_wp(1<<(wp-8)){
    DDRB |= _wp;
    DDRB |= _cs;
    PORTB &= ~_wp;
    PORTB |= _cs;
    SPI.beginTransaction(SSS);
    SPI.begin();
}
void AT25F512::clearSector(const unsigned long& addr){
    PORTB &= ~_cs;
    SPI.transfer(wren);
    PORTB |= _cs;
    PORTB &= ~_cs;
    PORTB |= _wp;
    byte buffer[4];
    buffer[0] = sec_er;
    buffer[1] = 0;
    buffer[2] = addr>>8 & 0xFF;
    buffer[3] = addr & 0xFF;
    SPI.transfer(buffer,4);
    PORTB &= ~_wp;
    PORTB |= _cs;
    delay(1100);
}
void AT25F512::clearChip(){
    PORTB &= ~_cs;
    SPI.transfer(wren);
    PORTB |= _cs;
    PORTB &= ~_cs;
    PORTB |= _wp;
    SPI.transfer(ch_er);
    PORTB &= ~_wp;
    PORTB |= _cs;
    delay(2200);
}
void AT25F512::program(const unsigned long& addr,const byte* buf,const byte& size){
    byte buffer[4+(size&0xFF)];
    buffer[0] = prog;
    buffer[1] = 0;
    buffer[2] = addr>>8 & 0xFF;
    buffer[3] = addr & 0xFF;
    for(int i = 0;i < (size&0xFF);++i)
        buffer[i+4] = buf[i];
    PORTB &= ~_cs;
    SPI.transfer(wren);
    PORTB |= _cs;
    PORTB &= ~_cs;
    PORTB |= _wp;
    SPI.transfer(buffer,4+(size&0xFF));
    PORTB &= ~_wp;
    PORTB |= _cs;
    delayMicroseconds((long)(size&0xFF)*100);
}
void AT25F512::statusSet(const byte& status){
    PORTB &= ~_cs;
    SPI.transfer(wren);
    PORTB &= ~_cs;
    PORTB |= _cs;
    PORTB |= _wp;
    byte buffer[2];
    buffer[0] = wrsr;
    buffer[1] = status;
    SPI.transfer(buffer,2);
    PORTB &= ~_wp;
    PORTB |= _cs;
    delay(60);
}
byte AT25F512::read(const unsigned long& addr){
    PORTB &= ~_cs;
    PORTB |= _wp;
    byte buffer[5];
    buffer[0] = rd;
    buffer[1] = 0;
    buffer[2] = addr>>8 & 0xFF;
    buffer[3] = addr & 0xFF;
    SPI.transfer(buffer,5);
    PORTB &= ~_wp;
    PORTB |= _cs;
    return buffer[4];
}
byte AT25F512::status(){
    PORTB &= ~_cs;
    byte buffer[2];
    buffer[0] = rdsr;
    SPI.transfer(buffer,2);
    PORTB |= _cs;
    return buffer[1];
}
int AT25F512::info(){
    PORTB &= ~_cs;
    byte buffer[3];
    buffer[0] = rdid;
    SPI.transfer(buffer,3);
    PORTB |= _cs;
    return (buffer[1]<<8)+buffer[2];
}