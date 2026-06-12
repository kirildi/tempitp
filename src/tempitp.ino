#include <Arduino.h>
#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>
#include "Fonts/FreeSans9pt7b.h"
#include "Fonts/FreeSans12pt7b.h"

// #include "SSD1306Wire.h"        // legacy: #include "SSD1306.h"

#define SIMULATION 1

#ifdef SIMULATION
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
Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;
float temp_data = 22.0;
float press_data = 1024.0;
float hum_data = 100.0;

// Raindrop bitmap image (10x11 pixels)
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

// Bar. pressure bitmap image (10x11 pixels)
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
    #ifndef SIMULATION
    // Check if the sensor is connected correctly
    if (! aht.begin()) {
      Serial.println("Could not find AHT20. Check wiring!");
    }
    else {
      Serial.println("AHT20 found!");
    }

    // Default I2C address is 0x76, sometimes 0x77
    if (!bmp.begin(0x77)) {
      Serial.println("Could not find BMP280. Check wiring!");
      while (1);
    }
    #endif

    display.setFont(&FreeSans9pt7b);

    drawWelcome();
    drawInitText();

}

void loop() {

  #ifndef SIMULATION
  // Read BMP280
  temp_data = bmp.readTemperature();
  press_data = bmp.readPressure() / 100.0F;

  sensors_event_t hum_ev, temp_ev;
  aht.getEvent(&hum_ev, &temp_ev);
  hum_data = hum_ev.relative_humidity;
  #endif

  Serial.print("Temp: "); Serial.print(temp_data); Serial.print(" C | ");
  Serial.print("Press: "); Serial.print(press_data); Serial.print(" hPa | ");
  Serial.print("Hum: "); Serial.print(hum_data); Serial.println(" %");

  delay(2000);
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);
  delay(1000);
  drawData();
}

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
  display.drawBitmap(66, 2, icon_pressure_large, 10, 11, WHITE);
    
  display.display();
}

void drawData(void) {
  display.clearDisplay();

  display.setFont(&FreeSans9pt7b);
  drawInitText();

  display.setTextSize(1);
  display.setCursor(10, 12);
  display.print(int(trunc(hum_data)));
  display.print("%");
  
  display.setCursor(70, 12);
  display.print(int(trunc(press_data)));
  display.print("hPa");

  display.setFont(&FreeSans12pt7b);
  display.setTextSize(1);
  display.setCursor(12,50);
  display.print(temp_data);
  display.setCursor(86,50);
  //display.write(247); // Degree circle sign(works only for default font)
  display.print("*C");

  display.display();

  delay(1000);
}
