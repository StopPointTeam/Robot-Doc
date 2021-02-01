const int buttonPin = 2;
int buttonState = 1;   

void setup() {    
  pinMode(buttonPin, INPUT); 
  Serial.begin(9600);
}

void loop(){
  buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) {
    Serial.println("Button_ON");
  }
  else {      
    Serial.println("Button_OFF");
  }
}
