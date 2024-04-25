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

int setPointDistance = 0;  // Distance corresponding to setpoint
bool settingMode = false;  // Flag to check if setting setpoint

void setup() {
  Serial.begin(9600);          // Start serial communication at 9600 baud
  lcd.init();                  // Initialize the LCD
  lcd.backlight();             // Turn on the backlight
  lcd.setCursor(0, 0);
  lcd.print("Set Point:");
}

// Map percentage setpoint to distance
int mapSetpointToDistance(int percent) {
  return 18 - (percent / 100.0) * 18;  // Converts percentage to distance within 0 to 18 cm range
}

void loop() {
  delay(50);  // Small delay between measurements

  char key = keypad.getKey(); // Get the key pressed
  static char firstDigit = ' ';  // To store the first digit
  static int setPointPercent = 0;  // Store setpoint percentage

  if (key) {
    Serial.print("Key Pressed: ");
    Serial.println(key);
    if (isdigit(key) && !settingMode) {
      if (firstDigit == ' ') {
        firstDigit = key;
        Serial.print("First digit stored: ");
        Serial.println(firstDigit);
      } else {
        setPointPercent = (firstDigit - '0') * 10 + (key - '0');
        if (setPointPercent == 20 || setPointPercent == 40 || setPointPercent == 60 || setPointPercent == 80 || setPointPercent == 100) {
          setPointDistance = mapSetpointToDistance(setPointPercent);
          settingMode = true;
          Serial.print("Setpoint calculated: ");
          Serial.println(setPointDistance);
        } else {
          Serial.println("Invalid setpoint. Try 20, 40, 60, 80, 100.");
          firstDigit = ' ';  // Reset first digit
        }
      }
    } else if (key == 'A' && settingMode) {
      Serial.print("Setpoint confirmed: ");
      Serial.println(setPointPercent);
      lcd.setCursor(0, 1);
      lcd.print("SetPt: ");
      lcd.print(setPointPercent);
      lcd.print("%     ");
      settingMode = false;
      firstDigit = ' ';  // Reset first digit
    }
  }

  if (!settingMode) {
    unsigned int uS = sonar.ping();  // Perform the measurement
    unsigned int cm = uS / US_ROUNDTRIP_CM;  // Convert to centimeters

    // Display the current distance
    lcd.setCursor(0, 0);
    lcd.print("Dist: ");
    lcd.print(cm);
    lcd.print(" cm      ");

    // Check if the setpoint is reached within a tolerance
    if (cm >= setPointDistance - 1 && cm <= setPointDistance + 1) {
      lcd.setCursor(0, 1);
      lcd.print("SetPt Reached!");
      Serial.println("Setpoint reached!");
    } else {
      lcd.setCursor(0, 1);
      lcd.print("Monitoring...  ");
    }
  }
}
