/*
 * MIDI Drum Kit
 * -------------
 * Convert Arduino to a MIDI controller using various inputs and
 * the serial port as a MIDI output.
 *
 * This sketch is set up to send General MIDI (GM) drum notes 
 * on MIDI channel 1, but it can be easily reconfigured for other
 * notes and channels
 *
 * It uses switch inputs to send MIDI notes of a fixed velocity with
 * note on time determined by duration of keypress and it uses
 * piezo buzzer elements as inputs to send MIDI notes of a varying velocity
 * & duration, depending on forced of impulse imparted to piezo sensor.
 *
 * To send MIDI, attach a MIDI out jack (female DIN-5) to Arduino.
 * DIN-5 pinout is:                               _____ 
 *    pin 2 - Gnd                                /     \
 *    pin 4 - 220 ohm resistor to +5V           | 3   1 |  MIDI jack
 *    pin 5 - Arduino D1 (TX)                   |  5 4  |
 *    all other pins - unconnected               \__2__/
 * On my midi jack, the color of the wires for the pins are:
 *   3 = n/c 
 *   5 = black  (blue)
 *   2 = red    (red)
 *   4 = orange (yellow)
 *   1 = brown
 *
 * Based off of Tom Igoe's work at:
 *    http://itp.nyu.edu/physcomp/Labs/MIDIOutput
 *
 * Created 25 October 2006
 * copyleft 2006 Tod E. Kurt <tod@todbot.com
 * http://todbot.com/
 *
 * Updates:
 * - 2 May 2009 - fixed noteOn() and noteOff() 
 *
 */

// what midi channel we're sending on
// ranges from 0-15
#define drumchan           1

// general midi drum notes
#define note_bassdrum     35
#define note_snaredrum    38
#define note_hihatclosed  42
#define note_hihatopen    44
#define note_crash        49

// define the pins we use
#define switchAPin 7
#define switchBPin 6
#define switchCPin 5
#define piezoAPin  0
#define piezoBPin  1
#define ledPin     13  // for midi out status

// analog threshold for piezo sensing
#define PIEZOTHRESHOLD 100

int switchAState = LOW;
int switchBState = LOW;
int switchCState = LOW;
int currentSwitchState = LOW;

int val,t;

void setup() {
  pinMode(switchAPin, INPUT);
  pinMode(switchBPin, INPUT);
  pinMode(switchCPin, INPUT);
  digitalWrite(switchAPin, HIGH);  // turn on internal pullup
  digitalWrite(switchBPin, HIGH);  // turn on internal pullup
  digitalWrite(switchCPin, HIGH);  // turn on internal pullup

  pinMode(ledPin, OUTPUT);
  Serial.begin(31250);   // set MIDI baud rate
}

void loop() {
  // deal with switchA
  currentSwitchState = digitalRead(switchAPin);
  if( currentSwitchState == LOW && switchAState == HIGH ) // push
    noteOn(drumchan,  note_bassdrum, 100);
  if( currentSwitchState == HIGH && switchAState == LOW ) // release
    noteOff(drumchan, note_bassdrum, 0);
  switchAState = currentSwitchState;

  // deal with switchB
  currentSwitchState = digitalRead(switchBPin);
  if( currentSwitchState == LOW && switchBState == HIGH ) // push
    noteOn(drumchan,  note_snaredrum, 100);
  if( currentSwitchState == HIGH && switchBState == LOW ) // release
    noteOff(drumchan, note_snaredrum, 0);
  switchBState = currentSwitchState;

  // deal with switchC
  currentSwitchState = digitalRead(switchCPin);
  if( currentSwitchState == LOW && switchCState == HIGH ) // push
    noteOn(drumchan,  note_hihatclosed, 100);
  if( currentSwitchState == HIGH && switchCState == LOW ) // release
    noteOff(drumchan, note_hihatclosed, 0);
  switchCState = currentSwitchState;

  // deal with first piezo, this is kind of a hack
  val = analogRead(piezoAPin);
  if( val >= PIEZOTHRESHOLD ) {
    t=0;
    while(analogRead(piezoAPin) >= PIEZOTHRESHOLD/2) {
      t++;
    }
    noteOn(drumchan,note_hihatopen, t*2);
    delay(t);
    noteOff(drumchan,note_hihatopen,0);
  }

  // deal with second piezos, this is kind of a hack
  val = analogRead(piezoBPin);
  if( val >= PIEZOTHRESHOLD ) {
    t=0;
    while(analogRead(piezoBPin) >= PIEZOTHRESHOLD/2) {
      t++;
    }
    noteOn(drumchan,note_crash, t*2);
    delay(t);
    noteOff(drumchan,note_crash,0);
  }
}

// Send a MIDI note-on message.  Like pressing a piano key
// channel ranges from 0-15
void noteOn(byte channel, byte note, byte velocity) {
  midiMsg( (0x90 | channel), note, velocity);
}

// Send a MIDI note-off message.  Like releasing a piano key
void noteOff(byte channel, byte note, byte velocity) {
  midiMsg( (0x80 | channel), note, velocity);
}

// Send a general MIDI message
void midiMsg(byte cmd, byte data1, byte data2) {
  digitalWrite(ledPin,HIGH);  // indicate we're sending MIDI data
  Serial.print(cmd, BYTE);
  Serial.print(data1, BYTE);
  Serial.print(data2, BYTE);
  digitalWrite(ledPin,LOW);
}
