#include <Wire.h>
#include "Adafruit_TCS34725.h"

// Motor pins (L298N style - full 4-pin control)
#define M1 8   // Left forward
#define M2 9   // Left backward
#define M3 10  // Right forward
#define M4 12  // Right backward

#define Ep 11

// RGB LED pins (visual feedback)
#define redpin   3
#define greenpin 5
#define bluepin  6
#define commonAnode true

byte gammatable[256];

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_16X);

// Simple tuning
const int   RED_THRESHOLD    = 120;     // red value > this → on red
const unsigned long SEARCH_TIMEOUT = 800;  // ms without red before search

int speed = 100;

unsigned long lastSeenRed = 0;

void setup() {
  Serial.begin(9600);

  // Motor pins
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(M3, OUTPUT);
  pinMode(M4, OUTPUT);
  pinMode(Ep, OUTPUT);

  analogWrite(Ep,speed);

  if (tcs.begin()) {
    Serial.println("TCS34725 found");
  } else {
    Serial.println("No TCS34725 ... check wiring");
    while (1);
  }

  // RGB LED setup
  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  pinMode(bluepin, OUTPUT);

  for (int i = 0; i < 256; i++) {
    float x = i / 255.0;
    x = pow(x, 2.5) * 255.0;
    gammatable[i] = commonAnode ? 255 - x : x;
  }

  stopMotors();
  Serial.println("Ready. Place red close to sensor.");
  Serial.println("Red > 120 → forward");
  Serial.println("Red <= 120 → search by turning");
}

void loop() {
  analogWrite(Ep,speed);
  float rf, gf, bf;
  tcs.setInterrupt(false);  // sensor LED on
  delay(30);
  tcs.getRGB(&rf, &gf, &bf);
  tcs.setInterrupt(true);   // sensor LED off

  int red   = (int)rf;
  int green = (int)gf;
  int blue  = (int)bf;

  // RGB LED shows detected color
  analogWrite(redpin,   gammatable[constrain(red,   0, 255)]);
  analogWrite(greenpin, gammatable[constrain(green, 0, 255)]);
  analogWrite(bluepin,  gammatable[constrain(blue,  0, 255)]);

  // --- Improved red detection ---
  bool seesRed = (red > 80) &&
                 (red > green * 1.4) &&
                 (red > blue * 1.4);

  if (seesRed) {
    lastSeenRed = millis();
    digitalWrite(M1, HIGH);
    digitalWrite(M2, LOW);
    digitalWrite(M3, HIGH);
    digitalWrite(M4, LOW);
    Serial.println("RED detected → forward");
  }
  else if (millis() - lastSeenRed < SEARCH_TIMEOUT) {
    digitalWrite(M1, HIGH);
    digitalWrite(M2, LOW);
    digitalWrite(M3, HIGH);
    digitalWrite(M4, LOW);
    Serial.println("Recent red → continue");
  }
  else {
    if ((millis() / 700) % 2 == 0) {
      Serial.println("Searching RIGHT");
      digitalWrite(M1, HIGH);
      digitalWrite(M2, LOW);
      digitalWrite(M3, LOW);
      digitalWrite(M4, HIGH);
      delay(120);
    } else {
      Serial.println("Searching LEFT");
      digitalWrite(M1, LOW);
      digitalWrite(M2, HIGH);
      digitalWrite(M3, HIGH);
      digitalWrite(M4, LOW);
      delay(120);
    }
  }

  // --- Debug ---
  Serial.print("R: "); Serial.print(red);
  Serial.print("  G: "); Serial.print(green);
  Serial.print("  B: "); Serial.print(blue);
  Serial.print("  seesRed: "); Serial.println(seesRed ? "YES" : "NO");

  delay(30);
}


void stopMotors() {
  digitalWrite(M1, LOW);
  digitalWrite(M2, LOW);
  digitalWrite(M3, LOW);
  digitalWrite(M4, LOW);
}