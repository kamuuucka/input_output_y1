/*
  Test program Game Controller

  Upload this file to your Arduino

   normal mode: toggle led states with UP, RIGHT en LEFT buttons
                invert led states with FIRE1, FIRE2 and LEFT buttons
                joystick button to have LEFT/RIGHT led show the analog X/Y values

    user controlled mode if you send serial data, see the help function below
*/

void help() {
  Serial.println("---------------------------------------------------------------------------------------------------");
  Serial.println("|                                                                                                 |");
  Serial.println("|  0 + enter or send - autonomous mode, toggle between analog and digital with joystick button    |");
  Serial.println("|                                   left led controlled by    UP (toggle) and FIRE1 (invert)           |");
  Serial.println("|                                 middle led controlled by RIGHT (toggle) and FIRE2 (invert)      |");
  Serial.println("|                                  right led controlled by  DOWN (toggle) and  LEFT (invert)      |");
  Serial.println("|  1 + enter or send - LED1/D10  On/Off                                                           |");
  Serial.println("|  2 + enter or send - LED2/D11  On/Off                                                           |");
  Serial.println("|  3 + enter or send - LED3/D12  On/Off                                                           |");
  Serial.println("|  4 + enter or send - Display the X and Y values from the joystick - 4 + enter or send to stop   |");
  Serial.println("|                                                                                                 |");
  Serial.println("|  Pushbuttons on the game controller:                                                            |");
  Serial.println("|  D2 till D8        - Feedback in window, and D13 (led on Arduino Nano IOT 33) On/Off            |");
  Serial.println("|                                                                                                 |");
  Serial.println("|  h or H + enter or send    - Display the help                                                   |");
  Serial.println("|  v or V + enter or send    - more verbose output with separator lines                           |");
  Serial.println("|                                                                                                 |");
  Serial.println("---------------------------------------------------------------------------------------------------");
  Serial.println("");
  delay(1000);
}


// -------
// DEFINES
// -------
#define NR_OF_ARRAY_ELEMENTS( array ) ( sizeof( array ) / sizeof( typeof( *array ) ) )


// ---------
// CONSTANTS
// ---------

enum pinModes {
  UNUSED         = -1,
  DIGITAL_INPUT  =  0,
  ANALOG_INPUT   =  1,
  DIGITAL_OUTPUT =  2,
  ANALOG_OUTPUT  =  3,
  SERVO          =  4
};


const int JOYSTICK_BUTTON_PIN =  2;
const int     LEFT_BUTTON_PIN =  3;
const int       UP_BUTTON_PIN =  4;
const int     DOWN_BUTTON_PIN =  5;
const int    RIGHT_BUTTON_PIN =  6;
const int    FIRE2_BUTTON_PIN =  7;
const int    FIRE1_BUTTON_PIN =  8;
const int       RIGHT_LED_PIN = 10;
const int      MIDDLE_LED_PIN = 11;
const int        LEFT_LED_PIN = 12;
const int     ONBOARD_LED_PIN = 13;

const int          X_AXIS_PIN = A1;
const int          Y_AXIS_PIN = A0;




// VARIABLES
boolean verbose = true;

int PIN_MODES[] {
  UNUSED,          // pin 00, TX
  UNUSED,          // pin 01, RX
  DIGITAL_INPUT,   // pin 02, joystick button
  DIGITAL_INPUT,   // pin 03, left button
  DIGITAL_INPUT,   // pin 04, up button
  DIGITAL_INPUT,   // pin 05, down button
  DIGITAL_INPUT,   // pin 06, right button
  DIGITAL_INPUT,   // pin 07, fire 2 button
  DIGITAL_INPUT,   // pin 08, fire 1 button
  UNUSED,          // pin 09, servo, does not properly do PWM if one or more servo pins are used
   ANALOG_OUTPUT,  // pin 10, right led
   ANALOG_OUTPUT,  // pin 11, middle led
   ANALOG_OUTPUT,  // pin 12, left led
  DIGITAL_OUTPUT,  // pin 13, onboard led
   ANALOG_INPUT,   // pin 14, joystick y axis
   ANALOG_INPUT,   // pin 15, joystick x axis
};

const int MAX_PINS = NR_OF_ARRAY_ELEMENTS( PIN_MODES );

int new_pin_state[MAX_PINS];
int old_pin_state[MAX_PINS];
int calibration_values[MAX_PINS];


enum OperationMode {
  ANALOG, DIGITAL, USER_CONTROL, SEND_ANALOG, IDLE,
};
OperationMode operation_mode = DIGITAL;


void setup()
{
  delay( 3000 );     // to make reprogramming etc. easier

  set_all_pinmodes();
  calibrate_analog_inputs();

  Serial.begin( 9600 );
  Serial.println( "Start\n----" );
  Serial.println( A0 );
}


