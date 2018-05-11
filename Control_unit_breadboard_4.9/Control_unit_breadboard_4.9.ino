/*  Toiminta: vasemmasta namiskasta (jos rajakytkin on auki) liikutaan suppilon suuntaan.
 *            oikeasta namiskasta liikutaan recon suuntaan.
 *            serialmonitorista voi seurata askelia.
 *      
 *      !!!   alla olevat 3 arvoa kalibroitava kohdalleen (vas ja oik nappeja kayttaen) ennen harvesteri -nappulan painamista!
 *          
 *            loopin loppupaassa harvesterin toimintasyklit saadetaan kohilleen kayttaen aliohjelma -komentoja seka viiveita.
 * 
 * 
 */


#define rajalta_recolle 15000   //askeleet rajakytkimelta recolle (koko matka)
#define magneetin_raja 100   //kohta ennen rajakytkinta, jolloin magneetti vapautetaan/lukitaan
#define hammerin_nostokulma 5000    //kohta, mihinka korkeuteen hammeri nostetaan ennen uutta lyontia

const int stepPin = 4;
const int dirPin = 5;
const int enable = 11;
const int raja_suppilo = 8;
//const int raja_reco = 9;
const int vasen = A2;
const int oikea = A5;
const int harvest_nappi = 6;
const int vasen_ledi = A4;
const int oikea_ledi = A3;
const int harvest_ledi = 7;
const int piezo = 12;
const int magneetti = 10;

unsigned char rajakytkin_v;
//unsigned char rajakytkin_o;
unsigned char vas_button;
unsigned char oik_button;
unsigned char harvest_button;

unsigned long step_laskuri;

void setup() 
{
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enable, OUTPUT);
  pinMode(raja_suppilo, INPUT_PULLUP);
//  pinMode(raja_reco, INPUT_PULLUP);
  pinMode(vasen, INPUT_PULLUP);
  pinMode(oikea, INPUT_PULLUP);
  pinMode(harvest_nappi, INPUT_PULLUP);
  pinMode(vasen_ledi, OUTPUT);
  pinMode(oikea_ledi, OUTPUT);
  pinMode(harvest_ledi, OUTPUT);
  pinMode(piezo, OUTPUT);
  pinMode(magneetti, OUTPUT);

  digitalWrite(enable, HIGH);   //virta pois

 // digitalWrite(magneetti, HIGH);    //magneetti kiinni

  Serial.begin(9600);

}
void hae_raja()
{
  digitalWrite(enable, LOW);      //virtaa moottorille
  digitalWrite(dirPin, HIGH);     //vastapaivaa kohti rajakytkinta

  rajakytkin_v = digitalRead(raja_suppilo);

  while (rajakytkin_v == HIGH)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(100);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(100);
    
    rajakytkin_v = digitalRead(raja_suppilo);
  }
  digitalWrite(enable, HIGH);  //virta pois
  step_laskuri = 0;
  Serial.print(step_laskuri);
  Serial.print(" \n");
}

void kaanna_recolle()
{
  digitalWrite(enable, LOW);      //virtaa moottorille
  digitalWrite(dirPin, LOW);      //myotapaivaa

  while(step_laskuri < rajalta_recolle)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(100);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(100);

    step_laskuri++;

    if(step_laskuri == magneetin_raja)     //KIINNI
    digitalWrite(magneetti, HIGH);
  }
  digitalWrite(magneetti, LOW);        //IRROTETAAN hammerin ollessa recolla
  digitalWrite(enable, HIGH);      //virtaa pois moottorilta
  
  Serial.print(step_laskuri);
  Serial.print(" \n");
}
void kaanna_suppilolle()
{
  digitalWrite(magneetti, HIGH);      //KIINNI
  
  digitalWrite(enable, LOW);      //virtaa moottorille
  digitalWrite(dirPin, HIGH);     //vastapaivaa kohti rajakytkinta

  rajakytkin_v = digitalRead(raja_suppilo);

  if(rajakytkin_v == HIGH)
  {
    while(step_laskuri > 0)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(100);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(100);

    step_laskuri--;

    if(step_laskuri == magneetin_raja)     //IRROTUS
    digitalWrite(magneetti, LOW);
    
    rajakytkin_v = digitalRead(raja_suppilo);
  }
  Serial.print(step_laskuri);
  Serial.print(" \n");
  }
  digitalWrite(enable, HIGH);  //virta pois
  Serial.print(step_laskuri);
  Serial.print(" \n");
}
void hammeri()
{
  digitalWrite(enable, LOW);      //virtaa moottorille
  digitalWrite(dirPin, LOW);      //myotapaivaa

  while(step_laskuri < hammerin_nostokulma)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(100);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(100);

    step_laskuri++;

    if(step_laskuri == magneetin_raja)     //KIINNI
    digitalWrite(magneetti, HIGH);
  }

  
  Serial.print(step_laskuri);
  Serial.print(" \n");
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
void loop() {

  rajakytkin_v = digitalRead(raja_suppilo);
 // rajakytkin_o = digitalRead(raja_reco);
  vas_button = digitalRead(vasen);
  oik_button = digitalRead(oikea);
  harvest_button = digitalRead(harvest_nappi);

  if (rajakytkin_v == LOW)
  {
    step_laskuri = 0;
    digitalWrite(vasen_ledi, HIGH);
  }
  else
    digitalWrite(vasen_ledi, LOW);

//  if (rajakytkin_o == LOW)
  //  digitalWrite(oikea_ledi, HIGH);

//  else
    digitalWrite(oikea_ledi, LOW);

  if (rajakytkin_v == HIGH && vas_button == LOW)  //vasen namiska   KALIBROINTIIN
  {
    for (int x = 0; x < 100; x++)
    {
      digitalWrite(enable, LOW);      //virtaa moottorille
      digitalWrite(dirPin, HIGH);      //vastapaivaa
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(100);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(100);

      if (step_laskuri > 0)
        step_laskuri--;
    }
    digitalWrite(enable, HIGH);     //virta pois moottilta!
  }
  if (rajakytkin_v == HIGH && oik_button == LOW)    //oikea namiska   KALIBROINTIIN
  {
    for (int x = 0; x < 100; x++)
    {
      digitalWrite(enable, LOW);      //virtaa moottorille
      digitalWrite(dirPin, LOW);      //myotapaivaa
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(100);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(100);

      step_laskuri++;
    }
    digitalWrite(enable, HIGH);     //virta pois moottilta!
  }
  //***************************************************************************  ALUSTAVA HARVESTERI -RUMBA
  if (harvest_button == LOW)
  {
    digitalWrite(harvest_ledi, HIGH);
    aanimerkki();
    delay(1000);
    hae_raja();       //SYKLI ALOITETTAVA AINA RAJAN HAKEMISELLA! 
    delay(3000);
    aanimerkki();
    kaanna_recolle();
    delay(10000);
    aanimerkki();
    kaanna_suppilolle();
    delay(1000);
    hammeri();
    delay(1000);
    kaanna_suppilolle();
    digitalWrite(harvest_ledi, LOW);
    digitalWrite(enable, HIGH);     //virta pois moottilta!
  }



  //************************************************************************

  Serial.print(step_laskuri);
  Serial.print(" \n");
  delay(300);
}
