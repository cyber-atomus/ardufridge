// tft
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

//#include "font_Arial.h"

#include "SPI.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "konfig.h"
// tft

#define TFT_DC 6
#define TFT_CS 10
#define TFT_MOSI 11
#define TFT_CLK 13
#define TFT_RST 2
#define TFT_MISO 12

#define GREY    0x8410
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
// inne

#define ONEWIRE_PIN 4

#define KOMPRESOR 5
#define KOMPRESOR_MOC 145.0

#define PRZYCISK_A 2
#define PRZYCISK_B 3

#define ANALOG_IN A0
#define ANALOG_IN2 A1

#define SLEEP_SEC 10
//temp


int delay_output;
int incrementation = 24;
int incomingByte = 0;
int scale1 = 1;
int scale2 = 2;
int scale4 = 4;
int scale8 = 8;
unsigned int ADCValue;


unsigned long runtime = 0, chilltime = 0;
unsigned long ticker = 0;
unsigned long tickerPamiecLCD = 0;
unsigned long tickerPamiecKOMPRESOR = 0;
unsigned long tickerPamiecDBG = 0;
unsigned long tickerPamiecTEMP = 0;
unsigned long tickerPamiecGRAF = 0;

unsigned long tickerRoznica = 0;


byte czujnik1[8] = {0x28, 0xB1, 0x6D, 0xA1, 0x3, 0x0, 0x0, 0x11};
const byte relay = 2; 

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

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
OneWire onewire(ONEWIRE_PIN);
DallasTemperature sensors(&onewire);

//int ANALOG_IN = 0; 
//int ANALOG_IN2 = 1; 

// definicje

float getTemperatureRaw(int ADC_PORT) {
  float raw_val =  analogRead(ADC_PORT);
  return raw_val;
}

float convertTemperature(float TEMP_IN) { 
    R2 = R1 * (1023.0 / TEMP_IN - 1.0);
    logR2 = log(R2);
    T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
    float converted = T - 273.15;
   //float converted = 0.00007*TEMP_IN*TEMP_IN-0.1849*TEMP_IN+103.02+2.3;
  return converted;
}

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

