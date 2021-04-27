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

// undefine/zahashuj i logo wyłączone
#define BOOT_LOGO

// jakie czujniki uzywane?
// ANALOG - piny ponizej ANALOG_IN/ANALOG_IN2
// DIGITAL - 1wire, nalezy ustawic adresy czujnik1 czujnik2
//
#define SENSOR1 ANALOG
#define SENSOR2 DIGITAL

// wejscia czujnikow analogowych
#define ANALOG_IN A0
#define ANALOG_IN2 A1

//adresy czujnikow 1wire
byte czujnik1[8] = {0x28, 0xB1, 0x6D, 0xA1, 0x3, 0x0, 0x0, 0x11};
byte czujnik2[8] = {0x28, 0xB1, 0x6D, 0xA1, 0x3, 0x0, 0x0, 0x12};

String nazwa_czujnik1="zamrazarka";
String nazwa_czujnik2="komora";

//
// koniec definicji czujnikow
//
// nic takiego
//
#define SLEEP_SEC 10



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
