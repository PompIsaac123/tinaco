#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <NewPing.h>

// Configuration of the LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Configuration of the matrix keypad
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

// Configuration of the ultrasonic sensor
const int TRIG_PIN = 10;
const int ECHO_PIN = 11;
NewPing sonar(TRIG_PIN, ECHO_PIN, 18);

// Configuration of the pump
const int PUMP_PIN = 12;  // Assign the pump control to pin 12

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

int fillPercentageThreshold = 20;  // Default fill percentage threshold for the pump

void setup() {
  pinMode(PUMP_PIN, OUTPUT);
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Welcome!");
}

void loop() {
  const int numReadings = 10;
  float total = 0;
  float average = 0;

  for (int i = 0; i < numReadings; i++) {
    delay(50);
    unsigned int uS = sonar.ping();
    float cm = (float)uS / US_ROUNDTRIP_CM;
    total += cm;
  }

  average = total / numReadings;
  average = round(average * 10.0) / 10.0;

  int fillPercentage = calculateFillPercentage(average);

  lcd.setCursor(0, 0);
  lcd.print("Setpoint: ");
  lcd.print(fillPercentageThreshold);
  lcd.print("%     ");

  lcd.setCursor(0, 1);
  lcd.print("Fill: ");
  lcd.print(fillPercentage);
  lcd.print("%    ");

  // Keypad handling
  char key = keypad.getKey();
  if (key && isdigit(key)) {  // Only consider numeric keys
    int newThreshold = (key - '0') * 10;  // Assuming single digit followed by 0 (e.g., pressing '4' sets 40%)
    
    if(newThreshold == 10){
      newThreshold = 100;
    } 
      
    if (newThreshold == 20 || newThreshold == 40 || newThreshold == 60 || newThreshold == 80 || newThreshold == 100) {
      fillPercentageThreshold = newThreshold;
      lcd.setCursor(0, 0);
      lcd.print("Setpoint: ");
      lcd.print(fillPercentageThreshold);
      lcd.print("%     ");
      Serial.print("New Setpoint: ");
      Serial.println(fillPercentageThreshold);
    } else {
      Serial.println("Invalid setpoint. Try 20, 40, 60, 80, 100.");
      Serial.println(newThreshold);
    }
  }

  // Pump control based on user-set threshold
  if (fillPercentage < fillPercentageThreshold) {
    digitalWrite(PUMP_PIN, HIGH);  // Activate the pump
    Serial.println("Pump Activated");
  } else {
    digitalWrite(PUMP_PIN, LOW);  // Deactivate the pump
    Serial.println("Pump Deactivated");
  }

  Serial.print("Measured Fill: ");
  Serial.println(fillPercentage);
}

int calculateFillPercentage(float cm) {
  if (cm >= 14.0) return 0;   // No fill for 14.0 cm and above
  if (cm >= 13.0) return 0;   // Still no fill for 13.0 cm to less than 14.0 cm

  if (cm >= 12.0) return 20;  // 20% fill for 12.0 cm to less than 13.0 cm
  if (cm >= 11.0) return 20;  // Still 20% fill for 11.0 cm to less than 12.0 cm
  if (cm >= 10.0) return 20;  // Still 20% fill for 10.0 cm to less than 11.0 cm

  if (cm >= 9.0) return 40;   // 40% fill for 9.0 cm to less than 10.0 cm
  if (cm >= 8.0) return 40;   // Still 40% fill for 8.0 cm to less than 9.0 cm

  if (cm >= 7.0) return 60;   // 60% fill for 7.0 cm to less than 8.0 cm
  if (cm >= 6.0) return 60;   // Still 60% fill for 6.0 cm to less than 7.0 cm
  if (cm >= 5.0) return 60;   // Still 60% fill for 5.0 cm to less than 6.0 cm

  if (cm >= 4.0) return 80;   // 80% fill for 4.0 cm to less than 5.0 cm
  if (cm >= 3.0) return 80;   // Still 80% fill for 3.0 cm to less than 4.0 cm

  return 100;                 // 100% fill for less than 3.0 cm
}
