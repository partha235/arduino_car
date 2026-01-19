#include <Servo.h>

#define Trig 6
#define Echo 7
#define M1 2   // Left motor forward
#define M2 3   // Left motor backward
#define M3 4   // Right motor forward
#define M4 5   // Right motor backward
#define water A0
#define buz 10
#define led 13



Servo myservo;

int distance = 100;         // Current front distance
int distanceLeft = 0;
int distanceRight = 0;
const int safeDistance = 25; // cm - adjust as needed

void setup() {
  myservo.attach(9);
  myservo.write(90);        // Center position

  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(M3, OUTPUT);
  pinMode(M4, OUTPUT);
  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);
  pinMode(water, INPUT);
  pinMode(buz, OUTPUT);
  pinMode(led, OUTPUT);

  digitalWrite(buz,0);
  digitalWrite(led,0);

  Serial.begin(9600);
  Serial.println("Obstacle Avoiding Robot Started!");
  delay(1500);              // Give time to settle
}

void loop() {
  distance = getDistance(); // Check front
  int wat_r=analogRead(water);

  if(wat_r>=350){
    digitalWrite(buz,1);
    digitalWrite(led,1);
    delay(1000);
  }
  else{
    digitalWrite(buz,0);
    digitalWrite(led,0);
  }


  if (distance <= safeDistance && distance > 0) {
    stopMotors();
    Serial.println("Obstacle detected! Scanning...");

    delay(300);

    // Look left
    myservo.write(45);
    delay(500);
    distanceLeft = getDistance();

    // Look right
    myservo.write(135);
    delay(500);
    distanceRight = getDistance();

    // Back to center
    myservo.write(90);
    delay(300);

    Serial.print("Left: "); Serial.print(distanceLeft);
    Serial.print(" | Right: "); Serial.println(distanceRight);

    if (distanceLeft > distanceRight && distanceLeft > safeDistance) {
      Serial.println("Turning LEFT");
      turnLeft();
      delay(600);           // Adjust time for ~90° turn
    }
    else if (distanceRight > distanceLeft && distanceRight > safeDistance) {
      Serial.println("Turning RIGHT");
      turnRight();
      delay(600);
    }
    else {
      Serial.println("Both sides blocked - Turning BACK");
      backward();
      delay(800);
      turnRight();          // or turnLeft() - your choice
      delay(1200);
    }
  }
  else {
    // Path clear → go forward
    forward();
  }

  delay(100); // Small loop delay for stability
}

// ------------------ Your existing functions ------------------

long getDistance() {
  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);

  long duration = pulseIn(Echo, HIGH, 30000);
  if (duration == 0) return -1;

  long distance = duration * 0.0343 / 2;
  Serial.print("Distance: "); Serial.print(distance); Serial.println(" cm");
  return distance;
}

void forward() {
  digitalWrite(M1, HIGH); digitalWrite(M2, LOW);
  digitalWrite(M3, HIGH); digitalWrite(M4, LOW);
  Serial.println("Forward");
}

void backward() {
  digitalWrite(M1, LOW); digitalWrite(M2, HIGH);
  digitalWrite(M3, LOW); digitalWrite(M4, HIGH);
  Serial.println("Backward");
}

void turnLeft() {
  digitalWrite(M1, LOW);  digitalWrite(M2, HIGH);  // Left backward
  digitalWrite(M3, HIGH); digitalWrite(M4, LOW);   // Right forward
  Serial.println("Left Turn");
}

void turnRight() {
  digitalWrite(M1, HIGH); digitalWrite(M2, LOW);   // Left forward
  digitalWrite(M3, LOW);  digitalWrite(M4, HIGH);  // Right backward
  Serial.println("Right Turn");
}

void stopMotors() {
  digitalWrite(M1, LOW); digitalWrite(M2, LOW);
  digitalWrite(M3, LOW); digitalWrite(M4, LOW);
  Serial.println("Stopped");
}