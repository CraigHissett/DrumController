/*Code to process MIDI commands using data received from SPI Slaves*/

#include <midi_serialization.h>
#include <usbmidi.h>
#include <Wire.h>

const int SLAVE_ADDRESS = 1; //Slave arduino ID
int table[]={0,0}; //the data will be transmited via table as to allow different data to be transfer.

void setup()
{
  Wire.begin();
  Serial.begin(9600); 
}

void loop()
{
    while (USBMIDI.available()) {
    // We must read entire available data, so in case we receive incoming
    // MIDI data, the host wouldn't get stuck.
    u8 b = USBMIDI.read();
}
  Wire.requestFrom(SLAVE_ADDRESS, 2);     // request 2 bytes from slave device #2
  for(int i=0;i<3;i++)//organizes the data from the slave in the table
  {
    int c = Wire.read(); // receive a byte as character
  table[i]=c;
  }
  Serial.println(table[0]);
  if(table[0] != 0){
    sendNote(0, table[0], table[1]);
  }
  // Flush the output.
  USBMIDI.flush();
  delay(100);
}

void sendNote(uint8_t channel, uint8_t note, uint8_t velocity) {
  USBMIDI.write((velocity != 0 ? 0x90 : 0x80) | (channel & 0xf));
  USBMIDI.write(note & 0x7f);
  USBMIDI.write(velocity &0x7f);
}
