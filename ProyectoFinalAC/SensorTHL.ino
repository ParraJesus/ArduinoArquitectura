/*!
\file   SensorTHL.ino
\date   2023-06-27
\author David Santiago Giron Muñoz <davidgiron@unicauca.edu.co>
        Jeferson Castano Ossa <jcastanoossa@unicauca.edu.co>
        Jesus Gabriel Parra Dugarte <jgparra@unicauca.edu.co>
\brief  Control de medida de los sensores de temperatura, humedad y luz.

\par Copyright
Information contained herein is proprietary to and constitutes valuable
confidential trade secrets of Unicauca, and
is subject to restrictions on use and disclosure.

\par
Copyright (c) Unicauca 2023. All rights reserved.

\par
The copyright notices above do not evidence any actual or
intended publication of this material.
******************************************************************************
*/

/********************************************//**
 *  DHT setup
 ***********************************************/
DHTStable DHT;

#define DEBUG(a) Serial.print(millis()); Serial.print(": "); Serial.println(a);

void readTemperature();
void printData();
void readPhotoRe();


/********************************************//**
 *  Async Task
 ***********************************************/
AsyncTask asyncTaskReadTemperature(2000, true, readTemperature);
AsyncTask asyncTaskReadPhotoR(1000, true, readPhotoRe);

/********************************************//**
 *  Values
 ***********************************************/
int light;
float temper;
float humidity;
bool maxTemperature = false;


/*F**************************************************************************
* NAME: readTHL
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Leer los valores de temperatura, humedad y luz
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void readTHL(){
  readTemperature();
  readPhotoRe();
  asyncTaskReadTemperature.Start();
  asyncTaskReadPhotoR.Start();
}
/*F**************************************************************************
* NAME: readTemperature
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Obtiene los valores de temperatura y humedad
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void readTemperature()
{
  // READ DATA
  int chk = DHT.read11(DHT11_PIN);
  temper = DHT.getTemperature();
  humidity = DHT.getHumidity();
  printData();
  if(temper >= 32.0f){
    maxTemperature = true;
  }else{
    maxTemperature = false;
  }
  printData();
}
/*F**************************************************************************
* NAME: updateTHL
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Actualizar tareas asincronicas 
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void updateTHL(){
  asyncTaskReadTemperature.Update();
  asyncTaskReadPhotoR.Update();
}
/*F**************************************************************************
* NAME: finishTaskSensor
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Terminar tareas asincronicas 
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void finishTaskSensor()
{
  asyncTaskReadTemperature.Stop();
  asyncTaskReadPhotoR.Stop();
}
/*F**************************************************************************
* NAME: readPhotoRe
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Obtiene el valor del fotoresistor
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void readPhotoRe()
{
  light = analogRead(photocellPin);
  printData();
}
/*F**************************************************************************
* NAME: printData
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Imprime los valores de temperatura, humedad y luz
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void printData()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("H:");
  lcd.print(humidity);
  lcd.setCursor(8,0);
  lcd.print(" T:");
  lcd.print(temper);
  lcd.setCursor(0,1);
  lcd.print(" L:");
  lcd.print(light);
} 
bool isTempMax(){
  return maxTemperature;
}
