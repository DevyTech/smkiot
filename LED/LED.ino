void setup() {
  // put your setup code here, to run once:
  pinMode(15, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(18, OUTPUT);
  pinMode(19, OUTPUT);
  pinMode(21, OUTPUT);
  pinMode(22, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(15, HIGH); // turn the LED off
  delay(500);             // wait for 500 milliseconds
  digitalWrite(15, LOW);  // turn the LED on
  delay(500);             // wait for 500 milliseconds
  digitalWrite(4, HIGH); // turn the LED off
  delay(500);             // wait for 500 milliseconds
  digitalWrite(4, LOW);  // turn the LED on
  delay(500);             // wait for 500 milliseconds
  digitalWrite(18, HIGH); // turn the LED off
  delay(500);             // wait for 500 milliseconds
  digitalWrite(18, LOW);  // turn the LED on
  delay(500);             // wait for 500 milliseconds
  digitalWrite(19, HIGH); // turn the LED off
  delay(500);             // wait for 500 milliseconds
  digitalWrite(19, LOW);  // turn the LED on
  delay(500);             // wait for 500 milliseconds
  digitalWrite(21, HIGH); // turn the LED off
  delay(500);             // wait for 500 milliseconds
  digitalWrite(21, LOW);  // turn the LED on
  delay(500);             // wait for 500 milliseconds
  digitalWrite(22, HIGH); // turn the LED off
  delay(500);             // wait for 500 milliseconds
  digitalWrite(22, LOW);  // turn the LED on
  delay(500);             // wait for 500 milliseconds
}
