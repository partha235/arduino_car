#include <Wire.h>
#include "Adafruit_TCS34725.h"

// ================== PIN DEFINITIONS ==================
#define M1  8   // Left forward
#define M2  9   // Left backward
#define M3  10  // Right forward
#define M4  12  // Right backward
#define Ep  11  // Enable (PWM)

// RGB LED (common anode)
#define redpin   3
#define greenpin 5
#define bluepin  6
#define commonAnode true

// ================== TUNING ==================
const int   SPEED           = 110;      // 0-255
const int   RED_MIN         = 70;       // Minimum red value
const float RED_RATIO       = 1.35;     // red must be this times higher than G and B
const unsigned long SEARCH_TIMEOUT = 700;  // ms without red before searching
const int   SEARCH_SPIN_MS  = 90;       // how long each search spin lasts

// ================== GLOBALS ==================
Adafruit_TCS34725 tcs = Adafruit_TCS34725(
  TCS34725_INTEGRATIONTIME_24MS,
  TCS34725_GAIN_16X
);

byte gammatable[256];
unsigned long lastSeenRed = 0;
bool searchingLeft = true;

// ================== SETUP ==================
void setup() {
  Serial.begin(9600);

  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(M3, OUTPUT);
  pinMode(M4, OUTPUT);
  pinMode(Ep, OUTPUT);
  analogWrite(Ep, SPEED);

  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  pinMode(bluepin, OUTPUT);

  // Gamma correction table for LED
  for (int i = 0; i < 256; i++) {
    float x = i / 255.0;
    x = pow(x, 2.5) * 255.0;
    gammatable[i] = commonAnode ? 255 - x : x;
  }

  if (tcs.begin()) {
    Serial.println("TCS34725 found");
  } else {
    Serial.println("No TCS34725 found - check wiring!");
    while (1);
  }

  stopMotors();
  Serial.println("Ready. Looking for red...");
}

// ================== MAIN LOOP ==================
void loop() {
  // ---- Read color sensor ----
  float r, g, b;
  tcs.setInterrupt(false);          // LED on
  delay(28);                        // wait for integration
  tcs.getRGB(&r, &g, &b);
  tcs.setInterrupt(true);           // LED off

  int red   = (int)r;
  int green = (int)g;
  int blue  = (int)b;

  // Show color on RGB LED
  analogWrite(redpin,   gammatable[constrain(red,   0, 255)]);
  analogWrite(greenpin, gammatable[constrain(green, 0, 255)]);
  analogWrite(bluepin,  gammatable[constrain(blue,  0, 255)]);

  // ---- Red detection ----
  bool seesRed = (red > RED_MIN) &&
                 (red > green * RED_RATIO) &&
                 (red > blue  * RED_RATIO);

  // ---- Decision ----
  if (seesRed) {
    lastSeenRed = millis();
    moveForward();
    Serial.println("RED → FORWARD");
  }
  else if (millis() - lastSeenRed < SEARCH_TIMEOUT) {
    // Recently saw red → keep going a bit more
    moveForward();
    Serial.println("Recent red → continue");
  }
  else {
    // Lost red → search by spinning
    searchForRed();
  }

  // Debug
  Serial.print("R:"); Serial.print(red);
  Serial.print(" G:"); Serial.print(green);
  Serial.print(" B:"); Serial.print(blue);
  Serial.print("  seesRed:"); Serial.println(seesRed ? "YES" : "NO");
}

// ================== MOTOR FUNCTIONS ==================
void moveForward() {
  analogWrite(Ep, SPEED);
  digitalWrite(M1, HIGH);
  digitalWrite(M2, LOW);
  digitalWrite(M3, HIGH);
  digitalWrite(M4, LOW);
}

void spinLeft() {
  analogWrite(Ep, SPEED);
  digitalWrite(M1, LOW);
  digitalWrite(M2, HIGH);   // Left backward
  digitalWrite(M3, HIGH);   // Right forward
  digitalWrite(M4, LOW);
}

void spinRight() {
  analogWrite(Ep, SPEED);
  digitalWrite(M1, HIGH);   // Left forward
  digitalWrite(M2, LOW);
  digitalWrite(M3, LOW);
  digitalWrite(M4, HIGH);   // Right backward
}

void stopMotors() {
  analogWrite(Ep, 0);
  digitalWrite(M1, LOW);
  digitalWrite(M2, LOW);
  digitalWrite(M3, LOW);
  digitalWrite(M4, LOW);
}

// Non-blocking search (alternates direction)
void searchForRed() {
  static unsigned long lastSpinChange = 0;

  if (millis() - lastSpinChange > SEARCH_SPIN_MS) {
    searchingLeft = !searchingLeft;
    lastSpinChange = millis();
  }

  if (searchingLeft) {
    spinLeft();
    Serial.println("Searching LEFT");
  } else {
    spinRight();
    Serial.println("Searching RIGHT");
  }
}