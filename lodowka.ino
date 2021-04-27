#include "SPI.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "konfig.h"
#include "grafika.h"
//temp


int delay_output;
int incomingByte = 0;

unsigned int ADCValue;


unsigned long runtime = 0, chilltime = 0;
unsigned long ticker = 0;
unsigned long tickerPamiecLCD = 0;
unsigned long tickerPamiecKOMPRESOR = 0;
unsigned long tickerPamiecDBG = 0;
unsigned long tickerPamiecTEMP = 0;
unsigned long tickerPamiecGRAF = 0;

unsigned long tickerRoznica = 0;


boolean kompresor = false;
boolean relay_on, relay_on_off;
boolean stringComplete = false;  
boolean start = true;


float R1 = 10000;
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
float temperaturaRaw;
float temperatura;
float temperaturaRaw2;
float temperatura2;
float moc;

struct config_t {
  float temp1_min;
  float temp1_max;
  float temp2_min;
  float temp2_max;

} konfiguracja;

//char temp2_t[5];
//char temp1_t[5];

OneWire onewire(ONEWIRE_PIN);
DallasTemperature sensors(&onewire);

//int ANALOG_IN = 0; 
//int ANALOG_IN2 = 1; 

// definicje

// czytaj dane RAW z analogow
float getTemperatureRaw(int ADC_PORT) {
  float raw_val =  analogRead(ADC_PORT);
  return raw_val;
}

// konwersja temperatury
// WIP: dopasowac do danego typu czujnika
float convertTemperature(float TEMP_IN) { 
    R2 = R1 * (1023.0 / TEMP_IN - 1.0);
    logR2 = log(R2);
    T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
    float converted = T - 273.15;
   //float converted = 0.00007*TEMP_IN*TEMP_IN-0.1849*TEMP_IN+103.02+2.3;
  return converted;
}

// alternatywa dla czujnikow 1wire
// 
float getTemperatureDIG(byte SENS_ADDR) {
    float DIG_TEMP = sensors.getTempC(SENS_ADDR);
 
    if(DIG_TEMP != DEVICE_DISCONNECTED_C) 
      {
        return DIG_TEMP;
      } 
    else
      {    
        return;
      }
}

// tony do przyciskow
void toneOK() {
    tone(7, 200, 70);
    delay(70);
    tone(7, 500, 100);
    delay(100);
    tone(7, 900, 120);
    delay(120);
}

void toneNOOK() {
    tone(7, 900, 70);
    delay(70);
    tone(7, 500, 100);
    delay(100);
    tone(7, 200, 120);
    delay(120);
}


// produkuje JSON sensora
void spitJSON(float temp, float raw, String sensor, String sensorName) {
  Serial.print("\"");
  Serial.print(sensor);
  Serial.print("\": ");
  Serial.print("{ \"nazwa\": \"");
  Serial.print(sensorName);
  Serial.print("\", \"ADC\": ");
  Serial.print(raw);
  Serial.print(", \"temp\": ");
  Serial.print(temp);
  Serial.print("}");
 }

// wiadomo

void setup() {

  //analogReference(EXTERNAL);
  // moj przekaznik zalacza sie przy stanie niskim, wiec wlacz wysoki
  pinMode(KOMPRESOR, OUTPUT);
  digitalWrite(KOMPRESOR, HIGH);

  pinMode(PRZYCISK_A, INPUT);
  pinMode(PRZYCISK_B, INPUT);
  
  pinMode(ANALOG_IN, INPUT_PULLUP);
  pinMode(ANALOG_IN2, INPUT_PULLUP);
 
  Serial.begin(9600);

  EEPROM_readAnything(0, konfiguracja);

  sensors.begin();

  SetupDisplay();
}

