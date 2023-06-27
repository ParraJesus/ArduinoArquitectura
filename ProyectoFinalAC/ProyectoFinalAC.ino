/*!
\file   sketch.ino
\date   2023-06-27
\author David Santiago Giron Muñoz <davidgiron@unicauca.edu.co>
        Jeferson Castano Ossa <jcastanoossa@unicauca.edu.co>
        Jesus Gabriel Parra Dugarte <jgparra@unicauca.edu.co>
\brief  Control de la maquina de estados y ejecucion principal.

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
#include "StateMachineLib.h"
#include <LiquidCrystal.h>
#include <Keypad.h>
#include "pin.h"
#include "AsyncTaskLib.h"
#include "DHTStable.h"
 
/********************************************//**
 *  Configuracion del LCD
 ***********************************************/
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

byte charPeligro[] = {
  B00000,
  B00100,
  B00100,
  B01110,
  B01110,
  B11111,
  B11111,
  B00000
};

/********************************************//**
 *  Configuracion del teclado
 ***********************************************/
const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 4;
byte rowPins[KEYPAD_ROWS] = {41, 43, 45, 47};
byte colPins[KEYPAD_COLS] = {A3, A2, A1, A0};
char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);


/** An enum type. 
 *  Definir los esatados de la maquina de estados
 */
enum State
{
	login = 0,
	mTHL = 1,
	alarm = 2,
	blocked = 3,
  mPV = 4,
};

/** An enum type. 
 *  Definir las entradas de la maquina de estados
 */
enum Input
{
	reset = 0,
  passed = 1,
	block = 2,
	timeout = 3,
  temperature = 4,
  event = 5,
  unknown = 6
};

// Crear maquina de estados
StateMachine stateMachine(5, 10);

// Almacena la ultima entrada
Input input;

// Conteo para bloquear sistema por temperatura maxima
int blockSystem = 0;

/*F**************************************************************************
* NAME: setupStateMachine
*----------------------------------------------------------------------------
* PARAMS:
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Configurar maquina de estados con transiciones y acciones de entrada y salida
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void setupStateMachine()
{
	// Add transitions
	stateMachine.AddTransition(login, mTHL, []() { return input == passed; });
  stateMachine.AddTransition(login, blocked, []() { return input == block; });

	stateMachine.AddTransition(mTHL, alarm, []() { return input == temperature; });
	stateMachine.AddTransition(mTHL, mPV, []() { return input == timeout; });

	stateMachine.AddTransition(alarm, mTHL, []() { return input == timeout; });
  stateMachine.AddTransition(alarm, blocked, []() { return input == temperature; });

	stateMachine.AddTransition(mPV, login, []() { return input == reset; });
  stateMachine.AddTransition(mPV, mTHL, []() { return input == timeout; });
	stateMachine.AddTransition(mPV, blocked, []() { return input == event; });

	stateMachine.AddTransition(blocked, mPV, []() { return input == timeout; });

	// Add actions

	stateMachine.SetOnEntering(login, inputLogin);
	stateMachine.SetOnEntering(mTHL, inputMTHL);
	stateMachine.SetOnEntering(alarm, inputAlarm);
	stateMachine.SetOnEntering(blocked, inputBlocked);
  stateMachine.SetOnEntering(mPV, inputMPV);


	stateMachine.SetOnLeaving(login, outPutLogin);
	stateMachine.SetOnLeaving(mTHL, []() {Serial.println("Leaving mTHL"); });
	stateMachine.SetOnLeaving(alarm, []() {Serial.println("Leaving Alarm"); });
	stateMachine.SetOnLeaving(blocked, outPutBlocked);
  stateMachine.SetOnLeaving(mPV, []() {Serial.println("Leaving mPV"); });
}

/********************************************//**
 *  Tareas asincronicas para tiempos de espera
 ***********************************************/
AsyncTask aTReadPV(2500, false, changeInpTHLtoPV);
AsyncTask aTReadTHL(1500, false, changeInpPVtoTHL);
AsyncTask aTMovePV(20000, false, changeInpBlocktoPV);
AsyncTask aTCheckTemp(5000, false, changeInpAltoTHL);
AsyncTask aTBlockAlarm(20000, false, changeInpAltoBlock);


void setup() 
{
	Serial.begin(9600);
	Serial.println("Starting State Machine...");
	setupStateMachine();	
	Serial.println("Start Machine Started");

	// Initial state
	stateMachine.SetState(login, false, true);
}

void loop() 
{
	// Read user input
	input = static_cast<Input>(readInput());
	// Update State Machine
	stateMachine.Update();
  readLeaving();
}

