// U/S sensor pins 
const int trigPin = 10;
const int frontEchoPin = 9;
const int leftEchoPin = 11;
const int rightEchoPin = 12;

// Motor control pins
const int M1forward = 5;
const int M1reverse = 4;
const int M2forward = 6;
const int M2reverse = 7;
const int M1pwm = 2;
const int M2pwm = 3;

// speed control (for PWM)
const int FAST = 255;
const int SLOW = 175;
const int STOP = 0;

// obstacle distance threshold
const int DIST = 30;

// turn delay
const int TDELAY = 5000;

// 1 enables debug logging
const int DEBUG = 0;

// to enable looping mode - distance calibration
const int LOOP = 0;

// debug led pin
const int ledPin = 8;

// debug routine
void debug(char *msg);

// global variables
int command = 0;
char cmd;
char jsonMsg[22] = "";
int respCmd = 0;
int distFront = 0;
float distRight = 0;
float distLeft = 0;
int rpiResponse=0;
boolean startFlag = false;
boolean compFlag = false;

// global function declarations
void move_forward(void);
void turn_right(int);
void turn_left(int);
void reverse(int);
void stop_full(void);
int measure_alignment(void);
int measure_front_distance(void);
float measure_right_distance(void);
float measure_left_distance(void);

void flash_led(int count);
boolean no_front_obs(void);
void send_rpi(int);
int get_response(void);
void reposition(void);
void fix_alignment(void);


void setup() {                
  // initialize U/S sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(frontEchoPin, INPUT);
  pinMode(leftEchoPin, INPUT);
  pinMode(rightEchoPin, INPUT);  

  // initiatlize motor control pins
  pinMode(M1forward, OUTPUT);
  pinMode(M1reverse, OUTPUT);
  pinMode(M2forward, OUTPUT);
  pinMode(M2reverse, OUTPUT);
  pinMode(M1pwm, OUTPUT);
  pinMode(M2pwm, OUTPUT);

  pinMode(ledPin, OUTPUT);

  Serial.begin(9600);

  debug("Initialization complete"); 
}

void loop() {
	debug("Loop beginning");
        while(LOOP) {
	  distFront = measure_front_distance();
          if (DEBUG) {
            Serial.print("Received distance = ");
            Serial.print(distFront);
            Serial.print("\n");
          }
          distRight = (float)measure_right_distance();
          if (DEBUG) {
            Serial.print("Received right distance = ");
            Serial.print(distRight);
            Serial.print("\n");
          }
          distLeft = (float)measure_left_distance();
          if (DEBUG) {
            Serial.print("Received left distance = ");
            Serial.print(distLeft);
            Serial.print("\n");
          }
          delay(3000);
        }
        // Wait for initiation from RasPi
        /*while (startFlag == false) {
          rpiResponse = get_response();
          if (rpiResponse == 9) startFlag = true;
        }*/
        
        distFront = measure_front_distance();
        if (DEBUG) {
          Serial.print("Received distance = ");
          Serial.print(distFront);
          Serial.print("\n");
        }
	if (distFront > 6) {
          // if destination is far, keep moving forward
          move_forward();
        }
        else if ((distFront <= 6) && (distFront > 2)) {
	  // ask RPi for visual navigation
		send_rpi(distFront);
		rpiResponse = get_response();
		if (rpiResponse == 1) {
			turn_left(0);
		} else if (rpiResponse == 2) {
			turn_right(0);
		} else if (rpiResponse == 3) {
			turn_left(1);
		} else if (rpiResponse == 4) {
			turn_right(1);
		} else if (rpiResponse == 5) {
                        turn_left(3);
                } else if (rpiResponse == 6) {
                        turn_right(3);
                } else if (rpiResponse == 7) {
			move_forward();
		} else if (rpiResponse == 8) {
                      reverse(2);
                }
         } 
         else if (distFront <= 2) {
           if (distFront == 1) reverse(2);
           else {
             send_rpi(0);
             while(compFlag == false) {
                rpiResponse = get_response();
                 if (rpiResponse == 1) {
	             turn_left(0);
                    compFlag = true;
	     } 
             else if (rpiResponse == 2) {
	            turn_right(0);
                    compFlag = true;
	     } 
             else if (rpiResponse == 8) {
                    reverse(1);
                    compFlag = true;
             } 
             else if (rpiResponse == 9) {
                    reverse(2);
		    // keep waiting for further instructions - this was done for VR; maybe not needed anymore
                    compFlag = false;
             } 
             else if (rpiResponse == 5) {
		    turn_left(3);
		    compFlag = true;
             } 
             else if (rpiResponse == 6) {
		    turn_right(3);
		    compFlag = true;
             }
           }  
           // reset completion flag
           compFlag = false;
          }
      }
      delay(100);
} // end of loop

