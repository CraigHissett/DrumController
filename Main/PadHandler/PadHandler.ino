/*Code to read data from analog pins and send via SPI*/

#include <midi_serialization.h>
#include <usbmidi.h>
#include <Wire.h>

// general midi drum notes
#define note_bassdrum     35
#define note_snaredrum    38
#define note_hihatclosed  42
#define note_hihatopen    44
#define note_crash        49

const int SLAVE_ADDRESS = 1; //Slave arduino ID
int table[]={0,0};

void requestEvent()
{
  uint8_t Buffer[2];
  Buffer[0]=table[0];
  Buffer[1]=table[1];
  //Serial.println("requested: ");
  Wire.write(Buffer,2);
}

const int ANALOG_PIN_COUNT = 16;
const int BUTTON_PIN_COUNT = 5;

// Change the order of the pins to change the ctrl or note order.
int analogPins[ANALOG_PIN_COUNT] = { A15, A14, A13, A12, A11, A10, A9, A8, A7, A6, A5, A4, A3, A2, A1, A0 };
//int buttonPins[BUTTON_PIN_COUNT] = { 9, 8, 7, 6, 5 };
int DrumNote[ANALOG_PIN_COUNT] = { note_bassdrum, note_snaredrum, note_hihatclosed, note_hihatopen, note_crash, note_bassdrum, note_snaredrum, note_hihatclosed, note_hihatopen, note_crash, note_bassdrum, note_snaredrum, note_hihatclosed, note_hihatopen, note_crash, note_bassdrum };

int ccValues[ANALOG_PIN_COUNT];
//int buttonDown[BUTTON_PIN_COUNT];

int readCC(int pin) {
  // Convert from 10bit value to 7bit.
  return analogRead(pin) >> 3;
}

void setup() {
Wire.begin(SLAVE_ADDRESS);      // join i2c bus with address #1
Wire.onRequest(requestEvent);   // register event
Serial.begin(9600);

  // Initialize initial values.
  for (int i=0; i<ANALOG_PIN_COUNT; ++i) {
    pinMode(analogPins[i], INPUT);
    ccValues[i] = readCC(analogPins[i]);
  }
}

void loop() {
  for (int i=0; i<ANALOG_PIN_COUNT; ++i) {
    int value = readCC(analogPins[i]);

    // Send CC only if th has changed.
    if (ccValues[i] != value) {
      //sendCC(0, i, value);
      //sendNote(0, DrumNote[i], value);
      table[0]=DrumNote[i];
      table[1]=value;
      Serial.println(table[0]);
      ccValues[i] = value;
    }
  }
  delay(100);
}
