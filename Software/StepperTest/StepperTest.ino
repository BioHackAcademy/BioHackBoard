/*
  This file adds on to the Waag Society's BioHack Academy Code,
  and interfaces with the BioHack Board.

 The code demonstrates how to use the AccelStepper library to drive
 the stepper motor.

 This sketch makes use of the serial port connection to observe the result.
 In case you are unfamiliar with serial port connections check this 
 tutorial: https://www.arduino.cc/en/Tutorial/AnalogReadSerial
 Or have a look at the BHA_SerialCommunication demonstration file
 */

/*
 * INCLUDE LIBRIARIES
 */

/*
Download the AccelStepper library from
http://www.airspayce.com/mikem/arduino/AccelStepper/
There should be a .zip download of the latest version
Install in menus: Sketch/Include Library/Add .ZIP Library
*/
#include <AccelStepper.h>

/*
 * DECLARE VARIABLES AND OBJECTS
 */

// Declare stepper driver pins
const byte PIN_STEP=7;
const byte PIN_DIR=8;
const byte PIN_ENABLE=4;

// Stepper hardware enable toggle
byte stepper_en = 0;

// Declares 'stepper' as an AccelStepper class
// AccelStepper::DRIVER constructs the class to work with a 2 pin 
// contoller: with a step and direction pins.
AccelStepper stepper(AccelStepper::DRIVER,PIN_STEP,PIN_DIR);

// Declare stepper commands
bool runToPos = false;
bool runConst = false;

// These strings hold the serial input and an Int parser, respectively
// For serial interface
String buffer;
String intbuffer;

/*
 * DEFINE LOCAL FUNTIONS
 */

// Handle serial IO
void serial() {
  // Wait for a serial command
  while (Serial.available()>0) {
    char c = (char)Serial.read();
    if (c == '\n') {
      if(buffer.startsWith("id")) {
        Serial.println("id:stepper-test"); // so other devices can figure out what is connected 
      }
      else if(buffer.startsWith("help")) {
        // Help function
        // Display action
        Serial.println("AVAILABLE COMMANDS:");
        Serial.println("id : device id");
        Serial.println("stop : stop stepper");
        Serial.println("moveto <INT> : moves to specified position");
        Serial.println("runspeed <INT> : runs forever at a constant speed");
        Serial.println("accel <INT> : sets acceleration");
        Serial.println("maxspeed <INT> : sets max speed");
        Serial.println("help : displays this menu again");
      } 
      else if(buffer.startsWith("stop")) {
        // Stops stepper
        runToPos = false;
        runConst = false;
        stepper.stop(); //hard stop
        // Clear target position
        int pos = stepper.currentPosition();
        stepper.moveTo(pos);
        // Display action
        Serial.println("Stepper stopped at position:");
        Serial.println((int)pos);
      } 
      else if(buffer.startsWith("runspeed")) {
        // Runs at a constant speed forever
        intbuffer = buffer;
        intbuffer.remove(0,9);
        int spd = intbuffer.toInt();
        stepper.setSpeed(spd);
        runToPos = false;
        runConst = true;
        // Display action
        Serial.println("Running at speed: ");
        Serial.println((int)spd);
      }
      else if(buffer.startsWith("moveto")) {
        // Moves to a relative position
        intbuffer = buffer;
        intbuffer.remove(0,7);
        int dest = intbuffer.toInt();
        stepper.moveTo(dest);
        runToPos = true;
        runConst = false;
        // Display action
        Serial.println("Moving to: ");
        Serial.println((int)dest);
      }
      else if(buffer.startsWith("accel")) {
        // Set acceleration
        intbuffer = buffer;
        intbuffer.remove(0,6);
        int acc = intbuffer.toInt();
        stepper.setAcceleration(acc);
        // Display action
        Serial.println("Accel: ");
        Serial.println((int)acc);
      }
      else if(buffer.startsWith("maxspeed")) {
        // Set max speed
        intbuffer = buffer;
        intbuffer.remove(0,9);
        int mspd = intbuffer.toInt();
        stepper.setMaxSpeed(mspd);
        // Display action
        Serial.println("Max Speed: ");
        Serial.println((int)mspd);
      }
      else if(buffer.startsWith("pos")) {
        int pos = stepper.currentPosition();
        // Display action
        Serial.println("Position: ");
        Serial.println((int)pos);
      }
      else {
        Serial.println("Unknown cmd.");
      }
      buffer="";
    } else buffer+=c;
  }
}

/*
 * SETUP FUNCTION
 */

// Required by Arduino; runs once at reset
void setup() {
  // Open serial connection and print a message
  Serial.begin(9600);
  Serial.println(F("BioHack Board Stepper Test"));
  Serial.println("AVAILABLE COMMANDS:");
  Serial.println("id : device id");
  Serial.println("stop : stop stepper");
  Serial.println("moveto <INT> : moves to specified position");
  Serial.println("runspeed <INT> : runs forever at a constant speed");
  Serial.println("accel <INT> : sets acceleration");
  Serial.println("maxspeed <INT> : sets max speed");
  Serial.println("help : displays this menu again");

  // Setup stepper
  stepper.setMaxSpeed(2000);
  stepper.setAcceleration(100);
}

/*
 * LOOP FUNCTION
 */

void loop() {

  // stepper.run(Speed) should be run as frequently as possible in a loop
  // it will step the stepper if a step is queued at the time.
  if (runToPos) {
    // if a step is queued; steps
    // runs only to desired position
    stepper.run();
  }
  else if (runConst) {
    // runs at a constant speed
    stepper.runSpeed();
  }

  serial();

}