/*F**************************************************************************
* NAME: readInput
*----------------------------------------------------------------------------
* PARAMS:
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Actualiza la entrada segun el estado actual de la maquina
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
int readInput()
{
  int currentState = stateMachine.GetState();
  switch (currentState)
  {
    case login: return stateLogin(); break;
    case mTHL: return stateMTHL(); break;
    case alarm: return stateAlarm();  break;
    case blocked: return stateBlocked();  break;
    case mPV: return stateMPV();  break;
    default: Serial.println("state Unknown");   break;
  }
  return 6;
}
/*F**************************************************************************
* NAME: stateLogin
*----------------------------------------------------------------------------
* PARAMS:
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Determina y actualiza entrada en el estado de Login
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
int stateLogin(){
  if(getAccess()){
    return Input::passed;
  }
  if(getBlock()){
    return Input::block;
  }
  else{
    return Input::unknown;
  }
}
/*F**************************************************************************
* NAME: stateMTHL
*----------------------------------------------------------------------------
* PARAMS:
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Determina y actualiza entrada en el estado de MTHL
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
int stateMTHL(){
  aTReadPV.Update();
  aTBlockAlarm.Update();
  updateTHL();
  if(isTempMax()){
    aTReadPV.Stop();
    finishTaskSensor();
    return Input::temperature;
  }
  else{
    aTBlockAlarm.Stop();
    return Input::unknown;
  }
}
/*F**************************************************************************
* NAME: stateAlarm
*----------------------------------------------------------------------------
* PARAMS:
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Determina y actualiza entrada en el estado de Alarma
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
int stateAlarm(){
  aTCheckTemp.Update();
  aTBlockAlarm.Update();
  return Input::unknown;
}
/*F**************************************************************************
* NAME: stateBlocked
*----------------------------------------------------------------------------
* PARAMS:
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Determina y actualiza entrada en el estado de bloqueo
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
int stateBlocked(){
  aTMovePV.Update();
  return Input::unknown;
}
/*F**************************************************************************
* NAME: stateMPV
*----------------------------------------------------------------------------
* PARAMS:
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Determina y actualiza entrada en el estado de mPV
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
int stateMPV()
{
  aTReadTHL.Update();
  if(getReset()){
    aTReadTHL.Stop();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Saliendo del");
    lcd.setCursor(7,1);
    lcd.print("sistema...");
    delay(2000);
    return Input::reset;
  }
  if(getEventDetected()){
    aTReadTHL.Stop();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Movimiento");
    lcd.setCursor(7,1);
    lcd.print("Detectado.");
    delay(2000);
    return Input::event;
  }
  else{
    return Input::unknown;
  }
}
/*F**************************************************************************
* NAME: inputLogin
*----------------------------------------------------------------------------
* PARAMS:
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Accion de entrada en el estado de Login
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void inputLogin(){
  Serial.println("state login");
  startLogin();
}
/*F**************************************************************************
* NAME: inputMTHL
*----------------------------------------------------------------------------
* PARAMS:
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Accion de entrada en el estado de mTHL
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void inputMTHL(){
  Serial.println("state mTHL");
  readTHL();
  aTReadPV.Start();
}
/*F**************************************************************************
* NAME: inputAlarm
*----------------------------------------------------------------------------
* PARAMS:
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Accion de entrada en el estado de Alarm
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void inputAlarm(){
  Serial.println("state alarm");
  aTCheckTemp.Start();
  if (blockSystem == 0){
      aTBlockAlarm.Start();
  }
  startBuzzer();
  blockSystem++;
}
/*F**************************************************************************
* NAME: inputBlocked
*----------------------------------------------------------------------------
* PARAMS:
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Accion de entrada en el estado de Bloqueo
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void inputBlocked(){
  Serial.println("state blocked");
  aTMovePV.Start();
  resetPV();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Sistema");
  lcd.setCursor(7,1);
  lcd.print("Bloqueado");
}
/*F**************************************************************************
* NAME: inputMPV
*----------------------------------------------------------------------------
* PARAMS:
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Accion de entrada en el estado de mPV
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void inputMPV(){
  Serial.println("state mPV");
  aTReadTHL.Start();
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("No se detecta");
  lcd.setCursor(3,1);
  lcd.print("Movimiento");
}
/*F**************************************************************************
* NAME: outPutLogin
*----------------------------------------------------------------------------
* PARAMS:
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Accion de salida en el estado de Login
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void outPutLogin(){
  Serial.println("Leaving Login");
  setupPV();
}
/*F**************************************************************************
* NAME: outPutBlocked
*----------------------------------------------------------------------------
* PARAMS:
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Accion de salida en el estado de Bloqueo
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void outPutBlocked(){
  Serial.println("Leaving Blocked");
  resetPV();
}
/*F**************************************************************************
* NAME: changeInpTHLtoPV
*----------------------------------------------------------------------------
* PARAMS:
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Cambiar del estado mTHL a mPV
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void changeInpTHLtoPV(){
  blockSystem = 0;
  input = static_cast<Input>(Input::timeout);
  finishTaskSensor();
  stateMachine.Update();
}
/*F**************************************************************************
* NAME: changeInpPVtoTHL
*----------------------------------------------------------------------------
* PARAMS:
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Cambiar del estado mPV a mTHL
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void changeInpPVtoTHL(){
  input = static_cast<Input>(Input::timeout);
  stateMachine.Update();
}
/*F**************************************************************************
* NAME: changeInpBlocktoPV
*----------------------------------------------------------------------------
* PARAMS:
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Cambiar del estado de Bloqueo a mPV
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void changeInpBlocktoPV(){
  input = static_cast<Input>(Input::timeout);
  stateMachine.Update();
}
/*F**************************************************************************
* NAME: verifyMaxTemp
*----------------------------------------------------------------------------
* PARAMS:
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Verificar cambio de temperatura maxima y apagar alarma
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void changeInpAltoTHL(){
    input = static_cast<Input>(Input::timeout);
    stateMachine.Update();

}
/*F**************************************************************************
* NAME: changeInpAltoBlock
*----------------------------------------------------------------------------
* PARAMS:
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Cambiar del estado Alarma al de Bloqueo
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void changeInpAltoBlock(){
  blockSystem = 0;
  input = static_cast<Input>(Input::temperature);
  stateMachine.Update();
}