void loop()
{
    read_all_inputs();
    read_and_process_serial_data();
    process_operation_mode();
    write_all_outputs();
    delay( 100 );  // KISS: for now no debouncing necessary
}



void set_all_pinmodes()
{
  for ( int pin = 0 ; pin < MAX_PINS ; pin++ ) {
    if ( PIN_MODES[pin] != UNUSED ) {
      pinMode( pin , new_pin_state[pin] );
      old_pin_state[pin] = new_pin_state[pin] = 0;
    }
  }
}


void calibrate_analog_inputs()
{
  // Not the best calibration, good enough for now
  for ( int pin = 0 ; pin < MAX_PINS ; pin++ ) {
    if ( PIN_MODES[pin] == ANALOG_INPUT ) {
      calibration_values[pin] = analogRead( pin );
    }
  }
}


void read_all_inputs()
{
  for ( int pin = 0 ; pin < MAX_PINS ; pin++ ) {
    if ( PIN_MODES[pin] == DIGITAL_INPUT ) {
      old_pin_state[pin] = new_pin_state[pin];
      new_pin_state[pin] = digitalRead( pin );
      if ( old_pin_state[pin] != new_pin_state[pin] ) {
        print_digital_input_state( pin );
      }
    } else if ( PIN_MODES[pin] == ANALOG_INPUT ) {
      old_pin_state[pin] = new_pin_state[pin];
      new_pin_state[pin] = analogRead( pin );
    }
  }
}


void write_all_outputs()
{
  for ( int pin = 0 ; pin < MAX_PINS ; pin++ ) {
    if ( PIN_MODES[pin] == DIGITAL_OUTPUT ) {
      if ( old_pin_state[pin] != new_pin_state[pin] ) {
        digitalWrite( pin, new_pin_state[pin] );
        print_digital_output_state( pin );
        old_pin_state[pin] = new_pin_state[pin];
      }
    } else if ( PIN_MODES[pin] == ANALOG_OUTPUT ) {
      if ( old_pin_state[pin] != new_pin_state[pin] ) {
        analogWrite( pin, new_pin_state[pin] );
        old_pin_state[pin] = new_pin_state[pin];
      }
    }
  }
}



void read_and_process_serial_data()
{
  while ( Serial.available() > 0 ) {
    char input = Serial.read();
     switch( input ) {
      case 'h': case 'H': help();                            break;
      case 'v': case 'V': verbose = ! verbose;               break;
      case '0': set_operation_mode( DIGITAL ); reset_leds(); break;
      case '1':
      case '2':
      case '3': set_operation_mode( USER_CONTROL ); toggle_led( 12 + '1' - input ); break;
      case '4': set_operation_mode( SEND_ANALOG  ); break;
     }
  }
}


void process_operation_mode()
{
  switch( operation_mode ) {
    case ANALOG:       do_analog_mode();   break;
    case DIGITAL:      do_digital_mode();  break;
    case USER_CONTROL: do_user_control();  break;
    case SEND_ANALOG:  send_analog_data(); break; 
    case IDLE:         idle();             break;
  }
}



// Weird error, should know the enum type so this ugly workaround is needed???
void set_operation_mode( int new_mode )
//void set_operation_mode( OperationMode new_mode )
{
  switch( new_mode ) {
    case SEND_ANALOG:   operation_mode = operation_mode == SEND_ANALOG ? USER_CONTROL : SEND_ANALOG; break; 
    case IDLE:
    case ANALOG:
    case DIGITAL:       
//    case USER_CONTROL:       operation_mode = new_mode;  break;
    case USER_CONTROL: operation_mode = (OperationMode) new_mode;  break;
  }
}


void do_analog_mode()
{
  toggle_operation_mode_if_necessary();

  new_pin_state[ ONBOARD_LED_PIN ] = HIGH;

  new_pin_state[  LEFT_LED_PIN ] = abs( calibration_values[ X_AXIS_PIN ] - new_pin_state[ X_AXIS_PIN ] ) / 2;
  new_pin_state[ RIGHT_LED_PIN ] = abs( calibration_values[ Y_AXIS_PIN ] - new_pin_state[ Y_AXIS_PIN ] ) / 2;
  if (  new_pin_state[  LEFT_LED_PIN ] < 16 && new_pin_state[ RIGHT_LED_PIN ] < 16 ) {
      new_pin_state[ MIDDLE_LED_PIN ] = 255;
  } else {
      new_pin_state[ MIDDLE_LED_PIN ] = 0;
  }
}


