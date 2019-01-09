#include <AccelStepper.h>
#include <math.h>

//Stepper X, Y, Z are the same as the X, Y, Z on Protoneer shield
AccelStepper stepper0(AccelStepper::DRIVER, 2, 5); // pin 2 for step, pin 5 for direction
AccelStepper stepper1(AccelStepper::DRIVER, 3, 6);
AccelStepper stepper2(AccelStepper::DRIVER, 4, 7);

// For serial communication
byte buf[9];
byte targetX_H;
byte targetX_L;
byte targetX_dir; // direction should be either 1 for CCW or 3 for CW. This number will later be...
            // ...subtracted to 2, which then create -1 and 1. Remember, byte is an unsigned number.
byte targetY_H; 
byte targetY_L;
byte targetY_dir;

byte targetPhi_H;
byte targetPhi_L;
byte targetPhi_dir;

float targetX = 0;
float targetY = 0;
float targetPhi = 0;

// For calculating state of rover
float v0 = 0; // in cm/s
float v1 = 0;
float v2 = 0;

float v0s = 0; // in step/s
float v1s = 0;
float v2s = 0;
float largest_vel;

double v = 0;
double v_n = 0;
double w = 0;

double v_x = 0;
double v_y = 0;

float x = 0;
float y = 0;
float phi = 0;

float dx = 0;
float dy = 0;
float dphi = 0;

unsigned long start1 = 0;
unsigned long start2 = 0;

int stop_flag = 0;
int start_flag = 0;
int elapsedTime = 0;
float time_frame1 = 1;
float time_frame2 = 0.1;

int resolution = 8;
float rover_radius = 13.75; // in cm
float wheel_radius = 2.55; // in cm
float constant_linearVelocity = 300*((wheel_radius*PI)/(resolution*100)); // in cm/s
float constant_angularVelocity = PI/4.0*wheel_radius/rover_radius; // in rad/s
int threshold_velocity = 400;

float time_limit = 0.0;

int twobytes1int(byte high, byte low)
{
  int number;

  number = low | (high << 8);
  return number;
}

float largest_of_three(float first, float second, float third)
{
  float largest = first;
  if (second > largest)
  {
    largest = second;
  }

  if (third > largest)
  {
    largest = third;
  }

  return largest;
}

void setup()
{
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(5, OUTPUT);
   
  pinMode(3, OUTPUT);
  pinMode(6, OUTPUT);
   
  pinMode(4, OUTPUT);
  pinMode(7, OUTPUT);

  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);

  stepper0.setMaxSpeed(8000);
  stepper1.setMaxSpeed(8000);
  stepper2.setMaxSpeed(8000);

  stepper0.setSpeed(0);
  stepper1.setSpeed(0);
  stepper2.setSpeed(0);

  
  start2 = millis();
}


void loop()
{
  if (Serial.available())
  {
  	if (Serial.read() == 255)
    {
      Serial.readBytes(buf, 9);
      targetX_H = buf[0];
      targetX_L = buf[1];
      targetX_dir = buf[2];

      targetY_H = buf[3];
      targetY_L = buf[4];
      targetY_dir = buf[5];

      targetPhi_H = buf[6];
      targetPhi_L = buf[7];
      targetPhi_dir = buf[8];

      targetX = twobytes1int(targetX_H, targetX_L);
      targetY = twobytes1int(targetY_H, targetY_L);
      targetPhi= twobytes1int(targetPhi_H, targetPhi_L);

      targetX = targetX * (targetX_dir - 2);
      targetY = targetY * (targetY_dir - 2);
      targetPhi = targetPhi * (targetPhi_dir - 2);
      targetPhi = targetPhi/180.0*PI;

      stop_flag = 0;
    }

    if (Serial.read() == 254)
    {
      stop_flag = 1;
      stepper0.stop();
      stepper1.stop();
      stepper2.stop();

      x = 0;
      y = 0;
      phi = 0;
    }
  }

 if (((millis() - start1) > (time_frame1*1000)) && (stop_flag == 0) && (start_flag == 1)) // Update new speed after every time_frame1
 {
  start1 = millis(); // reset the timer

  // find linear and angular velocity v_x, v_y and w
  time_limit = sqrt(dx*dx + dy*dy)/constant_linearVelocity;

  if (time_limit == 0)
  {
    time_limit = 1;
  }

  v_x = dx/time_limit;
  v_y = dy/time_limit;
  w = dphi/time_limit;

  // find v, v_n
  v = v_x*cos(phi) + v_y*sin(phi);
  v_n = -v_x*sin(phi) + v_y*cos(phi);

  // find v0, v1, v2 (in cm/s)
  v0 = -v*sin(PI/3) + v_n*cos(PI/3) + w*rover_radius;
  v1 =              - v_n           + w*rover_radius;
  v2 = v*sin(PI/3)  + v_n*cos(PI/3) + w*rover_radius;

  // convert it back to step/s
  v0s = v0 * ((resolution*100)/(wheel_radius*PI));
  v1s = v1 * ((resolution*100)/(wheel_radius*PI));
  v2s = v2 * ((resolution*100)/(wheel_radius*PI));

  // scale down v0, v1, v2 if necessary
  if (v0s > threshold_velocity || v1s > threshold_velocity || v2s > threshold_velocity)
  {
    // find the largest velocity
    largest_vel = largest_of_three(v0s, v1s, v2s);

    // scale down
    v0s = v0s/largest_vel*threshold_velocity;
    v1s = v1s/largest_vel*threshold_velocity;
    v2s = v2s/largest_vel*threshold_velocity;
  }
  
  // set speed for each motor
  stepper0.setSpeed(v0s);
  stepper1.setSpeed(v1s);
  stepper2.setSpeed(v2s);

  // recalculate v_x, v_y, w
  w = 1/(3*rover_radius)*(v0 + v1 + v2);

  v = (sqrt(3)/3)*(v2-v0);
  v_n = 1/3.0*(v2+v0) - 2/3.0*v1;

  v_x = v*cos(phi) - v_n*sin(phi);
  v_y = v*sin(phi) + v_n*cos(phi);
  
 }

 if ((millis() - start2 > time_frame2*1000) && (stop_flag == 0)) // update coordinate every time_frame2
 {
  if (start_flag == 0)
  {
    start1 = millis();
  }
  start2 = millis(); // reset the timer
  start_flag = 1;
  
  // calculate x, y, phi
  x = x + v_x*time_frame2;
  y = y + v_y*time_frame2;
  phi = phi + w*time_frame2;

  // Calculate required displacement
  dx = targetX - x;
  dy = targetY - y;
  dphi = targetPhi - phi;

  // stopping condition
  if (abs(dx) < 0.5 && abs(dy) < 0.5 && abs(dphi) < 0.0175)
  {
    stop_flag = 1;
    stepper0.stop();
    stepper1.stop();
    stepper2.stop();
  }
 }

 if (stop_flag == 0)
 {
  stepper0.runSpeed();
  stepper1.runSpeed();
  stepper2.runSpeed();
 }


}
