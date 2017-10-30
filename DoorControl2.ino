
#define VERSION_BANNER "Compiled " __TIME__ " " __DATE__
#define STATUS_PERIOD 10000UL // Emit a heartbeat message every 10 seconds

// globals for LED flash
unsigned long NextFlash = 0;
unsigned long FlashRate = 0;

// globals for periodic status
unsigned long NextStatus = 0;
boolean reboot=true;

// globals for using the doorbell as a shift key
#define BELL_NONE 0
#define BELL_RING 1
#define BELL_SHIFT 2
byte isBell = BELL_NONE;

// Feeling buggy?
#define DEBUG true

// BittyMorse settings - define before including!
// #define MORSE_DEBUG true
#define MORSE_OUTPUT_ON LOW
#define MORSE_OUTPUT_OFF HIGH

// No display ... yet.
// #include <LiquidCrystal_I2C.h>

// reader instance
#include <Wiegand.h>
WIEGAND wg;

#include <String.h>
#include <Bounce2.h>
#include <TimeLib.h>
#include "hardware.h"
#include "BittyMorse.h"


// bitmask for Status()
#define STATUS_TIME   0x01
#define STATUS_INPUT  0x02
#define STATUS_OUTPUT 0x04
#define STATUS_FLASH  0x08
#define STATUS_ALL    0xFF

///////////////////// FUNCTIONS /////////////////////

String readLine(boolean numeric=false) {   // read serial input until CR/LF
  unsigned long timeout = millis() + 3000UL; // or 3 second timeout
  String mybuffer;
  while (millis() < timeout) {
    if (Serial.available()) {
      uint8_t inbyte = Serial.read();
      if (inbyte == 0x0D) break; // end on CR
      if (inbyte == 0x0A) break; // or LF
      if (inbyte < 0x20) continue; // skip other control chars
      if ((numeric) && ((inbyte < '0') || (inbyte > '9'))) continue;
      mybuffer += char(inbyte);
    }
  }
#ifdef DEBUG
  Serial.print(F("DEBUG readLine returning "));
  Serial.println(mybuffer);
#endif
  return(mybuffer);
}

String WallClock() {
  String mytime="00:00:00";
  mytime.setCharAt(0, (hour() / 10) + 0x30);
  mytime.setCharAt(1, (hour() % 10) + 0x30);
  mytime.setCharAt(3, (minute() / 10) + 0x30);
  mytime.setCharAt(4, (minute() % 10) + 0x30);
  mytime.setCharAt(6, (second() / 10) + 0x30);
  mytime.setCharAt(7, (second() % 10) + 0x30);
  return(mytime);
}

void Status(uint8_t section=STATUS_ALL) {
  if (section & STATUS_TIME) {
    Serial.print(F("STATUS UPTIME "));
    Serial.print(millis(), DEC);
    Serial.print(F(" CLOCK "));
    if (timeStatus()!= timeNotSet) {
      Serial.println(WallClock());
    } else {
      Serial.println(F("NOTSET"));
    }
  }  
  if (section & STATUS_INPUT) {
    Serial.print(F("STATUS INPUT ")); 
    Serial.print(Input1.read(), DEC);
    Serial.print(" ");
    Serial.print(Input2.read(), DEC);
    Serial.print(" ");
    Serial.print(Input3.read(), DEC);
#ifdef INPUT4
    Serial.print(" ");
    Serial.print(Input4.read(), DEC);
#endif
#ifdef INPUT5 
    Serial.print(" ");
    Serial.println(Input5.read(), DEC);
#endif
    Serial.println("");
  }
  //// Future: Analog inputs?
  if (section & STATUS_OUTPUT) {
    Serial.print(F("STATUS OUTPUT "));
    Serial.print(digitalRead(OUTPUT1), DEC);
    Serial.print(" ");
    Serial.print(digitalRead(OUTPUT2), DEC);
    Serial.print(" ");
    Serial.print(digitalRead(OUTPUT3), DEC);
    Serial.print(" ");
    Serial.println(digitalRead(OUTPUT4), DEC);
  }
  if (section & STATUS_FLASH) {
    Serial.print(F("STATUS FLASHRATE "));
    Serial.print(FlashRate, DEC);
    Serial.print(" ");
    Serial.println(NextFlash, DEC);
  }
}

void OK(String remark)  { Serial.print("OK "); Serial.println(remark); }
void getFlashRate() {
  FlashRate = (0 + readLine(true).toInt());
  OK("FLASHRATE " + String(FlashRate, DEC));
  pinMode(RFID_LED, OUTPUT);
}

void getClockOffset() {
  unsigned long pctime = (0 + readLine(true).toInt());
  setTime(pctime); // Sync Arduino clock to the time received on the serial port
}

