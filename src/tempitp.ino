#include <Arduino.h>
#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Fonts/FreeMono9pt7b.h"
#include "Fonts/FreeMono12pt7b.h"

// #include "SSD1306Wire.h"        // legacy: #include "SSD1306.h"

#define IS_SIMULATION 1

#ifdef IS_SIMULATION
  #define I2C_SDA 21  // I2C Pins ESP32 in Wokwi pins
  #define I2C_SCL 22
#else
  #define I2C_SDA 33  // I2C Pins Wemos/Lolin S2 Mini
  #define I2C_SCL 35
#endif

#define LED_PIN 15// could be 15
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


// TwoWire I2CBME = TwoWire(0);

// #define SEALEVELPRESSURE_HPA (1013.25)

// Declaration for SSD1306 display connected using I2C
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // 0x3C for 128 x 32 // 0x3D for 128 x 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// SSD1306Wire display(SCREEN_ADDRESS, I2C_SDA, I2C_SCL);  // ADDRESS, SDA, SCL, OLEDDISPLAY_GEOMETRY  -  Extra param required for 128x32 displays.

// Нова по-голяма капка (10x11 пиксела)
const unsigned char icon_droplet_large[] PROGMEM = {
  B00001100, B00000000,
  B00001100, B00000000,
  B00011110, B00000000,
  B00011110, B00000000,
  B00111111, B00000000,
  B01111111, B10000000,
  B11111111, B11000000,
  B11111111, B11000000,
  B01111111, B10000000,
  B00111111, B00000000,
  B00011110, B00000000
};

// Ново по-голямо налягане (10x11 пиксела)
const unsigned char icon_pressure_large[] PROGMEM = {
  B00011110, B00000000,
  B00100001, B00000000,
  B01001000, B10000000,
  B10001000, B01000000,
  B10011100, B01000000,
  B10000000, B01000000,
  B10000000, B01000000,
  B10000000, B01000000,
  B01000000, B10000000,
  B00100001, B00000000,
  B00011110, B00000000
};
void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);
    Serial.println("Starting");
    Wire.begin();

    pinMode(I2C_SDA, OUTPUT);
    pinMode(I2C_SCL, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(I2C_SDA, HIGH);
    digitalWrite(I2C_SCL, HIGH);
    digitalWrite(LED_PIN, HIGH);
    //  I2CBME.begin(I2C_SDA,I2C_SCL, 100000);

    // SSD1306_SWITCHCAPVCC or SSD1306_EXTERNALVCC
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }
    display.setFont(&FreeMono9pt7b);

    drawWelcome();
    drawInitText();

}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);
  delay(1000);
  drawData();
}

float temp_data = 43.0;
int hum_data = 1.0;

void drawWelcome(void) {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(28,36);
  display.println("TempiTP");
  display.display();

  delay(1600);
  display.clearDisplay();
}

void drawInitText() {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.drawBitmap(0, 2, icon_droplet_large, 10, 11, WHITE);   
  display.drawBitmap(70, 2, icon_pressure_large, 10, 11, WHITE);
    
  display.display();
}

void drawData(void) {
  display.clearDisplay();

  display.setFont(&FreeMono9pt7b);
  drawInitText();

  display.setTextSize(1);
  display.setCursor(14, 12);
  display.print(hum_data * 100);
  display.print("%");
  
  display.setCursor(84, 12);
  display.print("1024");

  display.setFont(&FreeMono12pt7b);
  display.setTextSize(1);
  display.setCursor(12,50);
  display.print(temp_data);
  display.setCursor(86,50);
  //display.write(247); // Degree circle sign(works only for default font)
  display.print("*C");

  display.display();

  delay(1000);
}
