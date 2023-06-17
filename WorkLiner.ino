#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>

#define ENC_A 2        // Пін енкодера
#define ENC_B 3        // Пін енкодера
#define ENC_TYPE 1     // Тип енкодера, 0 або 1

LiquidCrystal_I2C lcd(0x27, 16, 2); // Адреса 0x27 або 0x3f
Encoder myEncoder(ENC_A, ENC_B);  // Define the encoder pins

volatile long encCounter;
String s1;
//float met1 = 0.0001953125;
float met1 = 0.0001953125;

float met;
volatile boolean state0, lastState, turnFlag;

boolean butt1_flag = 0;
boolean butt2_flag = 0;
boolean butt3_flag = 0;

unsigned long last_time = 0; // Оголошення змінної last_time

void int0() {
  state0 = digitalRead(ENC_A);
  if (state0 != lastState) {
#if (ENC_TYPE == 1)
    turnFlag = !turnFlag;
    if (turnFlag)
      encCounter += (digitalRead(ENC_B) != state0) ? 1 : -1;
#else
    encCounter += (digitalRead(ENC_B) != state0) ? 1 : -1;
#endif
    lastState = state0;
  }
}

void setup() {
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(ENC_A), int0, CHANGE);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);

  String myString = " meters             ";

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(myString);
}

void loop() {
  boolean but1 = !digitalRead(5);
  boolean but2 = !digitalRead(4);
  boolean but3 = !digitalRead(6); // Зчитувати кнопку з контактом, який замкнутий при натисканні

  met = met1 * encCounter;

  if (millis() - last_time > 100) {
    last_time = millis();
    String myString = " -> meters     ";

    lcd.setCursor(0, 0);
    lcd.print(met, 2);
    lcd.print(myString);
    Serial.println(met, 8);
    Serial.println(encCounter);
  }

  if (but1 == 1 && butt1_flag == 0) {
    butt1_flag = 1;
    encCounter += 512;
    lcd.setCursor(0, 0);
    lcd.print(met, 2);
    lcd.print(" -> meters");
  }

  if (but1 == 0 && butt1_flag == 1) {
    butt1_flag = 0;
  }

  if (but3 == 1 && butt3_flag == 0) { // Зміна умови перевірки для кнопки обнулення
    butt3_flag = 1;
    encCounter = 0;
    lcd.setCursor(0, 0);
    lcd.print(met, 2);
    lcd.print(" -> meters");
  }

  if (but3 == 0 && butt3_flag == 1) {
    butt3_flag = 0;
  }

  if (but2 == 1 && butt2_flag == 0) {
    butt2_flag = 1;
    encCounter -= 512;
    lcd.setCursor(0, 0);
    lcd.print(met, 2);
    lcd.setCursor(10, 0);
    lcd.print(" -> meters");
  }

  if (but2 == 0 && butt2_flag == 1) {
    butt2_flag = 0;
  }
}