void doMorse(String somejunk=readLine()) {
  Serial.print("DEBUG: doMorse: ");
  Serial.println(somejunk);
  for (int i=0; i < somejunk.length(); i++) {
    playMorse(somejunk.charAt(i));
  }
}

void checkAction() {
  if (! Serial.available()) return;
  uint8_t newcmd = Serial.read(); // gobble up one byte
  switch (int(newcmd)) {
    case '1': digitalWrite(OUTPUT1, LOW);   Status(STATUS_OUTPUT); break;
    case '2': digitalWrite(OUTPUT2, LOW);   Status(STATUS_OUTPUT); break;
    case '3': digitalWrite(OUTPUT3, LOW);   Status(STATUS_OUTPUT); break;
    case '4': digitalWrite(OUTPUT4, LOW);   Status(STATUS_OUTPUT); break;
    case '!': digitalWrite(OUTPUT1, HIGH);  Status(STATUS_OUTPUT); break;
    case '@': digitalWrite(OUTPUT2, HIGH);  Status(STATUS_OUTPUT); break;
    case '#': digitalWrite(OUTPUT3, HIGH);  Status(STATUS_OUTPUT); break;
    case '$': digitalWrite(OUTPUT4, HIGH);  Status(STATUS_OUTPUT); break;
    case '.': Status(STATUS_ALL); break;
    case '?': Help(); break;
    case 'B': BEEP(true);    OK("BEEP ON"); break;
    case 'b': BEEP(false);   OK("BEEP OFF"); break;
    case 'L': LED(true);     OK("LED GREEN"); break;
    case 'l': LED(false);    OK("LED RED"); break;
    case 'f': getFlashRate(); break;
    case 't': getClockOffset(); break;
    case 'm': doMorse(); break;
#ifdef MORSE_DEBUG
    case 'M': MorseDump(); break;
#endif
    case 0x0D: Serial.println("OK"); return; break; // acknowledge CR
    case 0x0A: Serial.println("OK"); return; break; // acknowledge LF
  }
}

void Help() {
  OK(F(VERSION_BANNER));
  OK(F("HELP: (?)Help      (.)Status"));
  OK(F("HELP: (l)ed red    (L)ed green    (f)(msec)(CR) Flash LED"));
  OK(F("HELP: (b)eep off   (B)eep on      (m)(string)(CR) Output Morse"));
  OK(F("HELP: (1/2/3/4) Output LOW        (!/@/#/$) Output HIGH"));
}


//  KEYPAD LAYOUT (without / with shift)
// 1  2  3       A  B  C
// 4  5  6       D  E  F
// 7  8  9       G  H  I
// :  0  ;       J  @  K

void checkWiegand() {
  if (wg.available()) {
    uint32_t myCode = wg.getCode();
    if (myCode < 0xFF) {        // Keypress
      myCode = (myCode & 0x0F); // Only keep the meaningful part - the low 4 bits
      if (isBell == BELL_NONE) {
        myCode += 0x30; // 0 ... 9, ESC=:, ENT=; = Unshifed keys
      } else {
        myCode += 0x40; // @, A ... I, ESC=J, ENT=K = Shifted keys
        isBell = BELL_SHIFT;  // not a doorbell request
      }
      Serial.print(F("KEY "));
      Serial.println(char(myCode));
      return;
    }
    Serial.print(F("READ "));
    if (isBell == BELL_NONE) {
      Serial.println(myCode, HEX);
    } else {
      isBell = BELL_SHIFT;
      Serial.print(myCode, HEX);
      Serial.println(F(" SHIFT"));
    }
  }
}

void doFlash() {
  if (FlashRate == 0) return;
  if (millis() < NextFlash) return;
  digitalWrite(RFID_LED, !digitalRead(RFID_LED));
  NextFlash=millis() + FlashRate;
}

void periodicStatus() {
  if (millis() < STATUS_PERIOD) {
    if (NextStatus > 10000000UL ) {
      Serial.println(F("DEBUG caught millis() overflow"));
      NextStatus = STATUS_PERIOD;
      return;
    }
    if (NextStatus == 0UL ) {
      Serial.println(F("DEBUG rebooted"));
      NextStatus = STATUS_PERIOD;
      return;
    }
  }
  if (NextStatus > millis()) return;
  NextStatus= ( millis() + STATUS_PERIOD);
  Status(STATUS_TIME);
}

/////////////////// Main program //////////////////////

void setup() {
  Serial.begin(115200);
  HardwareSetup();
  while (!Serial); // required for some models ... but which ones?
  pinMode(MORSE_PIN, OUTPUT);
}

void loop() {
  periodicStatus();
  doFlash();
  checkEvent();
  doFlash();
  checkWiegand();
  doFlash();
  checkAction();
  doFlash();
}

