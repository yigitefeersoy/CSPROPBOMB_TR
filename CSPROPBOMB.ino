#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); 

const byte ROWS = 4; 
const byte COLS = 4; 
char keys[ROWS][COLS] = {
  {'1','2','3','A'}, {'4','5','6','B'}, {'7','8','9','C'}, {'*','0','#','D'}
};

byte rowPins[ROWS] = {2, 3, 4, 5}; 
byte colPins[COLS] = {7, 8, 9, 10}; 

Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const int buzzerPin = 6;
const int buttonPin = 11;
const int ledPin = 12;

String password_store = "";
bool bomb_active = false;
unsigned long bomb_start_time = 0;
const long bomb_duration = 45000; 
unsigned long last_beep_time = 0;
int defuse_progress = 0;

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  
  fullReset(); 
}

void loop() {
  char key = kpd.getKey();

  if (!bomb_active) {
    if (key) {
      tone(buzzerPin, 1000, 50);
      if (key == '#') {
        if (password_store == "7355608") activateBomb();
        else {
          lcd.clear();
          lcd.print("KOD HATALI!");
          delay(1500);
          fullReset();
        }
      } else if (key == '*') {
        fullReset();
      } else {
        password_store += key;
        lcd.setCursor(0, 1);
        lcd.print(password_store);
      }
    }
  } else {
    handleBombLogic();
  }
}

void fullReset() {
  bomb_active = false;
  password_store = "";
  defuse_progress = 0;
  bomb_start_time = 0;
  last_beep_time = 0;
  
  digitalWrite(ledPin, LOW);
  noTone(buzzerPin);
  
  lcd.clear();
  lcd.print("SIFRE GIRIN:");
  lcd.setCursor(0, 1);
  lcd.print("                ");
}

void activateBomb() {
  bomb_active = true;
  bomb_start_time = millis();
  password_store = "";
  lcd.clear();
  lcd.print("BOMBA AKTIF!");
  tone(buzzerPin, 1500, 500);
  delay(1000);
}

void handleBombLogic() {
  unsigned long current_time = millis();
  long elapsed = current_time - bomb_start_time;
  long remaining = (bomb_duration - elapsed) / 1000;

  lcd.setCursor(0, 0);
  lcd.print("SURE: ");
  lcd.print(remaining);
  lcd.print("s  ID:"); 
  lcd.print(defuse_progress);
  lcd.print("% ");

  int beep_speed = map(remaining, 45, 0, 1000, 100);
  if (current_time - last_beep_time >= beep_speed) {
    tone(buzzerPin, 2000, 40);
    digitalWrite(ledPin, !digitalRead(ledPin));
    last_beep_time = current_time;
  }

  if (digitalRead(buttonPin) == LOW) {
    defuse_progress++;
    delay(50);
    if (defuse_progress >= 100) {
      lcd.clear();
      lcd.print("IMHA EDILDI!");
      tone(buzzerPin, 1200, 1000);
      delay(3000);
      fullReset();
    }
  } else {
    defuse_progress = 0;
  }

  if (remaining <= 0) explode();
}

void explode() {
  lcd.clear();
  lcd.print("BOOOOM!");
  tone(buzzerPin, 100, 3000);
  delay(4000);
  fullReset();  
}
