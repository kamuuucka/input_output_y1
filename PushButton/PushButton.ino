#include <Keyboard.h>
// constants won't change. They're used here to set pin numbers:
const int buttonPin = 9;     // the number of the pushbutton pin
const int ledPin =  12;      // the number of the LED pin

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
int buttonOldState = LOW;
int state = HIGH;
int counter = -1;
bool buttonIsPressed = false;
unsigned long time = 0;          
unsigned long debounce = 200UL;

void setup() {
  delay(2000);
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);
  Keyboard.begin();
}

void loop() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
  //Serial.println(buttonState);

  //    if(buttonState == LOW){
  //      if(buttonIsPressed) return;
  //      buttonIsPressed = true;
  //      Keyboard.press(' ');
  //      delay(5);
  //      Keyboard.releaseAll();
  //      digitalWrite(ledPin, HIGH);
  //    } else {
  //      buttonIsPressed = false;
  //      digitalWrite(ledPin, LOW);
  //    }

  //TOGGLE SWITCH
  Serial.println(counter);
  if (buttonState == HIGH && buttonOldState == LOW && millis() - time > debounce){
    if (state == HIGH){
      state = LOW; 
      counter++;
    }else{
     state = HIGH;
     counter++;
    }
      
    time = millis();
  }

  digitalWrite(ledPin, state);

  buttonOldState = buttonState;
}
