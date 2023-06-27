/*!
\file   SensorPV.ino
\date   2023-06-27
\author David Santiago Giron Muñoz <davidgiron@unicauca.edu.co>
        Jeferson Castano Ossa <jcastanoossa@unicauca.edu.co>
        Jesus Gabriel Parra Dugarte <jgparra@unicauca.edu.co>
\brief  Control de los sensores de puertas y ventanas.

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
bool resetSys = false;
volatile int eventDetected = LOW;

/*F**************************************************************************
* NAME: setupPV
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Configurar pines e interrupcciones para los sensores de puertas y ventanas
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void setupPV()
{
  pinMode(hallPin,INPUT_PULLUP); //initialize the hall as an input
  pinMode(trackingPin,INPUT_PULLUP); //initialize the tracking as an input
  pinMode(metalTouchPin,INPUT_PULLUP); //initialize the metalTouch as an input
  activateInts();
  resetSys = false;
  eventDetected = LOW;
}
/*F**************************************************************************
* NAME: readLeaving
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Identificar caracter de salida
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void readLeaving()
{
    char key = keypad.getKey();
    if(key =='*') {
    resetSys = true;
  }
}
/*F**************************************************************************
* NAME: resetPV
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Reiniciar deteccion de puertas y ventanas
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void resetPV()
{
  eventDetected = LOW;
}
/*F**************************************************************************
* NAME: activateInts
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Activar las interrupciones para la deteccion de puertas y ventanas
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void activateInts()
{
  attachInterrupt(digitalPinToInterrupt(hallPin), setDetection, FALLING);
  attachInterrupt(digitalPinToInterrupt(trackingPin), setDetection, FALLING);
  attachInterrupt(digitalPinToInterrupt(metalTouchPin), setDetection, RISING);
  eventDetected = LOW;
}
/*F**************************************************************************
* NAME: setDetection
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Activar evento
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void setDetection()
{
  eventDetected = HIGH;
}
/*F**************************************************************************
* NAME: getReset
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   Verdadero: Reiniciar sistema | Falso: No reiniciar sistema
*----------------------------------------------------------------------------
* PURPOSE:
* Identificar si se debe reiniciar el sistema
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
bool getReset()
{
  return resetSys;
}
/*F**************************************************************************
* NAME: getEventDetected
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   Verdadero: Evento detectado | Falso: Evento no detectado
*----------------------------------------------------------------------------
* PURPOSE:
* Identificar si ha ocurrido un evento
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
bool getEventDetected()
{
  return eventDetected==HIGH;
}