// Prepare JSON message and send to RPi
void send_rpi(int stat) 
{
    sprintf(jsonMsg, "%d\n", stat);
    Serial.print(jsonMsg);
    Serial.flush();
}

// Move 1 feet forward while keeping a tab on front obstacle
// Objective is to hold the line while covering the required distance
// Algorithm is heavily calibrated through actual trial and error
// Maze path should be close to 12 inches (1 foot) wide for accurate navigation

void move_forward (void)
{
	int currentDistance = 0;
	int newDistance = 0;
        float distRight=0;
        float distLeft = 0;
        float totalDist = 0;
        float ratio = 0;
	currentDistance = measure_front_distance();
	if (currentDistance > 2) {
            	digitalWrite(M1forward, HIGH);
		digitalWrite(M1reverse, LOW);
		digitalWrite(M2forward, HIGH);
		digitalWrite(M2reverse, LOW);
		// if you are close to target (1.5 feet); slow down
                if (currentDistance <= 3) {
                  // move in small quantums
                  analogWrite(M1pwm, FAST);
		  analogWrite(M2pwm, FAST);
                  delay(TDELAY/5);
                
                } 
                else { 
                  analogWrite(M1pwm, FAST);
		  analogWrite(M2pwm, FAST);
		  newDistance = measure_front_distance();
		  while((currentDistance - newDistance) == 0) {
                        // we will keep having these small delays throughout
                        // needed for ultrasound sensor accuracy
                        // prevent spurious readings due to echoes
                        //delay(100);
                        distRight=(float)measure_right_distance();
                        distLeft=(float)measure_left_distance();
                        
                        // below two parameters are critical for navigation
                        ratio = distRight/distLeft;
                        totalDist = distRight + distLeft;
                        if(DEBUG) {
                          Serial.print("Right:Left ratio is ");
                          Serial.print(ratio);
                          Serial.print("\n");
                          Serial.print("Sum of distance is ");
                          Serial.print(totalDist);
                          Serial.print("\n");
                        }
                        if ((ratio > 1.25) && (ratio <= 2.6)) {
                          turn_right(1);
                          analogWrite(M1pwm, FAST);
		          analogWrite(M2pwm, FAST);
                        }
                        else if ((ratio > 2.6) && (ratio <= 3.8)) {
                          // reverse and correct only misaligned
                          if ((totalDist > 10) && (totalDist < 15)) {
                            reverse(1);
                          }
                          turn_right(2);
                          analogWrite(M1pwm, FAST);
		          analogWrite(M2pwm, FAST);
                        }
                        else if ((ratio > 3.8) && (ratio <= 6)) {
                          // too close; and mostly struck at an angle; reverse and turn
                          if ((totalDist > 10) && (totalDist < 20)) {
                            reverse(1);
                          }
                          turn_right(2);
                          analogWrite(M1pwm, FAST);
		          analogWrite(M2pwm, FAST);
                        }
                        else if ((ratio <= 0.9) && (ratio > 0.6)) {
                          turn_left(1);
                          analogWrite(M1pwm, FAST);
		          analogWrite(M2pwm, FAST);
                        }
                        else if ((ratio <=0.6) && (ratio > 0.4)) {
                          if ((totalDist > 10) && (totalDist < 20)) {
                            reverse(1);
                          }
                          turn_left(2);
                          analogWrite(M1pwm, FAST);
		          analogWrite(M2pwm, FAST);
                        }
                        else if ((ratio <=0.4) && (ratio > 0.1)) {
                          // too close; mostly struck at an angle; reverse and turn
                          if ((totalDist > 10) && (totalDist < 15)) {
                            reverse(1);
                          }
                          turn_left(2);
                          analogWrite(M1pwm, FAST);
		          analogWrite(M2pwm, FAST);
                        }
			delay(900);
			newDistance = measure_front_distance();
			if (newDistance == 2) break;
		  } // end of while
              } // end of else
              // stop after moving the planned distance
              analogWrite(M1pwm, STOP);
	      analogWrite(M2pwm, STOP);
	} // end of main if
}

