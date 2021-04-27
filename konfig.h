#include <EEPROM.h>
#include <Arduino.h>  // for type definitions


// inne
#define TFT_DC 6
#define TFT_CS 10
#define TFT_MOSI 11
#define TFT_CLK 13
#define TFT_RST 2
#define TFT_MISO 12

#define ONEWIRE_PIN 4

#define KOMPRESOR 5
#define KOMPRESOR_MOC 145.0

#define PRZYCISK_A 2
#define PRZYCISK_B 3

#define ANALOG_IN A0
#define ANALOG_IN2 A1

#define SLEEP_SEC 10

// undefine/zahashuj i logo wyłączone
#define BOOT_LOGO

byte czujnik1[8] = {0x28, 0xB1, 0x6D, 0xA1, 0x3, 0x0, 0x0, 0x11};
const byte relay = 2; 


template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          EEPROM.write(ee++, *p++);
    return i;
}

template <class T> int EEPROM_readAnything(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          *p++ = EEPROM.read(ee++);
    return i;
} 
