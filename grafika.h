// tft
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

//#include "font_Arial.h"

#define GREY    0x8410
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// skalowanie wykresu
int incrementation = 24;
int scale1 = 1;
int scale2 = 2;
int scale4 = 4;
int scale8 = 8;

// temperatury/wartosci graniczne dla wykresu
int skala_min = -60;
int skala_max = 60;

// Ustawienie wyswietlacza, logo itd ;)
//
void SetupDisplay() {
    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(BLACK);

//  BOOT LOGO
//
#ifdef BOOT_LOGO
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
    tft.setCursor(100,230); tft.setTextColor(MAGENTA); tft.setTextSize(1); tft.println("by SO1Z");
    delay(500);
    tft.fillScreen(BLACK);
#endif
// thank you

// Wykres
    tft.fillRect(15,0,3,100,WHITE);
    tft.fillRect(15,100,303,3,WHITE);
    //tft.drawLine(13,100,13,106,WHITE);
    for(int n=23, inc=10; n<=293; ++inc, n+= inc) { tft.drawLine(n,100,n,106,WHITE); }
    tft.drawLine(15,50,293,50,WHITE);
    tft.setCursor(2,48);
    tft.setTextColor(GREEN);
    tft.setTextSize(1);
    tft.println("0");
    tft.setCursor(1,0);
    tft.setTextColor(YELLOW);
    tft.setTextSize(1);
    tft.println(skala_min);
    tft.setCursor(1,103);
    tft.setTextColor(RED);
    tft.setTextSize(1);
    tft.println(skala_max);
//    tft.drawLine(0,99,15,101,RED);
    tft.fillRect(0,99,15,2,RED);

//      opisy

// temp1
    tft.setCursor(10,115);
    tft.setTextColor(BLUE);
    tft.setTextSize(1);
    tft.println("Temp. zamrazarki");
// temp2
    tft.setCursor(180,115);
    tft.setTextColor(CYAN);
    tft.setTextSize(1);
    tft.println("Temp. chlodziarki");
}

// Aktualizuje na LCD min/max monitorowane wartosci
void aktualizuj_minmax(float temp1_min, float temp1_max, float temp2_min, float temp2_max) {
    tft.setCursor(10, 160);
    tft.setTextColor(BLUE, BLACK);
    tft.setTextSize(1);
    tft.print("< ");
    tft.print(temp1_min);
    tft.print(" : ");
    tft.print(temp1_max);
    tft.print(" >");
    tft.println();
  
    tft.setCursor(180, 160);
    tft.setTextColor(CYAN, BLACK);
    tft.setTextSize(1);
    tft.print("< ");
    tft.print(temp2_min);
    tft.print(" : ");
    tft.print(temp2_max);
    tft.print(" >");
    tft.println();
}


// Podaje temperature na LCD
//
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

// Rysowanie wykresu
// WIP: skala itd.
//
void draw_temps(float temp, float temp2, int kolor, int kolor2) {
  
//  unsigned long start = micros();
  int valba = map(temp,skala_min,skala_max, 100,0);
  tft.fillCircle(incrementation,valba,1,kolor2);
  
  int valbb = map(temp2,skala_min,skala_max, 100,0);
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
}

void lcd_kompresor(bool pracuje) {
  if(pracuje == false) {
      tft.fillRect(100,220,120,20,BLACK);
      tft.setCursor(100, 220);
      tft.setTextColor(GREEN, BLACK);
      tft.setTextSize(2);
      tft.println("ODPOCZYWA");
  } else {
      tft.fillRect(100,220,120,20,BLACK);
      tft.setCursor(100, 220);
      tft.setTextColor(RED, BLACK);
      tft.setTextSize(2);
      tft.println("CHLODZENIE");
  }
  
}
