#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <math.h>

LiquidCrystal lcd(6, 5, 4, 3, 2, 1);

#define PINtoRESET A0
#define ip_1 7
#define ip_2 8
#define ip_3 9
#define ip_4 10
#define op_1 11
#define op_2 12
#define op_3 13
#define BUZZER A1

int randNumber = 0;
unsigned long startMillis;
unsigned long currentMillis;
unsigned long timeLimits[] = {10000, 13000, 18000};  // Time limits for Easy, Medium, Hard
unsigned long currentTimeLimit;

String problems[40];
int solutions[40];
char keypad[10] = {1, 10, 10, 10, 10, 10, 10, 10, 10, 10};
int solcounter = 0;
int difficulty = 0;  // 0: Easy, 1: Medium, 2: Hard

void setup() {
  randomSeed(analogRead(A2));
  pinMode(PINtoRESET, INPUT_PULLUP);
  digitalWrite(PINtoRESET, HIGH);
  lcd.begin(16, 2);
  
  for (int i = op_1; i <= op_3; i++)
    pinMode(i, OUTPUT);
  
  pinMode(ip_1, INPUT_PULLUP);
  pinMode(ip_2, INPUT_PULLUP);
  pinMode(ip_3, INPUT_PULLUP);
  pinMode(ip_4, INPUT_PULLUP);
  
  initiate_Game();
}

void loop() {
  for (int i = op_1; i <= op_3; i++)
    digitalWrite(i, HIGH);
  
  column1();
  column2();
  column3();
  
  currentMillis = millis();
  updateCountdown();
  
  if ((currentMillis - startMillis) > currentTimeLimit) {
    gameOver("Time's up!");
  }
  
  // Check for reset button press
  if (digitalRead(PINtoRESET) == LOW) {
    delay(50); // Debounce
    if (digitalRead(PINtoRESET) == LOW) {
      resetGame();
    }
  }
}

void generateQuestions() {
  for (int i = 0; i < 40; i++) {
    int type = random(0, 10);  // Now 10 types of questions
    int a, b, c;
    
    switch (difficulty) {
      case 0:  // Easy
        a = random(1, 10);
        b = random(1, 10);
        c = random(2, 5);
        break;
      case 1:  // Medium
        a = random(10, 50);
        b = random(10, 50);
        c = random(3, 7);
        break;
      case 2:  // Hard
        a = random(50, 100);
        b = random(50, 100);
        c = random(5, 10);
        break;
    }
    
    switch (type) {
      case 0:  // Addition
        problems[i] = String(a) + "+" + String(b) + "=?";
        solutions[i] = a + b;
        break;
      case 1:  // Subtraction
        problems[i] = String(a + b) + "-" + String(a) + "=?";
        solutions[i] = b;
        break;
      case 2:  // Multiplication
        problems[i] = String(a) + "X" + String(c) + "=?";
        solutions[i] = a * c;
        break;
      case 3:  // Division
        problems[i] = String(a * c) + "/" + String(c) + "=?";
        solutions[i] = a;
        break;
      case 4:  // Mixed operation
        problems[i] = String(a) + "+" + String(b) + "X" + String(c) + "=?";
        solutions[i] = a + (b * c);
        break;
      case 5:  // Equation
        problems[i] = "x+" + String(a) + "=" + String(a + b) + ",x=?";
        solutions[i] = b;
        break;
      case 6:  // Pattern
        problems[i] = String(a) + "," + String(a + c) + "," + String(a + 2 * c) + ",?";
        solutions[i] = a + 3 * c;
        break;
      case 7:  // Power
        problems[i] = String(c) + "^" + String(2) + "=?";
        solutions[i] = c * c;
        break;
      case 8:  // Square root
        problems[i] = "sqrt(" + String(a * a) + ")=?";
        solutions[i] = a;
        break;
      case 9:  // Percentage (new type)
        problems[i] = String(a) + "% of " + String(b * 100) + "=?";
        solutions[i] = a * b;
        break;
    }
  }
}

void initiate_Game() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome to");
  lcd.setCursor(3, 1);
  lcd.print("Math Quiz ");
  
  delay(2000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("High Score:");
  lcd.setCursor(0, 1);
  lcd.print(EEPROM.read(0));
  
  delay(2000);
  
  chooseDifficulty();
  generateQuestions();
  start_Game();
}

void chooseDifficulty() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Choose difficulty");
  lcd.setCursor(0, 1);
  lcd.print("1:Easy 2:Med 3:Hard");
  
  while (true) {
    if (digitalRead(ip_1) == LOW) {
      delay(50); // Debounce
      if (digitalRead(ip_1) == LOW) {
        difficulty = 0;
        break;
      }
    } else if (digitalRead(ip_2) == LOW) {
      delay(50); // Debounce
      if (digitalRead(ip_2) == LOW) {
        difficulty = 1;
        break;
      }
    } else if (digitalRead(ip_3) == LOW) {
      delay(50); // Debounce
      if (digitalRead(ip_3) == LOW) {
        difficulty = 2;
        break;
      }
    }
  }
  
  currentTimeLimit = timeLimits[difficulty];
  delay(500);
}