void turn_left(int mode)
{
        float rightDist = 0;
	digitalWrite(M1forward, HIGH);
	digitalWrite(M1reverse, LOW);
	digitalWrite(M2forward, HIGH);
	digitalWrite(M2reverse, LOW);
	if (mode == 0) {
		// turn 90 degrees
	        rightDist = (float)measure_right_distance();
                while (rightDist < 3) {
                  // Not enough space to turn left
                  // Need to back up a bit and shift right
                  // create at least 3 inches space to execute turn
                  reverse(1);
                  digitalWrite(M1forward, HIGH);
	          digitalWrite(M1reverse, LOW);
	          digitalWrite(M2forward, HIGH);
	          digitalWrite(M2reverse, LOW);
                  analogWrite(M1pwm, STOP);
		  analogWrite(M2pwm, FAST);
                  delay(TDELAY/10);
                  analogWrite(M1pwm, FAST);
		  analogWrite(M2pwm, STOP);
                  delay(TDELAY/10);
                  rightDist = (float)measure_right_distance();
                }
                analogWrite(M1pwm, STOP);
		analogWrite(M2pwm, FAST);
		// fine tune this delay eventually for accurate 90 degree turn
		delay((TDELAY/4));
		analogWrite(M2pwm, STOP);
	} 
        else if (mode == 1) {
		// hunt mode; turn a bit
		analogWrite(M1pwm, STOP);
		analogWrite(M2pwm, SLOW);
		// fine tune this delay eventually for required accuracy
		delay((TDELAY/15));
		analogWrite(M2pwm, STOP);
	} 
        else if (mode == 2) {
                // hunt mode; turn a bit
		analogWrite(M1pwm, STOP);
		analogWrite(M2pwm, SLOW);
		// fine tune this delay eventually for required accuracy
		delay((TDELAY/10));
		analogWrite(M2pwm, STOP);
        }
        else if (mode == 3) {
                // hunt mode; turn a bit
		analogWrite(M1pwm, STOP);
		analogWrite(M2pwm, SLOW);
		// fine tune this delay eventually for required accuracy
		delay((TDELAY/15));
		analogWrite(M2pwm, STOP);
        }
}

void turn_right(int mode)
{
	float leftDist = 0;
        digitalWrite(M1forward, HIGH);
	digitalWrite(M1reverse, LOW);
	digitalWrite(M2forward, HIGH);
	digitalWrite(M2reverse, LOW);
	if (mode == 0) {
                leftDist = (float)measure_left_distance();
                while (leftDist < 3) {
                  // Not enough space to turn left
                  // Need to back up a bit and shift right - make up some space
                  reverse(1);
                  digitalWrite(M1forward, HIGH);
	          digitalWrite(M1reverse, LOW);
	          digitalWrite(M2forward, HIGH);
	          digitalWrite(M2reverse, LOW);
                  analogWrite(M1pwm, FAST);
		  analogWrite(M2pwm, STOP);
                  delay(TDELAY/10);
                  analogWrite(M1pwm, STOP);
		  analogWrite(M2pwm, FAST);
                  delay(TDELAY/10);
                  leftDist = (float)measure_left_distance();
                }
		analogWrite(M1pwm, FAST);
		analogWrite(M2pwm, STOP);
		// fine tune this delay eventually for accurate 90 degree turn
		delay(TDELAY/4);
		analogWrite(M1pwm, STOP);
	} else if (mode == 1) {
		// hunt mode; turn a bit 
		analogWrite(M1pwm, SLOW);
		analogWrite(M2pwm, STOP);
		// fine tune this delay eventually for accurate required accuracy
		delay(TDELAY/22);
		analogWrite(M1pwm, STOP);
	} else if (mode == 2) {
                // hunt mode; turn a bit more and straighten 
		analogWrite(M1pwm, SLOW);
		analogWrite(M2pwm, STOP);
		// fine tune this delay eventually for accurate required accuracy
		delay(TDELAY/10);
                analogWrite(M1pwm, STOP);
		analogWrite(M2pwm, SLOW);
                delay(TDELAY/10);
		analogWrite(M2pwm, STOP);
        } else if (mode == 3) {
                // hunt mode; turn a bit 
		analogWrite(M1pwm, SLOW);
		analogWrite(M2pwm, STOP);
		// fine tune this delay eventually for accurate required accuracy
		delay(TDELAY/29);
		analogWrite(M1pwm, STOP);
        }
}