void do_digital_mode()
{
  toggle_operation_mode_if_necessary();

  new_pin_state[ ONBOARD_LED_PIN ] = LOW;
  old_pin_state[  LEFT_LED_PIN] = new_pin_state[  LEFT_LED_PIN];
  old_pin_state[MIDDLE_LED_PIN] = new_pin_state[MIDDLE_LED_PIN];
  old_pin_state[ RIGHT_LED_PIN] = new_pin_state[ RIGHT_LED_PIN];

  toggle_led_with_button(   LEFT_LED_PIN ,    UP_BUTTON_PIN, FIRE1_BUTTON_PIN );
  toggle_led_with_button( MIDDLE_LED_PIN , RIGHT_BUTTON_PIN, FIRE2_BUTTON_PIN );
  toggle_led_with_button(  RIGHT_LED_PIN ,  DOWN_BUTTON_PIN,  LEFT_BUTTON_PIN );

  
}


void do_user_control()
{
  // nothing extra, only command processing
}


void send_analog_data()
{
  for ( int pin = 0 ; pin < MAX_PINS ; pin++ ) {
    if ( PIN_MODES[pin] == ANALOG_INPUT ) {
       print_analog_input_state( pin );
    }
  }
  if ( verbose ) {
    Serial.println("---------------------------");
  }
}


void toggle_operation_mode_if_necessary()
{
   if ( ! old_pin_state[ JOYSTICK_BUTTON_PIN ] && new_pin_state[ JOYSTICK_BUTTON_PIN ] ) {
     if ( operation_mode == ANALOG ) {
       set_operation_mode( DIGITAL );
     } else {
       set_operation_mode( ANALOG );
     }
   }
   set_idle_mode_if_necessary();
}


// Warning: all leds are currently PWM so 255 is used
void toggle_led( int led_pin )
{
  new_pin_state[led_pin] = 255 - ( old_pin_state[led_pin] == 0 ? 0 : 255 );
}


void toggle_led_with_button( int led_pin, int button_pin, int invert_button_pin )
{
  if ( ! old_pin_state[ button_pin ] && new_pin_state[ button_pin ] ) {
    toggle_led( led_pin );
  }
  if ( new_pin_state[ invert_button_pin ] != old_pin_state[ invert_button_pin ] ) {
    toggle_led( led_pin );
  }
}


void reset_leds()
{
   new_pin_state[   LEFT_LED_PIN ] = new_pin_state[   LEFT_LED_PIN ] == 0 ? 0 : 255;
   new_pin_state[ MIDDLE_LED_PIN ] = new_pin_state[ MIDDLE_LED_PIN ] == 0 ? 0 : 255;
   new_pin_state[  RIGHT_LED_PIN ] = new_pin_state[  RIGHT_LED_PIN ] == 0 ? 0 : 255;
}


void print_digital_input_state( int pin )
{
  Serial.print  ( "Pin D" );
  Serial.print  ( pin         );
  Serial.println( new_pin_state[ pin ] == LOW ? " released" : " pressed" );
  if ( verbose ) {
    Serial.println( "---------------------------" );
  }
}


void print_digital_output_state( int pin )
{
  Serial.print  ( "Pin D" );
  Serial.print  ( pin         );
  Serial.println( new_pin_state[ pin ] == LOW ? " off" : " on" );
  if ( verbose ) {
    Serial.println( "---------------------------" );
  }
}


void print_analog_input_state( int pin )
{
  Serial.print  ( "Analog A" );
  Serial.print  ( pin - 14 );
  Serial.print  ( " " );
  Serial.println( new_pin_state[ pin ] );
  // No separator, done at the end of all analog values
}








































































































































void set_idle_mode_if_necessary()
{
   if (    new_pin_state[ JOYSTICK_BUTTON_PIN ] && new_pin_state[ UP_BUTTON_PIN    ] 
        && new_pin_state[ DOWN_BUTTON_PIN     ] && new_pin_state[ RIGHT_BUTTON_PIN ]
        && new_pin_state[ LEFT_BUTTON_PIN     ]                                      ) {
     set_operation_mode( IDLE );
   }
}

int pattern[][3] {
  { 255,   0,   0 },
  {   0, 255,   0 },
  {   0,   0, 255 },
  {   0,   0, 255 },
  {   0, 255,   0 },
  { 255,   0,   0 },
};

int idx = -1;

void idle() {
  if ( ! old_pin_state[ FIRE1_BUTTON_PIN ] && new_pin_state[ FIRE1_BUTTON_PIN ] ) {
    set_operation_mode( DIGITAL );
  }
  idx = ( idx + 1 ) % NR_OF_ARRAY_ELEMENTS( pattern );
  new_pin_state[   LEFT_LED_PIN ] = pattern[ idx ][ 0 ];
  new_pin_state[ MIDDLE_LED_PIN ] = pattern[ idx ][ 1 ];
  new_pin_state[  RIGHT_LED_PIN ] = pattern[ idx ][ 2 ];  
}
