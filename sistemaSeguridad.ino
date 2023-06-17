/***************************************************
Jesus Gabriel Parra Dugarte
Jeferson Castaño Ossa
David Santiago Giron Muñoz
 ****************************************************/

#include <LiquidCrystal.h>
#include <Keypad.h>

/* Display */
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

/* Keypad setup */
const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 4;
byte rowPins[KEYPAD_ROWS] = {5, 4, 3, 2};
byte colPins[KEYPAD_COLS] = {A3, A2, A1, A0};
char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);

/* LED setup*/

const int redPin = 24; // R petal on RGB LED module connected to digital pin 11 
const int greenPin = 23; // G petal on RGB LED module connected to digital pin 10 
const int bluePin = 22; // B petal on RGB LED module connected to digital pin 9 

/* Access setup */
const String pass = "AAAAAA";
const int passLength = pass.length();
int tries = 3;
String check;
int count = 0;
bool accessGranted = false;
unsigned long time; 

void setup() 
{
  Serial.begin(9600);

  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Bienvenido!");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Contrasenia:");
  lcd.setCursor(0,1);

  pinMode(redPin, OUTPUT); // sets the redPin to be an output 
  pinMode(greenPin, OUTPUT); // sets the greenPin to be an output 
  pinMode(bluePin, OUTPUT); // sets the bluePin to be an output
}

void loop()
{
  readKey();
}

/**
* @brief Funcion de lectura de caracteres de la contrseña.
*/
void readKey()
{ 
  if(tries > 0 && !accessGranted)
  {
    char key = keypad.getKey();
    if(millis()-time > 10000 && count != 0){ 
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Tiempo agotado.");
      delay(2000);
      checkPass(pass, check);
      count = 0;
      check = "";
      time = millis();
  }
    if (key && count < passLength)
    {
      lcd.print('*');
      check += key;
      time = millis();
      count++;
    }
    if(count == passLength)
    {
      accessGranted = checkPass(pass, check);
      count = 0;
      check = "";
      time = millis();
    }
  }
}

/**
* @brief Funcion de verificacion de la contrseña ingresada.
* @param pass Contraseña ingresada
* @param check Contraseña de ingreso al sistema
* @return Verdadero si la contraseña correcta, False si es incorrecta
*/
bool checkPass(String pass, String check)
{
  if(pass == check)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Correcto :D");
    color(0, 255, 0);
    return true;
  }
  if(pass != check)
  {
    tries--;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Incorrecto");
    delay(1000);
    lcd.clear();
    lcd.print("Quedan ");
    lcd.print(tries);
    lcd.setCursor(0,1);
    lcd.print("intentos.");
    delay(1000);
    lcd.clear();
    lcd.print("Contrasenia:");
    lcd.setCursor(0,1);
    color(255, 102, 0);
  }
  if(tries <= 0)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Sistema");
    lcd.setCursor(0,1);
    lcd.print("bloqueado.");
    color(255, 0, 0); 
  }
  Serial.println("Cadena ingresada: " + check);
  return false;
}

/**
 * @brief Enciende el LED RGB con un color específico.
 * @param red Valor de brillo para el color rojo (0-255).
 * @param green Valor de brillo para el color verde (0-255).
 * @param blue Valor de brillo para el color azul (0-255).
 */
void color (unsigned char red, unsigned char green, unsigned char blue)  
{ 
  analogWrite(redPin, red); 
  analogWrite(bluePin, blue); 
  analogWrite(greenPin, green); 
}