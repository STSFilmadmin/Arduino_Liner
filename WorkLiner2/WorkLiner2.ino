/* розпіновка кабеля до екнкодера: RB 3100
  |  cabel        | encoder RB 3100 (IO-Link)  |  incremental encoder |
  Pin_1 = bround  | L+ (4,75...30 DC)          |  Ub (4,75...30 DC)   |
  Pin_2 = white   | do not conect              |  A                   |
  Pin_3 = blue    | L-                         |  GND                 |
  Pin_4 = black   | IO-Link                    |  z/0 pulse(90 deg)   |
  Pin_5 = grey    | do not conect              |  B                   |

  Пін 2 (ENC_A): Підключений до першого контакту енкодера (A).
  Пін 3 (ENC_B): Підключений до другого контакту енкодера (B).
  Підключення кнопок до Arduino:

  Пін 4: Підключений до першого контакту кнопки 2 нормально розімкнута віднімання значення -. 
  Пін 5: Підключений до першого контакту кнопки 1 нормально розімкнута добавлення знаяення +.
  Пін 6: Підключений до першого контакту кнопки 3 нормально замкнута скидання значення на 0.
  Пін 7: Підключений до першого контакту кнопки 4 нормально замкнута друк.

  Підключення LCD дисплея через I2C до Arduino:

  SDA: Підключений до відповідного піна Arduino (зазвичай A4) .
  SCL: Підключений до відповідного піна Arduino (зазвичай A5).
  Інші піни, які використовуються в коді:

  Пін 0 і Пін 1: Використовуються для зв'язку з комп'ютером через Serial порт.
  Пін 2 і Пін 3: Використовуються для інтеррупта на зчитування енкодера.
  Пін 4, Пін 5 і Пін 6: Використовуються для зчитування стану кнопок.

*/

/*
  #include <SoftwareSerial.h>

SoftwareSerial printerSerial(10, 11); // Піни TX і RX для з'єднання з принтером

void setup() {
  Serial.begin(9600);
  printerSerial.begin(9600); // Швидкість передачі даних для принтера
}

void loop() {
  // Ваш код основної програми
  // ...
  
  if (currentButtonState == LOW && !printButtonFlag) {
    // Встановлюємо флажок для кнопки друку
    printButtonFlag = true;

    // Задаємо значення met типу long
    long metValue = 265; // Замініть на ваше значення

    // Перетворюємо значення met в String
    String metString = String(metValue, 2); // 2 означає 2 десяткові знаки

    // Комбінуємо значення із словом "метрів"
    String data = metString + " метрів";

    // Надсилаємо дані на принтер
    printToPrinter(data);
  }

  // Перевіряємо, чи кнопка була відпущена (стан з LOW на HIGH)
  if (currentButtonState == HIGH && printButtonFlag) {
    // Знімаємо флажок для кнопки друку, щоб дозволити друкувати знову при наступному натисканні
    printButtonFlag = false;
  }
}

void printToPrinter(String data) {
  // Відправляємо дані на принтер через Serial порт (SoftwareSerial для зовнішнього принтера)
  // Наприклад, використовуємо команду "println" для друку тексту
  printerSerial.println(data);
}

  
  
  */


#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>
#include <SoftwareSerial.h>


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
boolean butt4_flag = 0;


unsigned long last_time = 0; // Оголошення змінної last_time
unsigned long backlight_timer = 0; // Оголошення змінної backlight_timer
unsigned long time_lcd_backlight = 10 * 1000; // Оголошення змінної backlight_timer для встановлення часу підсвічування lcd (секунди потріно перемножити на 1000 мілісекунд)

SoftwareSerial printerSerial(10, 11); // Піни TX і RX для з'єднання з принтером


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
  printerSerial.begin(9600); // Швидкість передачі даних для принтера
  attachInterrupt(digitalPinToInterrupt(ENC_A), int0, CHANGE);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
}

void loop() {
  boolean but1 = !digitalRead(5);
  boolean but2 = !digitalRead(8);
  boolean but3 = digitalRead(6); // Зчитувати кнопку з контактом, який замкнутий при натисканні
  boolean but4 = digitalRead(7); // Зчитувати кнопку з контактом, який замкнутий при натисканні

  float previousMet = met;
  met = met1 * encCounter;

  
  if (met != previousMet) {
    backlight_timer = millis(); // Оновлення часу останньої зміни met
  }

  if (millis() - backlight_timer < time_lcd_backlight) {
    lcd.backlight(); // Увімкнути підсвічування дисплею
  } else {
    lcd.noBacklight(); // Вимкнути підсвічування дисплею
  }

  if (millis() - last_time > 100) {
    last_time = millis();
    String myString = " -> Meters  ";

    lcd.setCursor(0, 0);
    lcd.print(met, 2);
    lcd.print(myString);
    Serial.println(met, 8);
    Serial.println(encCounter);
  }

  

  if (but1 == 1 && butt1_flag == 0) {
    butt1_flag = 1;
    encCounter += 512;
    }

  if (but1 == 0 && butt1_flag == 1) {
    butt1_flag = 0;
  }

  if (but3 == 1 && butt3_flag == 0) { // Зміна умови перевірки для кнопки обнулення
    butt3_flag = 1;
    encCounter = 0;
    backlight_timer = millis();
   }

  if (but3 == 0 && butt3_flag == 1) {
    butt3_flag = 0;
  }

  if (but2 == 1 && butt2_flag == 0) {
    butt2_flag = 1;
    encCounter -= 512;
  }

  if (but2 == 0 && butt2_flag == 1) {
    butt2_flag = 0;
  }


if (but4 == 1 && butt4_flag == 0) { // Зміна умови перевірки для кнопки обнулення
    butt4_flag = 1;
  
  String metString = String(met, 3); // 2 означає 2 десяткові знаки
    String data = "^XA^FO50,50^A0N,30,30^FD" + metString + " метрів^FS^XZ";
    printToPrinter(data); // Надсилаємо дані на принтер
  
   }

  if (but4 == 0 && butt4_flag == 1) {
    butt4_flag = 0;
    }
  }
void printToPrinter(String data) {
  // Відправляємо дані на принтер через Serial порт (SoftwareSerial для зовнішнього принтера)
  // Наприклад, використовуємо команду "println" для друку тексту
  printerSerial.println(data);
}
