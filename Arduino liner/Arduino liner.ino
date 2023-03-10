#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>

// Енкодер колеса
#define ENCODER_PIN_A 2
#define ENCODER_PIN_B 3

volatile int encoderPos = 0;
volatile bool encoderDir = true;

void handleEncoderInterruptA() {
  if (digitalRead(ENCODER_PIN_A) == digitalRead(ENCODER_PIN_B)) {
    encoderPos++;
    encoderDir = true;
  } else {
    encoderPos--;
    encoderDir = false;
  }
}

// Енкодер кнопки
#define BUTTON_ENCODER_PIN_A 4
#define BUTTON_ENCODER_PIN_B 5
#define BUTTON_ENCODER_PIN_BTN 6

volatile int buttonEncoderPos = 0;
volatile bool buttonEncoderDir = true;

void handleButtonEncoderInterruptA() {
  if (digitalRead(BUTTON_ENCODER_PIN_A) == digitalRead(BUTTON_ENCODER_PIN_B)) {
    buttonEncoderPos++;
    buttonEncoderDir = true;
  } else {
    buttonEncoderPos--;
    buttonEncoderDir = false;
  }
}
// Дисплей
LiquidCrystal_I2C lcd(0x27, 16, 2);

void updateDisplay(float length) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Length: ");
  lcd.print(length, 2);
  lcd.print("m");
}


// Розрахунок довжини
float calculateLength() {
  float encoderDistPerCount = 0.01953125; // 1024 спрацювань на 1 повний оберт колеса енкодера при довжині кола 20 сантиметрів (0.2 метра)
 
  if (!encoderDir) {
    encoderDistPerCount *= -1;
  }

  float length = encoderPos * encoderDistPerCount;

  return length;
}

// Обробка натискання кнопки енкодера
void handleButtonEncoderPress() {
  buttonEncoderPos = 0;
}

// Розрахунок відстані, яку необхідно додати / відняти при натисканні кнопки енкодера
float calculateButtonEncoderDist() {
  float buttonEncoderDistPerCount = 0.41666667; // 24 спрацювання на 1 повний оберт енкодера при 10 сантиметрах дистанції

  if (!buttonEncoderDir) {
    buttonEncoderDistPerCount *= -1;
  }

  float dist = buttonEncoderPos * buttonEncoderDistPerCount;

  return dist;
}

void setup() {
  pinMode(ENCODER_PIN_A, INPUT);
  pinMode(ENCODER_PIN_B, INPUT);
  pinMode(BUTTON_ENCODER_PIN_A, INPUT);
  pinMode(BUTTON_ENCODER_PIN_B, INPUT);
  pinMode(BUTTON_ENCODER_PIN_BTN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), handleEncoderInterruptA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BUTTON_ENCODER_PIN_A), handleButtonEncoderInterruptA, CHANGE);
}

void loop() {
  float length = calculateLength();
  updateDisplay(length);

  if (digitalRead(BUTTON_ENCODER_PIN_BTN) == LOW) {
    handleButtonEncoderPress();
    delay(10);
  }

  float buttonEncoderDist = calculateButtonEncoderDist();

  if (buttonEncoderDist != 0) {
    encoderPos = 0;
    float newLength = length + buttonEncoderDist;
    updateDisplay(newLength);
    while (digitalRead(BUTTON_ENCODER_PIN_BTN) == HIGH) {
      delay(10);
    }
  }
}