void reverse(int mode)
{
	digitalWrite(M1forward, LOW);
	digitalWrite(M1reverse, HIGH);
	digitalWrite(M2forward, LOW);
	digitalWrite(M2reverse, HIGH);
	analogWrite(M1pwm, FAST);
	analogWrite(M2pwm, FAST);
	// fine tune this delay as we progress
        if (mode == 1) {
	  delay(TDELAY/5);
        }
        else if (mode == 2) {
          delay(TDELAY/10);
        }
	analogWrite(M1pwm, STOP);
	analogWrite(M2pwm, STOP);
}

void stop_full(void)
{
  digitalWrite(M1forward, HIGH);
  digitalWrite(M1reverse, LOW);
  digitalWrite(M2forward, HIGH);
  digitalWrite(M2reverse, LOW);
  analogWrite(M1pwm, STOP);
  analogWrite(M2pwm, STOP);
}

void debug(char *msg)
{
  if (DEBUG == 1) {
    Serial.print(msg);
    Serial.print('\n');
  }
}

// measures front distance from obstacle in feet
// does a rounding and returns integer as it simplifies RasPi communication
// the returned value will have to be divided by 2 to get the actual distance
int measure_front_distance(void)
{
	float frontObsDist = 0;
        delay(100);
	// send trigger to U/S distance sensor and measure distance
	digitalWrite(trigPin, LOW);
	delayMicroseconds(20);
	digitalWrite(trigPin, HIGH);
	delayMicroseconds(20);
	digitalWrite(trigPin, LOW);
	frontObsDist = ((float)pulseIn(frontEchoPin, HIGH))/(74.0*24.0);
	if (DEBUG) {
          Serial.print("Front = ");
          Serial.print(frontObsDist);
          Serial.print("\n");
	}
   	// rounding logic - this may have to be fine tuned as we move forward
	if (frontObsDist >= 4.25) return(9);
	else if ((frontObsDist >= 3.75) && (frontObsDist < 4.25)) return(8);
        else if ((frontObsDist >=3.25) && (frontObsDist < 3.75)) return(7);
        else if ((frontObsDist >=2.75) && (frontObsDist < 3.25)) return(6);
        else if ((frontObsDist >= 2.25) && (frontObsDist < 2.75)) return(5);
        else if ((frontObsDist >= 1.75) && (frontObsDist <2.25)) return(4);
	else if ((frontObsDist >= 1.25) && (frontObsDist < 1.75)) return(3);
	else if ((frontObsDist >= 1.0) && (frontObsDist < 1.25)) return(2);
	else if (frontObsDist < 1.0) return(1);
}

float measure_right_distance(void)
{
	float rightObsDist = 0;
        // wait for other echos to die off
        delay(100);
	// send trigger to U/S distance sensor and measure distance
        digitalWrite(trigPin, LOW);
	delayMicroseconds(20);
	digitalWrite(trigPin, HIGH);
	delayMicroseconds(20);
	digitalWrite(trigPin, LOW);
	rightObsDist = ((float)pulseIn(rightEchoPin, HIGH))/(74.0*2.0);
	if (DEBUG) {
          Serial.print("Right = ");
          Serial.print(rightObsDist);
          Serial.print("\n");
	}
        return(rightObsDist);
}

float measure_left_distance(void)
{
	float leftObsDist = 0;
        // wait for other echos to die off
        delay(100);
	// send trigger to U/S distance sensor and measure distance
        digitalWrite(trigPin, LOW);
	delayMicroseconds(20);
	digitalWrite(trigPin, HIGH);
	delayMicroseconds(20);
	digitalWrite(trigPin, LOW);
	leftObsDist = ((float)pulseIn(leftEchoPin, HIGH)/74.0)/2.0;
	if (DEBUG) {
          Serial.print("Left = ");
          Serial.print(leftObsDist);
          Serial.print("\n");
	}
        return(leftObsDist);
}

void switch_led(void)
{
	digitalWrite(ledPin, HIGH);
	delay(1000);
	digitalWrite(ledPin, LOW);
}

// wait till we get some response from RasPi
// response assumed to be of one byte (char) length

int get_response(void)
{
	boolean gotResp = false;

	while (gotResp == false)
	{
		// wait and get the response command from RPi
		if (Serial.available() > 0) {
			// Any response from RPi has to come as a single number typecast as character
                        Serial.readBytes(&cmd, 1);
			command = atoi(&cmd);
			gotResp = true;
		}
		else delay(100);
	}
	return(command);
}
