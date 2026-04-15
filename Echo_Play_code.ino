#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pins
int buttonPins[] = {3, 5, 6};
int ledPins[] = {2, 4, 7};
int buzzer = 8;

// Notes
int notes[] = {262, 294, 330};

// Main Menu
int menuIndex = 0;
String menuItems[] = {"Memory Mode", "Song Mode", "Game Mode"};;
int totalMenus = 3;

// Song Menu
int songIndex = 0;
String songItems[] = {"Twinkle", "Coffin", "Rickroll"};
int totalSongs = 3;

// Demo melody
int demoMelody[] = {0,0,2,2,1,1,0};
int demoLength = 7;

// Game
int sequence[50];
int level = 0;

// ---------------- SETUP ----------------
void setup() {
  lcd.init();
  lcd.backlight();

  for (int i = 0; i < 3; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    pinMode(ledPins[i], OUTPUT);
  }

  pinMode(buzzer, OUTPUT);

  randomSeed(analogRead(0));
  showMenu();
}

// ---------------- LOOP ----------------
void loop() {
  handleMenu();
}

// ---------------- MAIN MENU ----------------
void showMenu() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Select Mode");
  lcd.setCursor(0,1);
  lcd.print(menuItems[menuIndex]);
}

void handleMenu() {
  if (digitalRead(buttonPins[0]) == LOW) {
    delay(200);
    menuIndex = (menuIndex + 1) % totalMenus;
    showMenu();
  }

  if (digitalRead(buttonPins[1]) == LOW) {
    delay(300);

    if (menuIndex == 0) runMemoryMode();
    else if (menuIndex == 1) runSongMenu();
    else runGameMode();

    showMenu();
  }
}

void runSongMenu() {

  int lastIndex = -1;

  while (true) {

    // Update display only when needed
    if (songIndex != lastIndex) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select Song");

      lcd.setCursor(0, 1);
      lcd.print("> ");
      lcd.print(songItems[songIndex]);

      lastIndex = songIndex;
    }

    // Scroll button
    if (digitalRead(buttonPins[0]) == LOW) {
      delay(200);
      songIndex = (songIndex + 1) % totalSongs;
    }

    // Select button (ONLY ONE BLOCK)
    if (digitalRead(buttonPins[1]) == LOW) {
      delay(300);

      lcd.clear();
      lcd.print("Playing...");
      delay(500);

      if (songIndex == 0) playTwinkle();
      else if (songIndex == 1) playCoffinDance();
      else playRickroll();

      delay(1000);

      lastIndex = -1; // refresh menu after playing
    }

    // Exit button
    if (digitalRead(buttonPins[2]) == LOW) {
      delay(300);
      return;
    }
  }
}
// ---------------- MEMORY MODE ----------------
void runMemoryMode() {
  lcd.clear();
  lcd.print("Repeat!");

  delay(1000);
  playDemo();

  if (checkDemoInput()) successSound();
  else gameOver();

  delay(1500);
}

void playDemo() {
  for (int i=0;i<demoLength;i++){
    int n=demoMelody[i];

    digitalWrite(ledPins[n], HIGH);
    tone(buzzer, notes[n]);
    delay(400);

    digitalWrite(ledPins[n], LOW);
    noTone(buzzer);
    delay(200);
  }
}

bool checkDemoInput() {
  for (int i=0;i<demoLength;i++){
    int input = waitForButton();

    digitalWrite(ledPins[input], HIGH);
    tone(buzzer, notes[input]);
    delay(400);

    digitalWrite(ledPins[input], LOW);
    noTone(buzzer);

    if (input != demoMelody[i]) return false;
  }
  return true;
}

// ---------------- GAME MODE ----------------
void runGameMode() {
  level = 0;

  while (true) {
    sequence[level] = random(0,3);

    lcd.clear();
    lcd.print("Level ");
    lcd.print(level+1);

    delay(800);

    for (int i=0;i<=level;i++){
      int n=sequence[i];

      digitalWrite(ledPins[n], HIGH);
      tone(buzzer, notes[n]);
      delay(400);

      digitalWrite(ledPins[n], LOW);
      noTone(buzzer);
      delay(200);
    }

    for (int i=0;i<=level;i++){
      int input = waitForButton();

      digitalWrite(ledPins[input], HIGH);
      tone(buzzer, notes[input]);
      delay(400);

      digitalWrite(ledPins[input], LOW);
      noTone(buzzer);

      if (input != sequence[i]) {
        gameOver();
        return;
      }
    }

    successSound();
    level++;
  }
}

// ---------------- SONGS ----------------
void playTwinkle() {
  int melody[] = {
    262,262,392,392,440,440,392,
    349,349,330,330,294,294,262
  };

  int duration[] = {
    400,400,400,400,400,400,800,
    400,400,400,400,400,400,800
  };

  playMelody(melody, duration, 14);
}

void playCoffinDance() {
  int melody[] = {
    262,262,262,262,
    294,294,294,294,
    330,330,330,330,
    294,294,294,294,
    262,262,262,262
  };

  int duration[] = {
    250,250,250,250,
    250,250,250,250,
    250,250,250,250,
    250,250,250,250,
    300,300,300,300
  };

  playMelody(melody, duration, 20);
}

void playRickroll() {
  int melody[] = {
    294,330,349,330,
    294,262,294,330,
    349,330,294
  };

  int duration[] = {
    300,300,400,300,
    300,300,300,400,
    300,300,600
  };

  playMelody(melody, duration, 11);
}

void playMelody(int melody[], int duration[], int length) {
  for (int i=0;i<length;i++){
    digitalWrite(ledPins[i % 3], HIGH);

    tone(buzzer, melody[i]);
    delay(duration[i]);

    digitalWrite(ledPins[i % 3], LOW);
    noTone(buzzer);
    delay(50);
  }
}

// ---------------- BUTTON ----------------
int waitForButton() {
  while (true) {
    for (int i=0;i<3;i++){
      if (digitalRead(buttonPins[i])==LOW){
        delay(30);
        if (digitalRead(buttonPins[i])==LOW){
          while(digitalRead(buttonPins[i])==LOW);
          delay(100);
          return i;
        }
      }
    }
  }
}

// ---------------- SOUND ----------------
void successSound() {
  tone(buzzer,1000);
  delay(300);
  noTone(buzzer);
}

void gameOver() {
  lcd.clear();
  lcd.print("Game Over");

  for(int i=0;i<3;i++){
    tone(buzzer,200);
    delay(300);
    noTone(buzzer);
    delay(200);
  }
}