#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
#include <SD.h>

// Pins
#define SD_CS      5
#define TFT_CS     10
#define TFT_DC     7
#define TFT_RST    -1  // If you don't use RESET pin, set to -1
#define TFT_BL     9

#define TP_CS      4
#define TP_IRQ     3

#define GRID_SIZE 10
#define MAX_LENGTH 30

#define VER "v1.0"

struct SnakeSegment {
  int x, y;
};


int menu = 0;
String history[3];  // 3-element array
int index = 0;      // Current write position
bool upload = false;
// Display initialization
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Touch panel initialization
XPT2046_Touchscreen ts(TP_CS, TP_IRQ);

void setup() {
  Serial.begin(9600);
  tft.init(240, 320); // Initialize with 240x320 resolution
  tft.setRotation(3); // Set screen orientation
  tft.fillScreen(ST77XX_WHITE); // Fill screen with white
  pinMode(9, OUTPUT);      // Set D9 as output
  digitalWrite(9, HIGH);    // Turn on backlight
  tft.setCursor(85, 100);   // Announce system is starting
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(4);
  tft.print("ZaurOS");
  tft.setCursor(100, 140);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(2);
  tft.print("Loading");
  if (!ts.begin()) {
    Serial.println("Failed to initialize touch panel.");
    dErr("Failed to initialize touch panel. Fix the screen!!!");
  } else {
    Serial.println("Touch panel initialized.");
  }
  if (!SD.begin(SD_CS)) {
    Serial.println("SD card initialization error.");
    addHistory("No SD card.");
  } else {
    Serial.println("SD card initialized.");
  }
  delay(2000);
  load();
}

void loop() {
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    Serial.print("X = ");
    Serial.print(p.x);
    Serial.print(", Y = ");
    Serial.print(p.y);
    Serial.print(", Pressure = ");
    Serial.println(p.z);
    delay(100);
    load();
    delay(100);
  }
  if (menu == -2) {
    load();
  }
}


void load() {
  switch (menu) {
    case -2:
      loopSnake();
      updateDirection();
      break;
    case -1:
      break;
    case 0:
      dMenu();
      break;
    case 1:
      dSys();
      break;
    case 2:
      dSusp();
      break;
    case 3:
      dSnake();
      break;
    default:
      dErr("Error: screen not recognized!!! Reset the system and report the problem.");
      break;
  }
}

void dErr(String error) {
  menu = -1;
  tft.fillScreen(ST77XX_BLUE);
  tft.setCursor(50, 50);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(4);
  tft.print(":-(");
  tft.setCursor(50, 90);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.print(error);
}

void dMenu() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(10, 10);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.print(hist());
  tft.setCursor(245, 10);
  tft.print("ZaurOS");
  tft.setCursor(10, 220);
  tft.setTextSize(1);  
  tft.print("https://ankylozaur.github.io/ZaurOS-for-Arduino/"); // Placeholder for actual website
  tft.setCursor(250, 220);
  tft.print(VER);
  tft.fillRect(20, 50, 100, 40, ST77XX_BLUE);    // Filled blue rectangle
  tft.drawRect(20, 50, 100, 40, ST77XX_WHITE);   // White border
  tft.setCursor(25, 65);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.print("Credits");
  tft.fillRect(140, 50, 100, 40, ST77XX_BLUE);    // Filled blue rectangle
  tft.drawRect(140, 50, 100, 40, ST77XX_WHITE);   // White border
  tft.setCursor(155, 65);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.print("Sleep");
  tft.fillRect(20, 100, 100, 40, ST77XX_BLUE);    // Filled blue rectangle
  tft.drawRect(20, 100, 100, 40, ST77XX_WHITE);   // White border
  tft.setCursor(35, 115);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.print("Crash");
  tft.fillRect(140, 100, 100, 40, ST77XX_BLUE);    // Filled blue rectangle
  tft.drawRect(140, 100, 100, 40, ST77XX_WHITE);   // White border
  tft.setCursor(155, 115);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.print("Snake");
  if(ts.touched()) {
    TS_Point p = ts.getPoint();
    if (p.x > 2560 && p.x < 3600 && p.y > 960 && p.y < 1390) {
      menu = 1;
      load();
      delay(500);
    }
    if (p.x > 1250 && p.x < 2260 && p.y > 960 && p.y < 1390) {
      menu = 2;
      load();
      delay(500);
    }
    if (p.x > 2560 && p.x < 3600 && p.y > 1390 && p.y < 2350) {
      dErr("System crash");
      delay(500);
    }
    if (p.x > 1250 && p.x < 2260 && p.y > 1390 && p.y < 2350) {
      menu = 3;
      load();
      delay(500);
    }
  }
}

