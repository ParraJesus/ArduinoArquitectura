/*!
\file   buzzAlarm.ino
\date   2023-06-27
\author David Santiago Giron Muñoz <davidgiron@unicauca.edu.co>
        Jeferson Castano Ossa <jcastanoossa@unicauca.edu.co>
        Jesus Gabriel Parra Dugarte <jgparra@unicauca.edu.co>
\brief  Manejo del buzzer (Alarma)

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

/*F**************************************************************************
* NAME: startBuzzer
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Activar el buzzer y proporcionar una advertencia visual y audible al usuario 
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/

void startBuzzer() {
  pinMode(BUZZER_PASIVO, OUTPUT); // pin 8 como salida
  tone(BUZZER_PASIVO, 100, 5000);
  lcd.clear();
  lcd.createChar(0, charPeligro);
  lcd.home();
  lcd.write(byte(0));
  lcd.setCursor(2,0);
  lcd.print("Peligro...");
  lcd.setCursor(0,1);
  lcd.print("Temperatura max");
}