void SetupDisplay() {
    tft.begin();
    tft.setRotation(1);

    tft.fillScreen(BLACK);
    int delay1;
    tft.fillRect(110,10,3,200,WHITE);  delay1=(((60000*4)/80)/(8*1.5)); tone(7,294,((60000*4)/80)/(8*1.5)); delay(delay1);
    tft.setCursor(50, 10); tft.setTextColor(BLUE); tft.setTextSize(3); tft.println("L");
    tft.setCursor(250, 130); tft.setTextColor(CYAN); tft.setTextSize(3); tft.println("S R");

    tft.fillRect(210,10,3,200,WHITE);  delay1=(((60000*4)/80)/(16)) ; tone(7,392,((60000*4)/80)/(16)); delay(delay1);
     tft.setCursor(50, 40); tft.setTextColor(CYAN); tft.setTextSize(3); tft.println("O");
    tft.setCursor(250, 190); tft.setTextColor(CYAN); tft.setTextSize(3); tft.println("A ");

    tft.fillRect(110,10,100,3, WHITE);  delay1=(((60000*4)/80)/(4*1.5));tone(7, 523,((60000*4)/80)/(4*1.5));delay(delay1);
    tft.setCursor(50, 70); tft.setTextColor(BLUE); tft.setTextSize(3); tft.println("D");
    tft.setCursor(250, 10); tft.setTextColor(CYAN); tft.setTextSize(3); tft.println("S  ");

    tft.fillRect(110,210,100,3, WHITE);  delay1=(((60000*4)/80)/(8));tone(7, 494,((60000*4)/80)/(8)); delay(delay1);
    tft.setCursor(50, 100); tft.setTextColor(CYAN); tft.setTextSize(3); tft.println("O");
    tft.setCursor(250, 40); tft.setTextColor(CYAN); tft.setTextSize(3); tft.println("I S");

    tft.fillRect(110,66,100,3, WHITE);  delay1=(((60000*4)/80)/(16*1.5));tone(7, 392,((60000*4)/80)/(16*1.5));delay(delay1);
    tft.setCursor(50, 130); tft.setTextColor(BLUE); tft.setTextSize(3); tft.println("W");
    tft.setCursor(250, 100); tft.setTextColor(CYAN); tft.setTextSize(3); tft.println("E A");

    tft.fillRect(120,20,1,28, WHITE);  delay1=(((60000*4)/80)/(16*1.5));tone(7, 330,((60000*4)/80)/(16*1.5));delay(delay1);
    tft.setCursor(50, 160); tft.setTextColor(CYAN); tft.setTextSize(3); tft.println("K");
    tft.setCursor(250, 160); tft.setTextColor(CYAN); tft.setTextSize(3); tft.println("I T");

    tft.fillRect(120,77,1,28, WHITE);  delay1=(((60000*4)/80)/(16*1.5));tone(7, 440,((60000*4)/80)/(16*1.5)); delay(delay1);
    tft.setCursor(50, 190); tft.setTextColor(BLUE); tft.setTextSize(3); tft.println("A");
    tft.setCursor(250, 70); tft.setTextColor(CYAN); tft.setTextSize(3); tft.println("L M");

    delay1= (587,((60000*4)/80)/(2));tone(7, 587,((60000*4)/80)/(2)); delay(delay1);

    delay(500);
    tft.fillScreen(BLACK);


    // gdzie ten prostokat?!
    tft.fillRect(15,0,3,100,WHITE);
    tft.fillRect(15,100,303,3,WHITE);
    ///tft.setFont(&FreeMonoBold9pt7b);
    //zamrazarka
    tft.setCursor(10,115);
    tft.setTextColor(BLUE);
    tft.setTextSize(1);
    tft.println("Temp. zamrazarki");
    // chldziarka
    tft.setCursor(180,115);
    tft.setTextColor(CYAN);
    tft.setTextSize(1);
    tft.println("Temp. chlodziarki");
    // grid
    //tft.drawLine(13,100,13,106,WHITE);
    tft.drawLine(23,100,23,106,WHITE);
    tft.drawLine(33,100,33,106,WHITE);
    tft.drawLine(43,100,43,106,WHITE);
    tft.drawLine(53,100,53,106,WHITE);
    tft.drawLine(63,100,63,106,WHITE);
    tft.drawLine(73,100,73,106,WHITE);
    tft.drawLine(83,100,83,106,WHITE);
    tft.drawLine(93,100,93,106,WHITE);
    tft.drawLine(103,100,103,106,WHITE);
    tft.drawLine(113,100,113,106,WHITE);
    tft.drawLine(123,100,123,106,WHITE);
    tft.drawLine(133,100,133,106,WHITE);
    tft.drawLine(143,100,143,106,WHITE);
    tft.drawLine(153,100,153,106,WHITE);
    tft.drawLine(163,100,163,106,WHITE);
    tft.drawLine(173,100,173,106,WHITE);
    tft.drawLine(183,100,183,106,WHITE);
    tft.drawLine(193,100,193,106,WHITE);
    tft.drawLine(203,100,203,106,WHITE);
    tft.drawLine(213,100,213,106,WHITE);
    tft.drawLine(223,100,223,106,WHITE);
    tft.drawLine(233,100,233,106,WHITE);
    tft.drawLine(243,100,243,106,WHITE);
    tft.drawLine(253,100,253,106,WHITE);
    tft.drawLine(263,100,263,106,WHITE);
    tft.drawLine(273,100,273,106,WHITE);
    tft.drawLine(283,100,283,106,WHITE);
    tft.drawLine(293,100,293,106,WHITE);
    tft.drawLine(15,50,293,50,WHITE);
    tft.setCursor(2,48);
    tft.setTextColor(GREEN);
    tft.setTextSize(1);
    tft.println("0");
    tft.setCursor(1,0);
    tft.setTextColor(YELLOW);
    tft.setTextSize(1);
    tft.println("60");
       tft.setCursor(1,103);
    tft.setTextColor(RED);
    tft.setTextSize(1);
    tft.println("-60");
//    tft.drawLine(0,99,15,101,RED);
    tft.fillRect(0,99,15,2,RED);

}

void setup() {

  //toneOK();
  //delay(600);
  //toneNOOK();
  //analogReference(EXTERNAL);
  
  pinMode(KOMPRESOR, OUTPUT);
  
  digitalWrite(KOMPRESOR, HIGH);
  
  pinMode(ANALOG_IN, INPUT_PULLUP);
  pinMode(ANALOG_IN2, INPUT_PULLUP);
 
  Serial.begin(9600);

  EEPROM_readAnything(0, konfiguracja);

  sensors.begin();

  SetupDisplay();

}