void dSys() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(85, 10);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(4);
  tft.print("ZaurOS");
  tft.setCursor(0, 40);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.print("Project created by AnkyloZaur using Arduino environment, Adafruit libraries, mapped for Waveshare TFT 2.8'' and compatible with UNO R3 and other Arduino boards.");
  if(ts.touched()) {
    menu = 0;
    load();
  }
  if(ts.touched()) {
    menu = 0;
    load();
  }
}

void dSusp() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(85, 100);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(4);
  tft.print("ZaurOS");
  tft.setTextSize(2);
  tft.setCursor(100, 140);
  tft.print("Sleeping...");
  tft.setCursor(250, 220);
  tft.print(VER);
  if(ts.touched()) {
    menu = 0;
    load();
  }
}

void addHistory(String newText) {
  if (index < 3) {
    history[index] = newText;
    index++;
  } else {
    // When array is full — shift everything left (FIFO)
    for (int i = 0; i < 4; i++) {
      history[i] = history[i + 1];
    }
    history[2] = newText; // New text at the end
  }
}

String hist() {
  if (index == 0) return "No alerts"; 
  if (index < 3) return history[index - 1]; 
  return history[2];
}

SnakeSegment snake[MAX_LENGTH];
int snakeLength = 3;
int dirX = 1, dirY = 0; // start: right

int foodX = 5, foodY = 5;

void spawnFood() {
  foodX = random(0, tft.width() / GRID_SIZE);
  foodY = random(0, tft.height() / GRID_SIZE);
}

void drawSquare(int x, int y, uint16_t color) {
  tft.fillRect(x * GRID_SIZE, y * GRID_SIZE, GRID_SIZE - 1, GRID_SIZE - 1, color);
}

void updateDirection() {
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    if (p.y < 1600) { dirX = 0; dirY = -1; } // Up
    else if (p.y > 3000) { dirX = 0; dirY = 1; } // Down
    else if (p.x < 1000) { dirX = 1; dirY = 0; } // Right
    else if (p.x > 2800) { dirX = -1; dirY = 0; } // Left
  }
}

void dSnake() {
  menu = -2;
  tft.fillScreen(ST77XX_BLACK);
  snake[0] = {5, 5};
  snake[1] = {4, 5};
  snake[2] = {3, 5};

  spawnFood();
}

void loopSnake() {

  // Move body
  for (int i = snakeLength - 1; i > 0; i--) {
    snake[i] = snake[i - 1];
  }
  
  snake[0].x += dirX;
  snake[0].y += dirY;

  // Check collision with walls
  if (snake[0].x < 0 || snake[0].y < 0 || 
      snake[0].x >= (tft.width() / GRID_SIZE) || 
      snake[0].y >= (tft.height() / GRID_SIZE)) {
    tft.fillScreen(ST77XX_RED);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.setCursor(100, 140);
    tft.print("You lose!");
    delay(1000);
    menu = 0;
    load(); // restart
    return;
  }

  // Check collision with itself
  for (int i = 1; i < snakeLength; i++) {
    if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
      tft.fillScreen(ST77XX_RED);
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(2);
      tft.setCursor(100, 140);
      tft.print("You lose!");
      delay(1000);
      menu = 0;
      load();
      return;
    }
  }

  // Eating
  if (snake[0].x == foodX && snake[0].y == foodY && snakeLength < MAX_LENGTH) {
    snakeLength++;
    spawnFood();
  }

  if (snakeLength == MAX_LENGTH) {
    tft.fillScreen(ST77XX_BLUE);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.setCursor(100, 140);
    tft.print("You win!");
    delay(1000);
    menu = 0;
    load(); // restart
  }

  tft.fillScreen(ST77XX_BLACK);
  drawSquare(foodX, foodY, ST77XX_GREEN);

  for (int i = 0; i < snakeLength; i++) {
    drawSquare(snake[i].x, snake[i].y, ST77XX_WHITE);
  }

  delay(1000); // game speed
}
