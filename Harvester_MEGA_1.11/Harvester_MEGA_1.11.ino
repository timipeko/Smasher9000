/*
  Versio testattu defalla 4.10
  loopin loppupaassa harvesterin toimintasyklit saadetaan kohilleen kayttaen aliohjelma -komentoja seka viiveita.


*/
#include "HX711.h"
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

HX711 scale(38, 40);    // DOUT SCK

#define rajalta_recolle 62000   //askeleet rajakytkimelta recolle (koko matka)
#define hidastus_alkaa 58000    //recolle mentaessa
#define magneetin_recoraja 44000
#define hammerin_irrotuskulma 19000    //kohta, mihinka korkeuteen hammeri nostetaan ennen magneetin vapautusta
#define kiipeamisaika 10000         //ms

#define hidas 800   //ms
#define nopea 80    //80 About minimiaika!

#define enable_paalle HIGH
#define enable_pois LOW

#define recolle LOW       //dir
#define suppilolle HIGH

const int stepPin = 25;
const int dirPin = 23;
const int enable = 27;
const int raja_suppilo = 36;
const int harvest_nappi = 47;
const int harvest_ledi = 49;
const int piezo = 45;
const int magneetti = 29;

const int aja_recolle = A1;     //manuaali-ajo -kytkin
const int aja_suppilolle = A0;

unsigned char manual_reco, manual_suppilo;    //muuttujat manuaali-ajo-kytkimelle
unsigned char rajakytkin_v;
unsigned char harvest_button;
unsigned int nopeus = hidas;
unsigned long step_laskuri;
unsigned char nayttotila;
float paino;

const unsigned char PROGMEM ento_oled [] = {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xC7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0x83, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFE, 0x10, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFC, 0x38, 0x7F, 0xFF, 0x80, 0xFC, 0x1F, 0x00, 0x78, 0x0F, 0xE0, 0xF8, 0xF3, 0xC0, 0xF8, 0x0F,
  0xF8, 0x1C, 0x3F, 0xFF, 0x80, 0xF8, 0x07, 0x00, 0x70, 0x07, 0x80, 0xF8, 0xF3, 0xC0, 0x78, 0x0F,
  0xF0, 0x0E, 0xDF, 0xFF, 0x80, 0xF8, 0x87, 0x00, 0x61, 0x83, 0x84, 0xF8, 0xF3, 0xC0, 0x78, 0x0F,
  0xE1, 0x07, 0x6F, 0xFF, 0x8F, 0xF1, 0xC7, 0xE3, 0xC3, 0xE3, 0x8F, 0xF8, 0xF3, 0xCE, 0x78, 0xFF,
  0xC3, 0x8F, 0xB7, 0xFF, 0x80, 0xF1, 0xC7, 0xE3, 0xC7, 0xE3, 0x1F, 0xF8, 0xF3, 0xC0, 0x78, 0x0F,
  0xC3, 0xDF, 0xB7, 0xFF, 0x80, 0xF1, 0xC7, 0xE3, 0xC7, 0xE3, 0x1F, 0xF8, 0xF3, 0xC0, 0x78, 0x0F,
  0xE1, 0xFF, 0x6F, 0xFF, 0x80, 0xF1, 0xC7, 0xE3, 0xC7, 0xE3, 0x1F, 0x98, 0xF3, 0xC0, 0x38, 0x0F,
  0xF0, 0xFE, 0xDF, 0xFF, 0x8F, 0xF1, 0xC7, 0xE3, 0xC3, 0xE3, 0x9F, 0x18, 0xF3, 0xCE, 0x38, 0xFF,
  0xF8, 0x3D, 0xBF, 0xFF, 0x80, 0x71, 0xC7, 0xE3, 0xE0, 0x07, 0x80, 0x1C, 0x03, 0xC0, 0x38, 0x07,
  0xFC, 0x5B, 0x7F, 0xFF, 0x80, 0x71, 0xC7, 0xE3, 0xF0, 0x0F, 0xC0, 0x3C, 0x03, 0xC0, 0x78, 0x07,
  0xFE, 0xE6, 0xFF, 0xFF, 0x80, 0x71, 0xC7, 0xE3, 0xFC, 0x3F, 0xF0, 0x7F, 0x0F, 0x80, 0xF8, 0x07,
  0xFF, 0x6D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xBB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xD7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

void setup()
{
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enable, OUTPUT);
  pinMode(raja_suppilo, INPUT_PULLUP);
  pinMode(harvest_nappi, INPUT_PULLUP);
  pinMode(harvest_ledi, OUTPUT);
  pinMode(piezo, OUTPUT);
  pinMode(magneetti, OUTPUT);
  pinMode(aja_recolle, INPUT_PULLUP);
  pinMode(aja_suppilolle, INPUT_PULLUP);

  digitalWrite(enable, enable_pois);   //virta pois

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.clearDisplay();
  display.drawBitmap(0, -15, ento_oled, 128, 64, WHITE);      //logo
  display.display();
  display.invertDisplay(true);
  delay(5000);

  display.clearDisplay();
  display.invertDisplay(false);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(20, 0);
  display.print("Smasher");
  display.setCursor(30, 17);
  display.print("9000+");
  display.display();
  delay(1000);

  oled();

  Serial.begin(9600);

  scale.set_scale(-419.f);    //VAAKAN KALIBROINTI
}
void hae_raja()
{
  digitalWrite(enable, enable_paalle);      //virtaa moottorille
  digitalWrite(dirPin, suppilolle);     //kohti rajakytkinta

  rajakytkin_v = digitalRead(raja_suppilo);

  while (rajakytkin_v == HIGH)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(nopea);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(nopea);

    rajakytkin_v = digitalRead(raja_suppilo);
  }
  digitalWrite(enable, enable_pois);  //virta pois
  step_laskuri = 0;
  Serial.print(step_laskuri);
  Serial.print(" \n");
}

