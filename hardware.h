////// hardware-dependent features
// these change for active-low and active-high outputs.

#ifndef _HARDWARE_H_
#define _HARDWARE_H_

// Arduino Pro Mini 5V
// Counterclockwise from Pin 0 (TTL UART pins facing 12 o'clock)
// Pin 0 = UART TX
// Pin 1 = UART RX
// Pin 2 = Reader data0 - hardcoded in Wiegand lib
// Pin 3 = Reader data1 - as above
#define RFID_DOORBELL   4  // needs pullup, normally open
#define RFID_LED        5  // PWM
#define RFID_BEEP       6  // PWM
#ifndef MORSE_PIN
#define MORSE_PIN RFID_BEEP
#endif
#define INPUT1     7  // needs pullup, door closed = circuit closed
#define INPUT2     8  // needs pullup, door closed = circuit closed
#define INPUT3     9  // needs pullup, door closed = circuit closed

//// Other side, continuing upward ////

#define OUTPUT1 10
#define OUTPUT2 11
#define OUTPUT3 12
#define OUTPUT4 13 // built-in LED

// Future ... Analog inputs?

// Note inboard A4/A5 may also be used for I2C clock/data
// Inboard pins...
// #define INPUT4 19 // marked A6
// #define INPUT5 22 // marked A7


// Digital inputs get debounced...
Bounce Dbell = Bounce();
Bounce Input1 = Bounce();
Bounce Input2 = Bounce();
Bounce Input3 = Bounce();
#ifdef INPUT4
Bounce Input4 = Bounce();
#endif
#ifdef INPUT5
Bounce Input5 = Bounce();
#endif

void wiringHalt(int pin, int expect, String desc) {
  while (true) {
    Serial.print(F("HALTED Wiring error during setup(): Pin ")); Serial.print(pin, DEC); Serial.print(" (");
    Serial.print(desc); Serial.print(F(") unexpectedly ")); Serial.println((expect ? "LOW" : "HIGH"));
    delay(5000); // wait 5 seconds, then check again
    // if it was a momentary glitch, resume startup sequence
    if (digitalRead(pin) == expect) {
      Serial.println(F("Looks like it returned to normal ... resuming startup."));
      return;
    }
  }
}

void wiringCheck(int pin, int expect, int wantmode, String desc) {
#ifdef WIRING_CHECK
  if (digitalRead(pin) != expect) wiringHalt(pin, expect, desc);
#endif
  pinMode(pin, wantmode);
}


void LED(boolean actv) {
  FlashRate=0; // because we want binary output...
  pinMode(RFID_LED, OUTPUT);
  if (actv) {
    digitalWrite(RFID_LED, LOW);
  } else {
//    pinMode(RFID_LED, INPUT );
    digitalWrite(RFID_LED, HIGH);
  }
}
void BEEP(boolean actv) {
  pinMode(RFID_BEEP, OUTPUT);
  if (actv) {
    digitalWrite(RFID_BEEP, LOW);
  } else {
    digitalWrite(RFID_BEEP, HIGH);
  }
}

/*void BEEPTONE(uint32_t mytone) {
  if (( mytone > 30) && (mytone < 18001)) {
    tone(RFID_BEEP, mytone);
    return;
  }
  noTone(RFID_BEEP);
  pinMode(RFID_BEEP, INPUT);
  digitalWrite(RFID_BEEP, HIGH);
}*/

void HardwareSetup() {
  delay(1000); // wait a second for the keypad to start up
  wiringCheck(2,              HIGH, INPUT, F("Keypad DATA-0 input"));
  wiringCheck(3,              HIGH, INPUT, F("Keypad DATA-1 input"));
  wiringCheck(RFID_DOORBELL,   LOW, INPUT_PULLUP, F("Doorbell input"));
  wiringCheck(RFID_LED,       HIGH, OUTPUT, F("Keypad LED output"));  // I float just over the TTL threshold
  wiringCheck(RFID_BEEP,      HIGH, OUTPUT, F("Keypad beep output")); // ^ me too
  wiringCheck(INPUT1,     LOW, INPUT_PULLUP, F("Input 1"));
  wiringCheck(INPUT2,     LOW, INPUT_PULLUP, F("Input 2"));
  wiringCheck(INPUT3,     LOW, INPUT_PULLUP, F("Input 3"));
#ifdef INPUT4
  wiringCheck(INPUT4,     LOW, INPUT_PULLUP, F("Input 4"));
#endif
#ifdef INPUT5
  wiringCheck(INPUT5,     LOW, INPUT_PULLUP, F("Input 5"));
#endif
  wiringCheck(OUTPUT1,         LOW, OUTPUT, F("Output 1"));
  wiringCheck(OUTPUT2,         LOW, OUTPUT, F("Output 2"));
  wiringCheck(OUTPUT3,         LOW, OUTPUT, F("Output 3"));
  wiringCheck(OUTPUT4,         LOW, OUTPUT, F("Output 4"));

  // set initial states:
  LED(false);
  BEEP(false);
 
  wg.begin();
  delay(250); // let pullups settle
  Dbell.attach(RFID_DOORBELL);  Dbell.interval(5); // milliseconds
  Input1.attach(INPUT1);    Input1.interval(25); // milliseconds
  Input2.attach(INPUT2);    Input2.interval(25); // milliseconds
  Input3.attach(INPUT3);    Input3.interval(25); // milliseconds
#ifdef INPUT4
  Input4.attach(INPUT4);    Input4.interval(25); // milliseconds
#endif
#ifdef INPUT5
  Input5.attach(INPUT5);    Input5.interval(25); // milliseconds
#endif
}

void checkEvent() {
  // Bell:   LOW == button is being pressed
  // Inputs:  LOW == door is shut
  Dbell.update();
  if (Dbell.fell()) {
    isBell = BELL_RING;
  }
  if (Dbell.rose()) {
    if (isBell == BELL_RING) {
      Serial.println("DOORBELL");
    }
    isBell = BELL_NONE; // reset state
  }
  Input1.update();
  if (Input1.fell()) Serial.println(F("INPUT 1 LOW"));
  if (Input1.rose()) Serial.println(F("INPUT 1 HIGH"));
  Input2.update();
  if (Input2.fell()) Serial.println(F("INPUT 2 LOW"));
  if (Input2.rose()) Serial.println(F("INPUT 2 HIGH"));
  Input3.update();
  if (Input3.fell()) Serial.println(F("INPUT 3 LOW"));
  if (Input3.rose()) Serial.println(F("INPUT 3 HIGH"));
#ifdef INPUT4
  Input4.update();
  if (Input4.fell()) Serial.println(F("INPUT 4 LOW"));
  if (Input4.rose()) Serial.println(F("INPUT 4 HIGH"));
#endif
#ifdef INPUT5
  Input5.update();
  if (Input5.fell()) Serial.println(F("INPUT 5 LOW"));
  if (Input5.rose()) Serial.println(F("INPUT 5 HIGH"));
#endif
}

#endif // _HARDWARE_H_

