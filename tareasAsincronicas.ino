/***************************************************
David Santiago Giron Muñoz
Jesus Gabriel Parra Dugarte
Jeferson Castaño Ossa
 ****************************************************/

#include "AsyncTaskLib.h"
#include "DHTStable.h"
#include <LiquidCrystal.h>

/* DHT setup*/
DHTStable DHT;
#define DHT11_PIN 18

#define DEBUG(a) Serial.print(millis()); Serial.print(": "); Serial.println(a);

/* Async Task*/
AsyncTask asyncTaskReadTemperature(2000, true, readTemperature);
AsyncTask asyncTaskReadPhotoR(4000, true, readPhotoRe);

/* Display */
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

/* Values  */
int outLight;
float temp;
float hum;

const int photocellPin = A1;
const int ledPin = 13;

void printData();
void readTemperature();
void readPhotoRe();

void setup()
{
	Serial.begin(115200);
  lcd.begin(16, 2);
  
	asyncTaskReadTemperature.Start();
  asyncTaskReadPhotoR.Start();
}

void loop()
{
	asyncTaskReadTemperature.Update();
  asyncTaskReadPhotoR.Update();
}
/**
* @brief Obtiene los valores de temperatura y humedad
*/
void readTemperature()
{
  // READ DATA
  int chk = DHT.read11(DHT11_PIN);
  temp = DHT.getTemperature();
  hum = DHT.getHumidity();
  printData();
}

/**
* @brief Obtiene el valor del fotoresistor
*/
void readPhotoRe()
{
  outLight = analogRead(photocellPin);
  printData();
}
/**
* @brief Imprime los valores de temperatura, humedad y luz
*/
void printData()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("H:");
  lcd.print(hum);
  lcd.setCursor(8,0);
  lcd.print(" T:");
  lcd.print(temp);
  lcd.setCursor(0,1);
  lcd.print(" L:");
  lcd.print(outLight);
} 