void start_Game() {
  lcd.clear();
  randNumber = random(0, 40);
  lcd.print(problems[randNumber]);
  startMillis = millis();
  updateCountdown();
}

void updateCountdown() {
  unsigned long remainingTime = (currentTimeLimit - (currentMillis - startMillis)) / 1000;
  lcd.setCursor(0, 1);
  lcd.print("T:");
  if (remainingTime < 10) lcd.print("0");
  lcd.print(remainingTime);
  lcd.print("s ");
}

void fill_Array(int x) {
  for (int i = 1; i < 10; i++) {
    if (keypad[i] == 10) {
      keypad[i] = x;
      break;
    }
  }
}

void find_Last_Element() {
  int endElement = 0;
  for (int i = 1; i < 10; i++) {
    if (keypad[i] == 10) {
      endElement = i - 1;
      break;
    }
  }
  check_Ans(endElement);
}

void check_Ans(int endElement) {
  long int answer = 0;
  for (int i = endElement, x = 1; i >= 1; i--, x++) {
    answer += keypad[i] * pow(10, x - 1);
  }
  answer *= keypad[0];
  
  if (answer == solutions[randNumber]) {
    correctAnswer();
  } else {
    gameOver("Wrong Answer!");
  }
}

void correctAnswer() {
  lcd.setCursor(7, 1);
  lcd.print("Correct!");
  playCorrectTone();
  delay(1000);
  solcounter++;
  
  if (solcounter == 40) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Congratulations!");
    lcd.setCursor(0, 1);
    lcd.print("You're a Genius!");
    playVictoryTone();
    delay(3000);
    resetGame();
  } else {
    resetKeypad();
    start_Game();
  }
}

void gameOver(String message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(solcounter);
  
  if (solcounter > EEPROM.read(0)) {
    EEPROM.write(0, solcounter);
    lcd.clear();
    lcd.print("New High Score!");
    lcd.setCursor(0, 1);
    lcd.print("Score: ");
    lcd.print(solcounter);
  }
  
  playGameOverTone();
  delay(3000);
  resetGame();
}

void resetGame() {
  solcounter = 0;
  resetKeypad();
  initiate_Game();
}

void resetKeypad() {
  keypad[0] = 1;
  for (int i = 1; i < 10; i++) {
    keypad[i] = 10;
  }
}

void column1() {
  digitalWrite(op_1, LOW);
  checkButton(ip_1, "1", 1);
  checkButton(ip_2, "4", 4);
  checkButton(ip_3, "7", 7);
  checkButton(ip_4, "-", -1);
  digitalWrite(op_1, HIGH);
}

void column2() {
  digitalWrite(op_2, LOW);
  checkButton(ip_1, "2", 2);
  checkButton(ip_2, "5", 5);
  checkButton(ip_3, "8", 8);
  checkButton(ip_4, "0", 0);
  digitalWrite(op_2, HIGH);
}

void column3() {
  digitalWrite(op_3, LOW);
  checkButton(ip_1, "3", 3);
  checkButton(ip_2, "6", 6);
  checkButton(ip_3, "9", 9);
  if (digitalRead(ip_4) == LOW) {
    delay(10);
    lcd.setCursor(7, 1);
    lcd.print("         ");
    find_Last_Element();
  }
  digitalWrite(op_3, HIGH);
}

void checkButton(int pin, String label, int value) {
  if (digitalRead(pin) == LOW) {
    delay(10);
    lcd.setCursor(7, 1);
    lcd.print(label);
    if (value == -1) {
      keypad[0] = -1;
    } else {
      fill_Array(value);
    }
    playKeyTone(value);
    delay(300);
  }
}

void playKeyTone(int key) {
  int frequency = 1000 + (key * 100);
  tone(BUZZER, frequency, 100);
}

void playCorrectTone() {
  tone(BUZZER, 1000, 100);
  delay(100);
  tone(BUZZER, 1500, 100);
  delay(100);
  tone(BUZZER, 2000, 100);
}

void playGameOverTone() {
  tone(BUZZER, 500, 300);
  delay(300);
  tone(BUZZER, 400, 300);
  delay(300);
  tone(BUZZER, 300, 300);
}

void playVictoryTone() {
  for (int i = 0; i < 3; i++) {
    tone(BUZZER, 1000, 100);
    delay(100);
    tone(BUZZER, 1500, 100);
    delay(100);
    tone(BUZZER, 2000, 100);
    delay(100);
  }
}
