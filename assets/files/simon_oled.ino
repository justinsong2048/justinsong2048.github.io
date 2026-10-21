#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// =====================
// OLED DEFINITIONS
// =====================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// =====================
// SOUND DEFINITIONS
// =====================
#define NOTE_C5   523
#define NOTE_E5   659
#define NOTE_G5   784
#define NOTE_C6  1047
#define NOTE_B4   494
#define NOTE_AS4  466
#define NOTE_G2   98

// =====================
// GAME VARIABLES
// =====================
byte user[100];
byte game[100];

const int button_red = 13;
const int button_green = 12;
const int button_yellow = 11;
const int button_blue = 10;
const int red_led = 9;
const int green_led = 8;
const int yellow_led = 7;
const int blue_led = 6;
const int Start_pin = 5;
const int speaker_pin = 4;

bool game_going = true; 
int game_counter = 1;
int blink_time = 400;
int score = 0;

int freq_red = 523;
int freq_green = 659;
int freq_yellow = 784;
int freq_blue = 1047;

// =====================
// OLED HELPER: CENTER TEXT
// =====================
void drawCenteredText(const char *text, int textSize, int y) {
  display.setTextSize(textSize);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  int x = (SCREEN_WIDTH - w) / 2;
  display.setCursor(x, y);
  display.println(text);
}

// =====================
// OLED SCREENS
// =====================
void showStartMenu() {
  display.clearDisplay();
  drawCenteredText("SIMON", 2, 10);
  drawCenteredText("Press START", 1, 40);
  display.display();
}

void showScore(int score) {
  display.clearDisplay();
  char buf[20];
  sprintf(buf, "Score: %d", score);
  drawCenteredText(buf, 2, 25);
  display.display();
}

void showGameOver(int score) {
  display.clearDisplay();
  drawCenteredText("GAME OVER", 2, 5);

  char buf[20];
  sprintf(buf, "Final: %d", score);
  drawCenteredText(buf, 1, 40);

  drawCenteredText("Press START", 1, 52);
  display.display();
}

// =====================
// SOUND FUNCTIONS
// =====================
void playStartJingle() {
  tone(speaker_pin, NOTE_C5, 150); delay(170);
  tone(speaker_pin, NOTE_E5, 150); delay(170);
  tone(speaker_pin, NOTE_G5, 150); delay(170);
  tone(speaker_pin, NOTE_C6, 300); delay(300);
}

void playGameOverJingle() {
  tone(speaker_pin, NOTE_C5, 100); delay(300);
  tone(speaker_pin, NOTE_B4, 100); delay(300);
  tone(speaker_pin, NOTE_AS4, 100); delay(300);
  tone(speaker_pin, NOTE_G2, 800); delay(800);
}

// =====================
// LED + SOUND BLINK
// =====================
void blink(int LED_pin, int freq) {
  digitalWrite(LED_pin, HIGH);
  tone(speaker_pin, freq);
  delay(blink_time);
  digitalWrite(LED_pin, LOW);
  noTone(speaker_pin);
}

int corresponding_led(int val) {
  if (val == 1) return red_led;
  if (val == 2) return green_led;
  if (val == 3) return yellow_led;
  return blue_led;
}

int corresponding_freq(int val) {
  if (val == 1) return freq_red;
  if (val == 2) return freq_green;
  if (val == 3) return freq_yellow;
  return freq_blue;
}

// =====================
// SETUP
// =====================
void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A0));

  pinMode(button_red, INPUT_PULLUP);
  pinMode(button_green, INPUT_PULLUP);
  pinMode(button_yellow, INPUT_PULLUP);
  pinMode(button_blue, INPUT_PULLUP);
  pinMode(red_led, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(yellow_led, OUTPUT);
  pinMode(blue_led, OUTPUT);
  pinMode(Start_pin, INPUT_PULLUP);
  pinMode(speaker_pin, OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED failed");
    while (true);
  }

  display.setTextColor(SSD1306_WHITE);
  delay(200);

  for (int i = 0; i < 100; i++)
    game[i] = random(1, 5);

  showStartMenu();
}

// =====================
// MAIN LOOP
// =====================
void loop() {

  while (digitalRead(Start_pin) == HIGH) { delay(10); }
  delay(200);

  score = 0;
  game_counter = 1;
  game_going = true;

  playStartJingle();
  showScore(score);

  // =====================
  //       GAME LOOP
  // =====================
  while (game_going) {

    delay(500);

    // --- 1. Show sequence ---
    for (int i = 0; i < game_counter; i++) {
      blink(corresponding_led(game[i]), corresponding_freq(game[i]));
      delay(100);
    }

    // --- 2. Get user input (with 10 sec timeout) ---
    for (int i = 0; i < game_counter; i++) {
      bool button_pressed = false;

      unsigned long startWait = millis();

      while (!button_pressed) {

        // TIMEOUT CHECK — 10 seconds
        if (millis() - startWait >= 10000) {
          game_going = false;
          break;
        }

        if (digitalRead(button_red) == LOW) {
          user[i] = 1; blink(red_led, freq_red); button_pressed = true;
        } 
        else if (digitalRead(button_green) == LOW) {
          user[i] = 2; blink(green_led, freq_green); button_pressed = true;
        } 
        else if (digitalRead(button_yellow) == LOW) {
          user[i] = 3; blink(yellow_led, freq_yellow); button_pressed = true;
        } 
        else if (digitalRead(button_blue) == LOW) {
          user[i] = 4; blink(blue_led, freq_blue); button_pressed = true;
        }
      }

      if (!game_going) break;

      while (digitalRead(button_red)==LOW || digitalRead(button_green)==LOW ||
             digitalRead(button_yellow)==LOW || digitalRead(button_blue)==LOW)
        delay(10);

      if (user[i] != game[i]) {
        game_going = false;
        break;
      }
    }

    if (!game_going) break;

    score++;
    game_counter++;
    showScore(score);
  }

  // =====================
  //       GAME OVER
  // =====================
  playGameOverJingle();
  showGameOver(score);

  while (digitalRead(Start_pin) == HIGH) { delay(10); }
  delay(200);

  for (int i = 0; i < 100; i++)
    game[i] = random(1, 5);
}
