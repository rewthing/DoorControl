// BittyMorse:  A bitwise-encoded Morse library for AVR microcontrollers, using a PROGMEM lookup table.
// Version 20171030 - @rewthing
// Each Morse symbol takes only a single byte of program memory, making it ideal for memory-constrained environments.

// With due apologies to Mr. Ed:
// A Morse is a Morse, of course, of course, unless you've already loaded Morse.
// And noone can doubt in Morse, of course. It's already in your head.
// Go right to the source that asked for Morse; #include it twice - that's fine, of course!
// Don't you worry about this source. "No warnings", compiler said. ;)

#ifndef _BITTYMORSE_H_
#define _BITTYMORSE_H_

// define these otherwise (prior to #include) if your output is active LOW.
#ifndef MORSE_OUTPUT_ON
#define MORSE_OUTPUT_ON HIGH
#endif
#ifndef MORSE_OUTPUT_OFF
#define MORSE_OUTPUT_OFF LOW
#endif

// Morse code timings are all based on the duration of a "DIT", the short tone...
// Per http://forums.qrz.com/index.php?threads/morse-code-timing-and-spacing.178795/
// A DIT (the short tone) is equal in duration to the pause between DITs/DAHs in a character.
// A DAH tone is three times as long, equal to the pause between characters.
// The pause between words is 7 times as long as a DIT.
// The delay below can be calculated as 1200 / words-per-minute = milliseconds
#ifndef MORSE_DELAY
#define MORSE_DELAY 92 // 13 WPM = 92.31 milliseconds per DIT (here, rounded down to 92)
#endif

// The lookup table
const uint8_t BittyMorse[] PROGMEM = {
    0xF0,      // 0x00  ^@: NUL
    B00110101, // 0x01  ^A: SOH = Begin transmission
    B00101010, // 0x02  ^B: STX = New Page
    B01000101, // 0x03  ^C: ETX = Silent Key
    B01000101, // 0x04  ^D: EOT/EOF = Silent Key
    0xF0,      // 0x05  ^E: ENQuire
    B00100010, // 0x06  ^F: ACK = Positive Acknowledged
    B00110101, // 0x07  ^G: BEL = Begin transmission
    0xF0,      // 0x08  ^H: Backspace
    B00110001, // 0x09  ^I: TAB = New Paragraph
    B00010101, // 0x0A  ^J: LF = New Line
    B00110001, // 0x0B  ^K: VT = New Paragraph
    B00101010, // 0x0C  ^L: FF = New Page
    0xF0,      // 0x0D  ^M: CR = cannot (don't) render
    0xF0,      // 0x0E  ^N: shift out
    0xF0,      // 0x0F  ^O: shift in
    0xF0,      // 0x10  ^P: data link escape
    B00001101, // 0x11  ^Q: DC1 (XON) = K, go ahead
    0xF0,      // 0x12  ^R: DC2
    B00011000, // 0x13  ^S: DC3 (XOFF) = Wait
    0xF0,      // 0x14  ^T: DC4
    0xFF,      // 0x15  ^U: NAK = Special case (eight sounds): Error
    0xF0,      // 0x16  ^V: SYNc
    0xF0,      // 0x17  ^W: ETB
    0xFF,      // 0x18  ^X: CANcel = Special case (eight sounds): Error
    0xF0,      // 0x19  ^Y: EM
    0xF0,      // 0x1A  ^Z: SUB
    0xF0,      // 0x1B  ^[: ESC
    0xF0,      // 0x1C  ^\: Field Sep
    0xF0,      // 0x1D  ^]: Group Sep
    0xF0,      // 0x1E  ^^: Record Sep
    0xF0,      // 0x1F  ^_: Unit Sep
    0xF4,      // 0x20 SPACE: Inter-word pause (7 dit duration, minus the automatic delay from the previous char)
    B01101011, // 0x21   !
    B01010010, // 0x22   "
    0xF0,      // 0x23   #: no Morse
    B10001001, // 0x24   $: Seven played bits = our longest (non-prosign) sound
    0xF0,      // 0x25   %: no Morse
    B00101000, // 0x26   &
    B01011110, // 0x27   '
    B00110110, // 0x28   (
    B01101101, // 0x29   )
    0xF0,      // 0x2A   *: no Morse
    B00101010, // 0x2B   +
    B01110011, // 0x2C   ,
    B01100001, // 0x2D   -
    B01010101, // 0x2E   .
    B00110010, // 0x2F   /
    B00111111, // 0x30   0
    B00101111, // 0x31   1
    B00100111, // 0x32   2
    B00100011, // 0x33   3
    B00100001, // 0x34   4
    B00100000, // 0x35   5
    B00110000, // 0x36   6
    B00111000, // 0x37   7
    B00111100, // 0x38   8
    B00111110, // 0x39   9
    B01111000, // 0x3A   :
    B01101010, // 0x3B   ;
    B00110110, // 0x3C   < -> (
    B00110001, // 0x3D   =
    B01101101, // 0x3E   > -> )
    B01001100, // 0x3F   ?
    B01100101, // 0x40   @
    B00000101, // 0x41   A
    B00011000, // 0x42   B
    B00011010, // 0x43   C
    B00001100, // 0x44   D
    B00000010, // 0x45   E
    B00010010, // 0x46   F
    B00001110, // 0x47   G
    B00010000, // 0x48   H
    B00000100, // 0x49   I
    B00010111, // 0x4A   J
    B00001101, // 0x4B   K
    B00010100, // 0x4C   L
    B00000111, // 0x4D   M
    B00000110, // 0x4E   N
    B00001111, // 0x4F   O
    B00010110, // 0x50   P
    B00011101, // 0x51   Q
    B00001010, // 0x52   R
    B00001000, // 0x53   S
    B00000011, // 0x54   T
    B00001001, // 0x55   U
    B00010001, // 0x56   V
    B00001011, // 0x57   W
    B00011001, // 0x58   X
    B00011011, // 0x59   Y
    B00011100, // 0x5A   Z
    B00110110, // 0x5B   [ -> (
    B00110010, // 0x5C   \ -> /
    B01101101, // 0x5D   ] -> )
    0xF0,      // 0x5E   ^: No Morse
    B01001101, // 0x5F   _
    0x00       // fencepost
};