void kaanna_recolle()
{
  digitalWrite(magneetti, HIGH);      //KIINNI

  digitalWrite(enable, enable_paalle);      //virtaa moottorille
  digitalWrite(dirPin, recolle);
  nopeus = nopea;

  while (step_laskuri < rajalta_recolle)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(nopeus);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(nopeus);

    step_laskuri++;

    char i;
    if (step_laskuri > hidastus_alkaa)
    {
      i++;

      if (i == 5 && nopeus < hidas)
      {
        nopeus++;       //hidastuu
        i = 0;
      }
    }
    if (step_laskuri == magneetin_recoraja)    //IRTI
      digitalWrite(magneetti, LOW);        //IRROTETAAN hammerin ollessa about recolla
  }

  digitalWrite(enable, enable_pois);      //virtaa pois moottorilta

  Serial.print(step_laskuri);
  Serial.print(" \n");
}
void kaanna_suppilolle()
{
  digitalWrite(magneetti, HIGH);      //KIINNI

  digitalWrite(enable, enable_paalle);      //virtaa moottorille
  digitalWrite(dirPin, suppilolle);     //kohti rajakytkinta

  nopeus = hidas;
  //while(rajakytkin_v == HIGH)

  while (step_laskuri > 0)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(nopeus);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(nopeus);

    char i;
    i++;

    if (i == 5 && nopeus > nopea)
    {
      nopeus--;       //nopeutuu
      i = 0;
    }

    step_laskuri--;

    if (step_laskuri == hammerin_irrotuskulma)    //IRROTUS
    {
      digitalWrite(magneetti, LOW);
      delay(1000);
    }

    rajakytkin_v = digitalRead(raja_suppilo);
    if (rajakytkin_v == LOW)
    {
      digitalWrite(enable, enable_pois);  //virta pois
    }
  }
  Serial.print(step_laskuri);
  Serial.print(" \n");


}
void hammeri_ylos()
{
  digitalWrite(magneetti, HIGH);    //KIINNI

  digitalWrite(enable, enable_paalle);      //virtaa moottorille
  digitalWrite(dirPin, recolle);

  while (step_laskuri < hammerin_irrotuskulma)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(nopea);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(nopea);

    step_laskuri++;

  }
  digitalWrite(magneetti, LOW);      //IRROTUS YLHAALLA
  delay(1000);
  digitalWrite(enable, enable_pois);  //virta pois

  Serial.print(step_laskuri);
  Serial.print(" \n");
}
void hammeri_alas()
{
  digitalWrite(enable, enable_paalle);      //virtaa moottorille
  digitalWrite(dirPin, suppilolle);     //kohti rajakytkinta

  nopeus = nopea;

  while (step_laskuri > 0 )
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(nopeus);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(nopeus);

    step_laskuri--;

    rajakytkin_v = digitalRead(raja_suppilo);
    if (rajakytkin_v == LOW)
      break;
  }
  digitalWrite(enable, enable_pois);  //virta pois

  Serial.print(step_laskuri);
  Serial.print(" \n");

  digitalWrite(enable, enable_pois);  //virta pois
}
void aanimerkki()
{
  for (int i = 0; i < 100; i++)
  {
    digitalWrite(piezo, HIGH);
    delay(2);
    digitalWrite(piezo, LOW);
    delay(2);
  }
  delay(50);
  for (int i = 0; i < 100; i++)
  {
    digitalWrite(piezo, HIGH);
    delay(2);
    digitalWrite(piezo, LOW);
    delay(2);
  }
  delay(50);
  for (int i = 0; i < 250; i++)
  {
    digitalWrite(piezo, HIGH);
    delay(2);
    digitalWrite(piezo, LOW);
    delay(2);
  }
}
void oled()
{
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);

  if (nayttotila == 0)
  {
    display.setCursor(35, 0);
    display.print("Aseta");
    display.setCursor(35, 15);
    display.print("reco!");
  }
  if (nayttotila == 1)
  {
    display.setCursor(30, 8);
    display.print("Aloita?");
  }
  if (nayttotila == 2)
  {
    display.setCursor(10, 0);
    display.print("Korjaan");
    display.setCursor(10, 15);
    display.print("satoa...");
  }
  if (nayttotila == 3)
  {
    /*  display.setCursor(17, 8);
      display.print("");
      display.display();
      delay(2000);*/
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(25, 0);
    display.print("Sirkkoja");
    display.setCursor(25, 17);
    display.print(paino);
    display.print(" g");
    delay(5000);
  }

  display.display();

}
void loop() {

  rajakytkin_v = digitalRead(raja_suppilo);
  harvest_button = digitalRead(harvest_nappi);

  manual_reco = digitalRead(aja_recolle);
  manual_suppilo = digitalRead(aja_suppilolle);

  if (rajakytkin_v == LOW)
  {
    step_laskuri = 0;
  }


  if (rajakytkin_v == HIGH && manual_suppilo == LOW)  //manuaali-ajo KALIBROINTIIN
  {
    for (int x = 0; x < 1000; x++)
    {
      digitalWrite(enable, enable_paalle);      //virtaa moottorille
      digitalWrite(dirPin, suppilolle);
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(100);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(100);

      if (step_laskuri > 0)
        step_laskuri--;

      if (step_laskuri == 0)
        break;

      manual_suppilo = digitalRead(aja_suppilolle);
    }
    digitalWrite(enable, enable_pois);     //virta pois moottilta!
  }
  if (rajakytkin_v == HIGH && manual_reco == LOW)    //manuaali-ajo KALIBROINTIIN
  {
    for (int x = 0; x < 1000; x++)
    {
      digitalWrite(enable, enable_paalle);      //virtaa moottorille
      digitalWrite(dirPin, recolle);
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(100);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(100);

      step_laskuri++;

      manual_reco = digitalRead(aja_recolle);
    }
    digitalWrite(enable, enable_pois);     //virta pois moottilta!
  }
  //***************************************************************************  ALUSTAVA HARVESTERI -RUMBA


  if (harvest_button == LOW)
  {
    nayttotila = 2;
    oled();
    digitalWrite(harvest_ledi, HIGH);
    aanimerkki();
    delay(1000);
    hae_raja();       //SYKLI ALOITETTAVA AINA RAJAN HAKEMISELLA!
    // delay(3000);      //DELAYT VOI OTTAA POIS..!
    //  aanimerkki();
    kaanna_recolle();
    delay(kiipeamisaika);

    scale.power_up();

    scale.tare();


    aanimerkki();
    kaanna_suppilolle();
    hae_raja();

    paino = scale.get_units();
    nayttotila = 3;
    oled();

    // delay(1000);
    hammeri_ylos();
    //  delay(1000);
    hammeri_alas();
    delay(1000);
    paino = scale.get_units();
    scale.power_down();
    oled();
    digitalWrite(harvest_ledi, LOW);
    digitalWrite(enable, enable_pois);     //virta pois moottilta!
    delay(5000);
    nayttotila = 0;
    oled();
  }



  //************************************************************************


  Serial.print(step_laskuri);
  Serial.print(" \n");
  delay(300);
}