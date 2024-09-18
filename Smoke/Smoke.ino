const int Smoke_Pin = 32;

void smoke(){
  int smokeState = digitalRead(Smoke_Pin);

  if(smokeState != HIGH){
    Serial.println("Smoke Detected");
  }else{
    Serial.println("Read..");
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(Smoke_Pin, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  smoke();
  delay(1000);
}
