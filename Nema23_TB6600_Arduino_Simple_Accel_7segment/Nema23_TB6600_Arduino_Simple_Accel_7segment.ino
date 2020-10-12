/*
TB6600－Arduino
https://www.makerguides.com/tb6600-stepper-motor-driver-arduino-tutorial/
http://makerhardware.net/wiki/doku.php?id=electronics:tb6600_stepper_motor_driver

circuit diagram(common-cathode_connection)
http://makerhardware.net/wiki/lib/exe/fetch.php?cache=&media=manuals:common-cathode_connection_01.png

AccelStepper　Library
https://www.airspayce.com/mikem/arduino/AccelStepper/classAccelStepper.html#a608b2395b64ac15451d16d0371fe13ce
 */

#include <AccelStepper.h>
#include <TM1637Display.h>

// Display_setting**********************************************************************
#define CLK 5
#define DIO 6
TM1637Display display(CLK, DIO);
uint8_t data[] = {0, 0, 0, 0};

// AccelSteppr_setting**********************************************************************
#define DirPin 2
#define StepPin 3
#define EnaPin 4
//////// MotorInterfaceType {  FUNCTION = 0, DRIVER = 1, FULL2WIRE = 2,
///FULL3WIRE = 3,FULL4WIRE = 4, HALF3WIRE = 6, HALF4WIRE = 8}
#define motorInterfaceType 1

//////// Create a new instance of the AccelStepper class:
AccelStepper stepper = AccelStepper(motorInterfaceType, StepPin, DirPin);
//////// set xxx step/rev
volatile long int rote = 1600; // 1/8microstep→1600step/rev

// Button_setting**********************************************************************
const int enter_pin = 11;
const int plus_pin = 12;
const int minus_pin = 13;

volatile long int count;        // rotation number
volatile long int i = 4;        // array index
volatile long int i_lim = 7;    // array index limit
uint8_t nejiri[] = {3, 4, 6, 8, 12, 16, 23, 33};// yori-number:JIS C 3216-5／巻線の電気特性

void setup() {
  Serial.begin(9600);
  stepper.setMaxSpeed(3000);    // Set maximum speed
  stepper.setAcceleration(500); // Set acceleration
  pinMode(EnaPin, OUTPUT);      // Fixing the motor shaft
  digitalWrite(EnaPin, HIGH);   // // HIGH:Shaft-Free

  pinMode(enter_pin, INPUT_PULLUP);
  pinMode(plus_pin, INPUT_PULLUP);
  pinMode(minus_pin, INPUT_PULLUP);

  //////// Set the number of revolutions. Execute when you press the enter button.
  while (digitalRead(enter_pin) != LOW) {

    if (digitalRead(plus_pin) == LOW) {
      i++;
      if (i >= i_lim + 1) {
        i = 0;
      }
      delay(200);
    }
    if (digitalRead(minus_pin) == LOW) {
      i--;
      if (i < 0) {
        i = i_lim;
      }
      delay(200);
    }
    
    count = nejiri[i];
    //////// Show on the display
    data[3] = display.encodeDigit(count / 1 % 10);
    data[2] = display.encodeDigit(count / 10 % 10);
    data[1] = display.encodeDigit(count / 100 % 10);
    data[0] = display.encodeDigit(count / 1000 % 10);

    display.setBrightness(7);//Brightness:0~7
    display.setSegments(data);
    delay(50);
    display.setBrightness(2);
    display.setSegments(data);
    delay(50);
  }
  display.setBrightness(7);
  display.setSegments(data);
}

//************************************************************************************/
void loop() {
  
  Serial.println("Start");

  stepper.setMaxSpeed(3000);
  stepper.setAcceleration(500);
  digitalWrite(EnaPin, LOW); // LOW:Shaft-Fixing

  stepper.moveTo(rote * count); // Set the target position: 1/8 microstep → 1600step/rev
  stepper.runToPosition(); // Run to target position

  delay(500);

  // stepper.moveTo(0); // Return to zero
  stepper.setCurrentPosition(0); // The current location is set to 0, and without this, it does not
          // move continuously = it is recognized that it is already in the
          // moveTo position.

  ///////////// Wait until the START button is pressed
  while (digitalRead(enter_pin) != LOW) {
    digitalWrite(EnaPin, HIGH); // HIGH:Shaft-Free
    delay(100);
    if (digitalRead(enter_pin) == LOW) {
      break;
    }
  }
  Serial.println("End");
}
