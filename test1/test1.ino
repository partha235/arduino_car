// motor working
#define M1 2   
#define M2 3
#define M3 4
#define M4 5   

void setup() {
 
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(M3, OUTPUT);
  pinMode(M4, OUTPUT);
 
  Serial.begin(9600);
  Serial.println("Robot Starting...");
  
  delay(1000);
}

void forward() {
  digitalWrite(M1, HIGH); digitalWrite(M2, LOW);
  digitalWrite(M3, HIGH); digitalWrite(M4, LOW);
  Serial.println("forw");
}

void backward() {
  digitalWrite(M1, LOW);  digitalWrite(M2, HIGH);
  digitalWrite(M3, LOW);  digitalWrite(M4, HIGH);
  Serial.println("back");
}

void turnLeft() {
  digitalWrite(M1, LOW);  digitalWrite(M2, HIGH);   // Left motors backward
  digitalWrite(M3, HIGH); digitalWrite(M4, LOW);    // Right motors forward
}

void turnRight() {
  digitalWrite(M1, HIGH); digitalWrite(M2, LOW);    // Left forward
  digitalWrite(M3, LOW);  digitalWrite(M4, HIGH);   // Right backward
}

void stopMotors() {
  digitalWrite(M1, LOW); digitalWrite(M2, LOW);
  digitalWrite(M3, LOW); digitalWrite(M4, LOW);
}

void loop() {
  Serial.println("hi");
    // forward();
    // delay(2000);
    // stopMotors();
    // delay(200);
    backward();
    delay(2000);
}