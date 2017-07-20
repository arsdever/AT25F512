#ifndef AT25F512_h
#define AT25F512_h

#include <SPI.h>

#define wren 0x06
#define wrdi 0x04
#define rdsr 0x05
#define wrsr 0x01
#define rd 0x03
#define prog 0x02
#define sec_er 0x52
#define ch_er 0x62
#define rdid 0x15

const SPISettings SSS(16000000,MSBFIRST,SPI_MODE0);

class AT25F512{
private:
    byte _wp;
    byte _cs;
public:
    AT25F512(const byte& cs,const byte& wp);
    void clearSector(const unsigned long& addr);
    void clearChip();
    void program(const unsigned long& addr,const byte* buf,const byte& size);
    void statusSet(const byte& status);
    byte read(const unsigned long& addr);
    byte status();
    int info();
};

#endif
