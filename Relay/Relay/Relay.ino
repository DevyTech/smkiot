#define relay D5

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(relay,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(relay,HIGH);
  Serial.println("Relay High");
  delay(5000);
  digitalWrite(relay,LOW);
  Serial.println("Relay Low");
  delay(1000);
}
