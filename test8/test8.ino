// this program is for line following robot
// IR Sensors
#define leftIR   2
#define rightIR  3

// Motor Pins
#define IN1  4
#define IN2  5
#define IN3  6
#define IN4  7
#define EN   9     // Shared enable pin for both motors

// Speeds
const int SPEED = 180;          // 0–255

void setup() {
  Serial.begin(115200);

  pinMode(leftIR, INPUT);
  pinMode(rightIR, INPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(EN, OUTPUT);

  analogWrite(EN, SPEED);

  Serial.println("Line follower started");
  delay(300);
}

void loop() {
  int left  = digitalRead(leftIR);
  int right = digitalRead(rightIR);

  // Optional debug (uncomment while testing)
  // Serial.print("L:"); Serial.print(left);
  // Serial.print(" R:"); Serial.println(right);

  if (left == 0 && right == 0) {
    moveForward();
  }
  else if (left == 0 && right == 1) {
    turnLeft();
  }
  else if (left == 1 && right == 0) {
    turnRight();
  }
  else {
    stopRobot();
  }

  delay(8);   // small delay for stability
}

// -------------------- Motor Functions --------------------

void moveForward() {
  analogWrite(EN, SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnLeft() {
  // Left motor stopped, right motor running
  analogWrite(EN, SPEED);
  digitalWrite(IN1, LOW);   // Left motor OFF
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);  // Right motor ON
  digitalWrite(IN4, LOW);
}

void turnRight() {
  // Right motor stopped, left motor running
  analogWrite(EN, SPEED);
  digitalWrite(IN1, HIGH);  // Left motor ON
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);   // Right motor OFF
  digitalWrite(IN4, LOW);
}

void stopRobot() {
  analogWrite(EN, 0);       // Stop both motors
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}