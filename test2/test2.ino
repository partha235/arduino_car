// this program i to check readings from HC-SR04 Ultrasonic sensor. 

#define Trig 6
#define Echo 7

long getDistance() {
  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);
  
  long duration = pulseIn(Echo, HIGH, 30000); // timeout ~30ms (~5m max)
  if (duration == 0) return -1; // no echo
  
  long distance = duration * 0.0343 / 2; // slightly more accurate
  return distance;
}

void setup() {
  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);
  Serial.begin(9600);
}  

void loop() {
  long dist = getDistance();
  
  Serial.print("distance = ");
  
  Serial.print(dist); Serial.println(" cm");
  
  
  delay(100);
}