#ifdef MORSE_DEBUG
void MorseDump() {
  uint8_t retbuf;
  for (uint8_t i=0; i < 0xFF; i++) {
    Serial.print(i > 0x0F ? F("BM 0x") : F("BM 0x0"));    
    Serial.print(i, HEX);
    Serial.print(" = B");
    retbuf=pgm_read_byte_near(BittyMorse + i);
    Serial.println(retbuf, BIN);
    if (retbuf == 0x00) return;
  }
}
#endif

void BittyDit(boolean isDah=false) {
#ifdef MORSE_DEBUG
  Serial.print(isDah ? "-" : ".");
#endif
  digitalWrite(MORSE_PIN, MORSE_OUTPUT_ON);
  delay(isDah ? (MORSE_DELAY * 3) : MORSE_DELAY);
  digitalWrite(MORSE_PIN, MORSE_OUTPUT_OFF);
  delay(MORSE_DELAY);
}

uint8_t GetBittyWith(uint8_t mycode) {
  if (mycode > 0x7D) {
#ifdef MORSE_DEBUG
    Serial.print(F("No Morse code for 0x"));
    Serial.println(mycode, HEX);
#endif
    return(0xF0);
  }
  if (mycode >= 'a') {
#ifdef MORSE_DEBUG
 //   Serial.print(F("Shifting "));
 //   Serial.print(char(mycode));
 //   Serial.print(F(" -> "));
#endif
    mycode -= 0x20;         // Shift lowercase to upper
#ifdef MORSE_DEBUG
//    Serial.println(char(mycode));
#endif
  }
  return(pgm_read_byte_near(BittyMorse + mycode)); // return lookup value
}

void playMorse(uint8_t mybyte) { // actually put the Morse code on an output pin
  uint8_t bitmorse=GetBittyWith(mybyte);
#ifdef MORSE_DEBUG
  Serial.print(F("DEBUG playMorse \x22"));
  Serial.print(char(mybyte));
  Serial.print(F("\x22 = 0x"));
  Serial.print(mybyte, HEX);
  Serial.print(F(" -> B"));
  Serial.println(bitmorse, BIN);
#endif
  if (bitmorse == 0xFF) { // error prosign = eight audible components
    BittyDit(); BittyDit(); BittyDit(); BittyDit();
    BittyDit(); BittyDit(); BittyDit(); BittyDit();
#ifdef MORSE_DEBUG
    Serial.println(F(" Error Prosign"));
#endif
    delay(MORSE_DELAY * 2); // inter-character delay
    return;
  }
  if (bitmorse >= 0xF0) {
    uint16_t mydelay = (bitmorse & 0x0F) * MORSE_DELAY;
#ifdef MORSE_DEBUG
    Serial.print(F("Delay (ms) "));
    Serial.println(mydelay, DEC);
#endif
    delay(mydelay);
    return;
  }
  // fallthrough: play the pattern
#ifdef MORSE_DEBUG
  Serial.print(F("Morse playing B"));
  Serial.println((bitmorse), BIN);
#endif
  boolean output=false;
  if                 (bitmorse >= B10000000) output=true;
  if (output) BittyDit(bitmorse & B01000000);
  if                 (bitmorse >= B01000000) output=true;
  if (output) BittyDit(bitmorse & B00100000);
  if                 (bitmorse >= B00100000) output=true;
  if (output) BittyDit(bitmorse & B00010000);
  if                 (bitmorse >= B00010000) output=true;
  if (output) BittyDit(bitmorse & B00001000);
  if                 (bitmorse >= B00001000) output=true;
  if (output) BittyDit(bitmorse & B00000100);
  if                 (bitmorse >= B00000100) output=true;
  if (output) BittyDit(bitmorse & B00000010);
              BittyDit(bitmorse & B00000001);
  delay(MORSE_DELAY * 2); // inter-character delay
#ifdef MORSE_DEBUG
  Serial.println();
#endif
}

#endif // _BITTYMORSE_H_

