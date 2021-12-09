/*
  Test program Game Controller

  Upload this file to your Arduino, open the Serial monitor and fill in the key combinations in the “first” line. There will be some feedback in the window below.

  1 + enter or send - LED1/D10  On/Off
  2 + enter or send - LED2/D11  On/Off
  3 + enter or send - LED3/D12  On/Off
  4 + enter or send - Display the X and Y values from the joystick

  Pushbuttons on the game controller:
  D2 till D8        -   Feedback in window, and D13 (led on Arduino Nano IOT 33) On/Off

  h or H + enter or send    - Display the help
*/

// declaring array's
int myButtons[8]        = { 0, 0, 0, 0, 0, 0, 0, 0 };
int myPreviousButton[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
bool ledStates[3]       = { 0, 0, 0 };
int firstTime[8]        = { 0, 0, 0, 0, 0, 0, 0, 0 };

// declaring variables
char input;
int analogReading = 0;
int ledLeftPin = 12;
int ledMiddlePin = 11;
int ledRightPin = 10;
int ledOnboardPin = 13;

// the follow variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long time = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers

void setup() {
  Serial.begin(9600);
  delay(3000);
  Serial.print("start\n");
  //Declare In- and Outputs
  pinMode(ledRightPin, OUTPUT);
  pinMode(ledMiddlePin, OUTPUT);
  pinMode(ledLeftPin, OUTPUT);
  pinMode(ledOnboardPin, OUTPUT);

  for (int i = 2; i < 9; i++) {
    pinMode(i, INPUT);
  }

  // write 0 to outputs
  digitalWrite(ledRightPin, 0);
  digitalWrite(ledMiddlePin, 0);
  digitalWrite(ledLeftPin, 0);
  digitalWrite(ledOnboardPin, 0);



  help();

}

void loop() {

  // checking keypressed 1, 2, 3, 4, h, H - Led's on/off, show joystick values, show help
  if (Serial.available() > 0) {
    input = Serial.read();
    if (input == '1') {
      ledStates[0] = !ledStates[0];
      digitalWrite(ledRightPin, ledStates[0]); // Led 10 on
      Serial.print("Led 1 / D10: ");
      Serial.println(ledStates[0]);
      Serial.println("---------------------------");

    }
    if (input == '2') {
      ledStates[1] = !ledStates[1];
      digitalWrite(ledMiddlePin, ledStates[1]); // Led 11 on
      Serial.print("Led 2 / D11: ");
      Serial.println(ledStates[1]);
      Serial.println("---------------------------");
    }

    if (input == '3') {
      ledStates[2] = !ledStates[2];
      digitalWrite(ledLeftPin, ledStates[2]); // Led 12 on
      Serial.print("Led 3 / D12: ");
      Serial.println(ledStates[2]);
      Serial.println("---------------------------");
    }

    if (input == '4') {
      analogReading = !analogReading;
    }

    if (input == 'h' || input == 'H' ) {
      help();
    }

  }

  // show joystickvalues
  if (analogReading == 1) {
    Serial.print("Ypos: ");
    Serial.println(analogRead(0));
    Serial.print("Xpos: ");
    Serial.println(analogRead(1));
      Serial.println("---------------------------");
    delay(100);
  }

  // check gamecontroller buttons are pressed, feedback on the screen and onborad LED
  for (int i = 2; i < 9; i++) {
    myButtons[i] = digitalRead(i);
    if ((myButtons[i] != myPreviousButton[i]) && millis() - time > debounce) {
      if (myButtons[i] == 1) {
        myPreviousButton[i] = 1 ;
        Serial.print("Button: D");
        Serial.print(i);
        Serial.println(" pressed");
        digitalWrite(ledOnboardPin, 1);
        firstTime[i] = 1;
      }
      if (myButtons[i] == 0 && firstTime[i] == 1) {
        firstTime[i] = 0;
        Serial.print("Button: D");
        Serial.print(i);
        Serial.println(" released");
      Serial.println("---------------------------");
        digitalWrite(ledOnboardPin, 0);
      }
      time = millis();
      myPreviousButton[i] = myButtons[i] ;

    }
  }

}

void help() {
  Serial.println("---------------------------------------------------------------------------------------------------");
  Serial.println("|                                                                                                 |");
  Serial.println("|  1 + enter or send - LED1/D10  On/Off                                                           |");
  Serial.println("|  2 + enter or send - LED2/D11  On/Off                                                           |");
  Serial.println("|  3 + enter or send - LED3/D12  On/Off                                                           |");
  Serial.println("|  4 + enter or send - Display the X and Y values from the joystick - 4 + enter or send to stop   |");
  Serial.println("|                                                                                                 |");
  Serial.println("|  Pushbuttons on the game controller:                                                            |");
  Serial.println("|  D2 till D8        - Feedback in window, and D13 (led on Arduino Nano IOT 33) On/Off            |");
  Serial.println("|                                                                                                 |");
  Serial.println("|  h or H + enter or send    - Display the help                                                   |");
  Serial.println("|                                                                                                 |");
  Serial.println("---------------------------------------------------------------------------------------------------");
  Serial.println("");
  delay(1000);
}
