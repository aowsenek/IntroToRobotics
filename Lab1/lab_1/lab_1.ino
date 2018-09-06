#include <sparki.h>

#define STATE_ROTATING 0
#define STATE_SEEKING_OBJECT 1
#define STATE_GRIPPING 2
#define STATE_SEEKING_LINE 3
#define STATE_SEEKING_START 4
#define STATE_DROPPING 5
#define STATE_STOPPED 6

#define GRIPPER_TOGGLE_TIME 5000

// Set up some global variables with default values to be replaced during operation
int current_state = STATE_ROTATING;
const int threshold = 700; // IR reading threshold to detect whether there's a black line under the sensor
int cm_distance = 1000;
int line_left = 1000;
int line_center = 1000;
int line_right = 1000;


void setup() {
  // put your setup code here, to run once:
  sparki.RGB(RGB_RED); // Turn on the red LED
  sparki.servo(SERVO_CENTER); // Center the ultrasonic sensor
  delay(1000); // Give the motor time to turn
  sparki.gripperOpen(); // Open the gripper
  delay(GRIPPER_TOGGLE_TIME); // Give the motor time to open the griper
  sparki.gripperStop(); // 5 seconds should be long enough
  sparki.RGB(RGB_GREEN); // Change LED to green so we know the robot's setup is done!
}

void readSensors() {
  cm_distance = sparki.ping(); // Replace with code to read the distance sensor
  line_left = sparki.lineLeft(); // Replace with code to read the left IR sensor
  line_center = sparki.lineCenter(); // Replace with code to read the center IR sensor
  line_right = sparki.lineRight(); // Replace with code to read the right IR sensor
}

void loop() {
  // put your main code here, to run repeatedly:
  readSensors(); // Read sensors once per loop() call

  sparki.clearLCD();
  sparki.print("STATE: ");
  sparki.println(current_state);

  // Your state machine code goes here
  switch (current_state) {
    case STATE_ROTATING:
      if(cm_distance < 30 && cm_distance != -1) {
        current_state = STATE_SEEKING_OBJECT;
      } else {
        sparki.moveRight(30);
      }
      break;
    case STATE_SEEKING_OBJECT:
      if(cm_distance < 7 && cm_distance != -1) {
        sparki.moveForward(3);
        current_state = STATE_GRIPPING;

      } else {
        sparki.moveForward(2);
      }
      break;
    case STATE_GRIPPING:
      sparki.gripperClose();
      delay(GRIPPER_TOGGLE_TIME);
      sparki.gripperStop();
      sparki.moveRight(180);
      current_state = STATE_SEEKING_LINE;
      break;
    case STATE_SEEKING_LINE:
      if (line_left < threshold) {
        sparki.moveLeft();
      }
      else if(line_right < threshold) {
        sparki.moveRight();
      }
      if (line_center > threshold) {
        sparki.moveForward();
      }

      if (line_center < threshold && line_left > threshold && line_right > threshold) {
        current_state = STATE_SEEKING_START;
      }
      break;
    case STATE_SEEKING_START:
      if(line_right < threshold && line_left < threshold && line_right < threshold) {
        sparki.moveStop();
        sparki.beep();
        current_state = STATE_DROPPING;
      }
      else{
        if (line_left < threshold) {
            sparki.moveLeft();
          }
        if(line_right < threshold) {
          sparki.moveRight();
          }
        if (line_center < threshold && line_right > threshold && line_left > threshold) {
          sparki.moveForward();
          }
      }


      break;
    case STATE_DROPPING:
      sparki.gripperOpen();
      delay(GRIPPER_TOGGLE_TIME); // Give the motor time to open the griper
      sparki.gripperStop(); // 5 seconds should be long enough
      sparki.beep();
      current_state = STATE_STOPPED;
      break;
    case STATE_STOPPED:
      sparki.moveStop();
      delay(10000);
      break;

      

  }

  sparki.updateLCD();
  delay(100); // Only run controller at 10Hz
}

