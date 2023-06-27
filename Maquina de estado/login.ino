/*!
\file   login.ino
\date   2023-06-27
\author David Santiago Giron Muñoz <davidgiron@unicauca.edu.co>
        Jeferson Castano Ossa <jcastanoossa@unicauca.edu.co>
        Jesus Gabriel Parra Dugarte <jgparra@unicauca.edu.co>
\brief  Control de ingreso al sistema (Lectura de contrasena).

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
/* Access setup */
const String pass = "AAAAAA";
const int passLength = pass.length();
int tries = 3;
String check;
int count = 0;
bool accessGranted = false;
bool accessBlocked = false;
unsigned long time;

/***************************************************************************
* NAME: startLogin
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Inicializa la pantalla de cristal líquido, la luz led y las variables
* necesarias para el registro al sistema
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void startLogin() {

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Bienvenido!");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Contrasenia:");
  lcd.setCursor(0, 1);

  tries = 3;
  check = "";
  count = 0;
  accessGranted = false;
  accessBlocked = false;
  time = 0;

  pinMode(redPin, OUTPUT);    // sets the redPin to be an output
  pinMode(greenPin, OUTPUT);  // sets the greenPin to be an output
  pinMode(bluePin, OUTPUT);   // sets the bluePin to be an output
  readPass();
}

/***************************************************************************
* NAME: readPass
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Lee y almacena los caracteres de la contraseña
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void readPass() {
  while (tries > 0 && !accessGranted) {
    char key = keypad.getKey();
    if (millis() - time > 10000 && count != 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Tiempo agotado.");
      delay(2000);
      checkPass(pass, check);
      count = 0;
      check = "";
      time = millis();
    }
    if (key && count < passLength) {
      lcd.print('*');
      check += key;
      time = millis();
      count++;
    }
    if (count == passLength) {
      accessGranted = checkPass(pass, check);
      count = 0;
      check = "";
      time = millis();
    }
  }
}

/***************************************************************************
* NAME: checkPass
*----------------------------------------------------------------------------
* PARAMS:   
*         pass: valor de la contraseña guardada en el sistema
*         check: contraseña ingresada por el usuario
*         
* return:   true: la contraseña ingresada es igual a la contraseña guardada
*           false: la contraseña ingresada es diferente a la contraseña guardada
*           y  reduce el número de intentos totales.
*----------------------------------------------------------------------------
* PURPOSE:
* Compara la contraseña ingresada por el usuario 
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
bool checkPass(String pass, String check) {
  if (pass == check) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Correcto :D");
    color(0, 255, 0);
    delay(2000);
    return true;
  }
  if (pass != check) {
    tries--;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Incorrecto");
    delay(1000);
    lcd.clear();
    lcd.print("Quedan ");
    lcd.print(tries);
    lcd.setCursor(0, 1);
    lcd.print("intentos.");
    delay(1000);
    lcd.clear();
    lcd.print("Contrasenia:");
    lcd.setCursor(0, 1);
    color(255, 102, 0);
  }
  if (tries <= 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sistema");
    lcd.setCursor(0, 1);
    lcd.print("bloqueado.");
    color(255, 0, 0);
    accessBlocked = true;
  }
  Serial.println("Cadena ingresada: " + check);
  return false;
}

 /***************************************************************************
* NAME: color
*----------------------------------------------------------------------------
* PARAMS:   
*         red: Valor de brillo para el color rojo (0-255).
*         green: Valor de brillo para el color verde (0-255).
*         blue: Valor de brillo para el color azul (0-255).
*         
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Enciende el LED RGB con un color específico. 
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void color(unsigned char red, unsigned char green, unsigned char blue) {
  analogWrite(redPin, red);
  analogWrite(bluePin, blue);
  analogWrite(greenPin, green);
}


/***************************************************************************
* NAME: getAccess
*----------------------------------------------------------------------------
* PARAMS:   none
*         
* return:   accessGranted: describe si el acceso al sistema es válido o no
*----------------------------------------------------------------------------
* PURPOSE:
* Recupera el valor de la variable accessGranted
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
bool getAccess() {
  return accessGranted;
}

/***************************************************************************
* NAME: getBlock
*----------------------------------------------------------------------------
* PARAMS:   none
*         
* return:   accessBlocked: describe si el acceso al sistema está bloqueado
*----------------------------------------------------------------------------
* PURPOSE:
* Recupera el valor de la variable accessBlocked
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
bool getBlock() {
  return accessBlocked;
}