void loop() {
  ticker = millis();

  czytajSerial();

// pseudo threading ;)

  if((ticker - tickerPamiecTEMP ) >= 1000UL) {
#if (SENSOR1 ==  ANALOG)
    temperaturaRaw = getTemperatureRaw(ANALOG_IN);
    temperatura = convertTemperature(temperaturaRaw);
#elif (SENSOR1 = DIGITAL)
    temperatura = getTemperatureDIG(czujnik1);
#else
    temperatura = -255;
#endif

#if (SENSOR2 ==  ANALOG)
    temperaturaRaw2 = getTemperatureRaw(ANALOG_IN2);
    temperatura2 = convertTemperature(temperaturaRaw2);
#elif (SENSOR2 = DIGITAL)
    temperatura2 = getTemperatureDIG(czujnik2);
#else
    temperatura2 = -255;
#endif

    tickerPamiecTEMP =  ticker;
  }
  
  if((ticker - tickerPamiecDBG) >= 5000UL) {
      serialJSON();
      tickerPamiecDBG =  ticker;
  }
  
  if((ticker - tickerPamiecGRAF) >= 5000UL) {
    update_temps(temperatura, temperatura2, CYAN, BLUE);
    tickerPamiecGRAF = ticker;
  }

  if((ticker - tickerPamiecLCD) >= 5000UL) {
    aktualizuj_minmax(konfiguracja.temp1_min,konfiguracja.temp1_max,konfiguracja.temp2_min,konfiguracja.temp2_max);
    draw_temps(temperatura, temperatura2, CYAN, BLUE);
    tickerPamiecLCD =  ticker;
  }
  
  if((ticker - tickerPamiecKOMPRESOR) >= 10000UL) {
    kompresorLoop();
    tickerPamiecKOMPRESOR = ticker;
  }

}

// zalacz / wylacz kompresor
void kompresorLoop() {
  if(temperatura2 > konfiguracja.temp2_max)
  {
    digitalWrite(KOMPRESOR, LOW);
    lcd_kompresor(true);
    if(!kompresor) kompresor = true;
  }
  else if(temperatura2 <= konfiguracja.temp2_min)
  {
     digitalWrite(KOMPRESOR, HIGH);
     lcd_kompresor(false);
     if(kompresor) kompresor = false;
  }

  runtime += SLEEP_SEC;
  if(kompresor) chilltime += SLEEP_SEC;
  
  moc=chilltime * KOMPRESOR_MOC / 3600 / 1000;

}

void serialJSON() {
  Serial.print("{ \"czujniki\": {");
  spitJSON(temperatura, temperaturaRaw, "1", nazwa_czujnik1);
  Serial.print(",");
  spitJSON(temperatura2, temperaturaRaw2, "2", nazwa_czujnik2);
  Serial.print("}, \"stats\": { ");
  Serial.print("\"runtime\": ");
  Serial.print(runtime);
  Serial.print(", \"chilltime\": ");
  Serial.print(chilltime);
  Serial.print(", \"moc_kwh\": ");
  Serial.print(moc);
  Serial.print(", \"kompresor\": ");
  if(kompresor) Serial.print("true");
  else if(!kompresor) Serial.print("false");
  Serial.print("}");
  Serial.print(", \"limits\": {");
  Serial.print("\"sensor1_min\": ");
  Serial.print(konfiguracja.temp1_min);
  Serial.print(", \"sensor1_max\": ");
  Serial.print(konfiguracja.temp1_max);
  Serial.print(", \"sensor2_min\": ");
  Serial.print(konfiguracja.temp2_min);
  Serial.print(", \"sensor2_max\": ");
  Serial.print(konfiguracja.temp2_max);
  Serial.print("}");
  Serial.print("}\n");
}

void czytajSerial() {
  String readString;
   if (Serial.available() > 0) {
   char incomingCharacter = Serial.read();

   switch (incomingCharacter) {
    case 'H': {
        float newTempHi = Serial.parseFloat();
        Serial.print("New max temp1:");
        Serial.print(newTempHi);
        Serial.println();
        konfiguracja.temp1_max = newTempHi;
        break;
     }
    case 'L': {
        float newTempLow = Serial.parseFloat();
        Serial.print("New min temp1:");
         Serial.print(newTempLow);
        Serial.println();
        konfiguracja.temp1_min = newTempLow;
        break; 
        }
    case 'G': {
        float newTempHi = Serial.parseFloat();
        Serial.print("New max temp2:");
        Serial.print(newTempHi);
        Serial.println();
        konfiguracja.temp2_max = newTempHi;
        break;
     }
    case 'K': {
        float newTempLow = Serial.parseFloat();
        Serial.print("New min temp2:");
        Serial.print(newTempLow);
        Serial.println();
        konfiguracja.temp2_min = newTempLow;
        break; 
        }
    case 'S': {
      EEPROM_writeAnything(0, konfiguracja);
      Serial.println("New values saved!");
        break; 
        }
   case 'X': {
      EEPROM_readAnything(0, konfiguracja);
      Serial.println("Data reset from EEPROM!");
        break; 
        }
 // end switch
     }
 // end if serial.available
  }
}
