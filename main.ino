
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <NewPing.h>

// Configuración del LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Configuración del teclado matricial
const byte ROWS = 4; // Cuatro filas
const byte COLS = 4; // Cuatro columnas
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; // Pines de conexión de las filas
byte colPins[COLS] = {5, 4, 3, 2}; // Pines de conexión de las columnas

const int TRIG_PIN = 10;
const int ECHO_PIN = 11;
NewPing sonar(TRIG_PIN, ECHO_PIN, 18);  // Máximo rango de distancia ajustado a 18 cm

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(9600);          // Start serial communication at 9600 baud
  lcd.init();                  // Initialize the LCD
  lcd.backlight();             // Turn on the backlight
  lcd.setCursor(0, 0);
  lcd.print("Welcome!");
}

// // Function to calculate specific fill percentages based on given distances
// int calculateFillPercentage(float cm) {
//   if (cm >= 14.0) return 0;
//   if (cm >= 13) return 0;
//   if (cm >= 12) return 20;
//   if (cm >= 11) return 20;
//   if (cm >= 10) return 20;
//   if (cm >= 9) return 40;
//   if (cm >= 8) return 40;
//   if (cm >= 7) return 60;
//   if (cm >= 6) return 60;
//   if (cm >= 5) return 60;
//   if (cm >= 4) return 80;
//   if (cm >= 3) return 80;
//   if (cm <= 2) return 100;
//     // Assumes any distance less than 2 cm is also 100%
// }

int calculateFillPercentage(float cm) {
  if (cm > 11.6) return 0;    // No fill for distances greater than 11.6 cm
  else if (cm > 9.0) return 20; // 20% fill between 9.0 cm and 11.6 cm
  else if (cm > 5.7) return 40; // 40% fill between 5.7 cm and 9.0 cm
  else if (cm > 3.8) return 60; // 60% fill between 3.7 cm and 5.7 cm
  else if (cm > 2.8) return 80; // 80% fill between 2.4 cm and 3.7 cm
  else return 100;            // 100% fill at 2.4 cm and below
}

// int calculateFillPercentage(float cm) {
//     if (cm > 15) return 0;  // No fill beyond 15 cm
//     if (cm < 2) return 100; // Full fill at 2 cm and below

//     // Linear interpolation between 2 cm (100% fill) and 15 cm (0% fill)
//     return (int)(-7.69 * cm + 115.38);  // y = mx + b with m = -7.69 and b = 115.38
// }

void loop() {
    delay(1000);  // Increase delay to allow time for recording measurements


  unsigned int uS = sonar.ping();  // Perform the measurement
  float cm = (float)uS / US_ROUNDTRIP_CM;  // Make sure US_ROUNDTRIP_CM is a float
  cm = round(cm * 10.0) / 10.0;  // Round to the nearest tenth

  int fillPercentage = calculateFillPercentage(cm);  // Calculate fill percentage

  // Display the current distance and fill percentage
  lcd.setCursor(0, 0);
  lcd.print("Dist: ");
  lcd.print(cm, 1);  // Show one decimal place for more precise distance display
  lcd.print(" cm    ");

  lcd.setCursor(0, 1);
  lcd.print("Fill: ");
  lcd.print(fillPercentage);
  lcd.print("%    ");

   // Log the distance
  Serial.print("Measured Distance: ");
  Serial.print(cm);
  Serial.println(" cm");
}