void loop() {
//  EEPROM_writeAnything(0, konfiguracja);
  ticker = millis();

  czytajSerial();

  if((ticker - tickerPamiecTEMP ) >= 1000UL) {
    temperaturaRaw = getTemperatureRaw(ANALOG_IN);
    temperatura = convertTemperature(temperaturaRaw);
  
    temperaturaRaw2 = getTemperatureRaw(ANALOG_IN2);
    temperatura2 = convertTemperature(temperaturaRaw2);
    
    tickerPamiecTEMP =  ticker;

  }
  //Serial.print("ticker: ");
  //Serial.println(ticker);
  
  if((ticker - tickerPamiecDBG) >= 5000UL) {
      serialJSON();
      tickerPamiecDBG =  ticker;
  }
  
  if((ticker - tickerPamiecGRAF) >= 5000UL) {
    update_temps(temperatura, temperatura2, CYAN, BLUE);
    tickerPamiecGRAF = ticker;
  }

  if((ticker - tickerPamiecLCD) >= 5000UL) {
    
    tft.setCursor(10, 160);
    tft.setTextColor(BLUE, BLACK);
    tft.setTextSize(1);
    tft.print("< ");
    tft.print(konfiguracja.temp1_min);
    tft.print(" : ");
    tft.print(konfiguracja.temp1_max);
    tft.print(" >");
    tft.println();
  
    tft.setCursor(180, 160);
    tft.setTextColor(CYAN, BLACK);
    tft.setTextSize(1);
    tft.print("< ");
    tft.print(konfiguracja.temp2_min);
    tft.print(" : ");
    tft.print(konfiguracja.temp2_max);
    tft.print(" >");
    tft.println();
    
    draw_temps(temperatura, temperatura2, CYAN, BLUE);
    tickerPamiecLCD =  ticker;
  }
  
  if((ticker - tickerPamiecKOMPRESOR) >= 10000UL) {
    kompresorLoop();
    tickerPamiecKOMPRESOR = ticker;
  }

}

void kompresorLoop() {
  if(temperatura2 > konfiguracja.temp2_max)
  {
    digitalWrite(KOMPRESOR, LOW);
      tft.fillRect(100,220,120,20,BLACK);
      tft.setCursor(100, 220);
      tft.setTextColor(RED, BLACK);
      tft.setTextSize(2);
      tft.println("CHLODZENIE");
    if(!kompresor)
    {
      kompresor = true;
 
    }
  }
  else if(temperatura2 <= konfiguracja.temp2_min)
  {
    digitalWrite(KOMPRESOR, HIGH);
      tft.fillRect(100,220,120,20,BLACK);
      tft.setCursor(100, 220);
      tft.setTextColor(GREEN, BLACK);
      tft.setTextSize(2);
      tft.println("ODPOCZYWA");
    if(kompresor)
    {
      kompresor = false;

    }
  }

//  delay(SLEEP_SEC * 1000);
  runtime += SLEEP_SEC;
  if(kompresor) chilltime += SLEEP_SEC;
  
  moc=chilltime * KOMPRESOR_MOC / 3600 / 1000;
  

}

void serialJSON() {
  Serial.print("{ \"czujniki\": {");
  spitJSON(temperatura, temperaturaRaw, "1", "zamrazarka");
  Serial.print(",");
  spitJSON(temperatura2, temperaturaRaw2, "2", "komora");
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

void update_temps(float temp, float temp2, int kolor, int kolor2) {
  //sensor1
  tft.fillRect(30,130,120,28,BLACK);
  tft.setCursor(30,130);
  tft.setTextColor(kolor2, BLACK);
  tft.setTextSize(3);
  int temp_p = temp;
  tft.print(temp_p);
  tft.print((char)247);
  tft.println("C");
  //sensor 2
  tft.fillRect(200,130,120,28,BLACK);
  tft.setCursor(200,130);
  tft.setTextColor(kolor, BLACK);
  tft.setTextSize(3);
  //tft.setFont(&FreeMono18pt7b);

  int temp2_p = temp2;
  //sprintf(temp2_t,"%d", temp2_p, 0xdf);
  tft.print(temp2_p);
  tft.print((char)247);
  tft.println("C");
  //tft.setFont(&FreeMono9pt7b);

  
}
void draw_temps(float temp, float temp2, int kolor, int kolor2) {
  
//  unsigned long start = micros();
  int valba = map(temp,-60,60, 100,0);
  tft.fillCircle(incrementation,valba,1,kolor2);
  
  int valbb = map(temp2,-60,60, 100,0);
  tft.fillCircle(incrementation,valbb,1,kolor);
  
  incrementation++;
  
  if(scale1==27){
    tft.fillRect(3,100,16,60,BLACK);
  }
  if(incrementation>312){
    tft.fillRect(10,100,100,12,BLACK);
    scale1 = scale1+26;
    scale2 = scale2+26;
    scale4 = scale4+26;
    scale8 = scale8+26;
    tft.setCursor(26,100);
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    tft.println(scale1);
    tft.setCursor(41,100);
    if(scale2>10){
      tft.setCursor(39,100);
      }
    if(scale2<100){
      tft.setTextColor(WHITE);
      tft.setTextSize(1);
      tft.println(scale2);
      }
    tft.setCursor(61,170);
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    tft.println(scale4);
    tft.setCursor(101,100);
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    tft.println(scale8);
    tft.fillRect(5,0,303,146,BLACK);

    incrementation = 24;
  }
//  return micros() - start;

}

void czytajSerial() {
  String readString;
   if (Serial.available() > 0) {
   char incomingCharacter = Serial.read();
 //  Serial.print("dev:");
  // Serial.println(incomingCharacter);

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

         }
 }


}
