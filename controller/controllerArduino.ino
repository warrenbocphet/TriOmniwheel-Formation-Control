///////////////////////////// Library /////////////////////////////
#include <Encoder.h>

///////////////////////////// DC motors /////////////////////////////
const int int1 = A0; // motor 0
const int int2 = A1;
const int enA = 2;

const int int3 = A2; // motor 1
const int int4 = A3;
const int enB = 3;

const int int5 = A4; // motor 2
const int int6 = A5;
const int enC = 4;

float rover_radius = 13.7; // in cm
float wheel_radius = 5.22; // in cm

int constant_linearVelocity = 1*(2*PI*wheel_radius);

///////////////////////////// Encoder /////////////////////////////
Encoder enc0(0, 1);
Encoder enc1(6, 7);
Encoder enc2(8, 9);

long posPrevious0 = 0;
long posPrevious1 = 0;
long posPrevious2 = 0;

float displacement0 = 0;
float displacement1 = 0;
float displacement2 = 0;

int countPerRotation = 2840;

////////////////// For keeping track of time ////////////////
unsigned long start0 = 0;
unsigned int elapsedTime0 = 0; // use as a timer
unsigned long start1 = 0;
unsigned int elapsedTime1 = 0; // use with time_frame0
unsigned long start2 = 0;
unsigned int elapsedTime2 = 0; // use to do displacement integration
unsigned long start3 = 0;
unsigned int elapsedTime3 = 0;

unsigned int time_frame0 = 10;
unsigned int time_frame1 = 100;

//////////////////////// For integration /////////////////////////

float v0 = 0; // in cm/s
float v1 = 0;
float v2 = 0;

float v = 0;
float v_n = 0;
float w = 0;

float v_x = 0;
float v_y = 0;

float x = 0;
float y = 0;
float phi = 0;

float targetX = 0;
float targetY = 0;
float targetPhi = 0;
int targetTime = 0;

float dx = 0;
float dy = 0;
float dphi = 0;

float speed0 = 0, speed1 = 0, speed2 = 0;

float time_limit = 0.0;

float phi_initial = PI;

float phi_t(float phi)
{
  return (-phi + phi_initial);
}
//////////////////// For serial communication ////////////////////
byte a;
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

byte targetTime_H;
byte targetTime_L;

byte x_send_h;
byte x_send_l;
byte y_send_h;
byte y_send_l;
byte phi_send_h;
byte phi_send_l;

int x_send;
int y_send;
int phi_send;

int v0_send;
int v1_send;
int v2_send;

byte v0_send_h;
byte v0_send_l;
byte v1_send_h;
byte v1_send_l;
byte v2_send_h;
byte v2_send_l;

byte elapsedTime0_send_h;
byte elapsedTime0_send_l;

int start_flag = 0;

int twobytes1int(byte high, byte low)
{
  int number;

  number = low | (high << 8);
  return number;
}

////////////////////////// setup and loop /////////////////////////

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  pinMode(int1, OUTPUT);
  pinMode(int2, OUTPUT);
  pinMode(enA, OUTPUT);

  pinMode(int3, OUTPUT);
  pinMode(int4, OUTPUT);
  pinMode(enB, OUTPUT);

  pinMode(int5, OUTPUT);
  pinMode(int6, OUTPUT);
  pinMode(enC, OUTPUT);

  analogWrite(enA, 0);
  analogWrite(enB, 0);
  analogWrite(enC, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
   elapsedTime0 = millis() - start0;
   elapsedTime1 = millis() - start1;
   elapsedTime3 = millis() - start3;

   if (Serial.available())
  {
    a = Serial.read();
    if (a == 255) // get coordinate
    {
      Serial.readBytes(buf, 12);
      targetX_H = buf[0];
      targetX_L = buf[1];
      targetX_dir = buf[2];

      targetY_H = buf[3];
      targetY_L = buf[4];
      targetY_dir = buf[5];

      targetPhi_H = buf[6];
      targetPhi_L = buf[7];
      targetPhi_dir = buf[8];

      targetTime_H = buf[9];
      targetTime_L = buf[10];

      targetX = twobytes1int(targetX_H, targetX_L);
      targetY = twobytes1int(targetY_H, targetY_L);
      targetPhi= twobytes1int(targetPhi_H, targetPhi_L);
      targetTime = twobytes1int(targetTime_H, targetTime_L);

      targetX = targetX * (targetX_dir - 2);
      targetY = targetY * (targetY_dir - 2);
      targetPhi = targetPhi * (targetPhi_dir - 2);
      targetPhi = targetPhi/180.0*PI;

      start1 = millis();
      start2 = millis();

    }

    if (a == 254) // reset
    {
      // stop the rover

      x = 0;
      y = 0;
      phi = 0;
      start0 = millis();
      start1 = millis();
      start2 = millis();
      start3 = millis();
      elapsedTime0 = 0;
    }

    if (a == 253) // send state to RPI
    {
      Serial.write(255);
      if (x < 0) 
      {
        x_send = (int) x*(-1);
        x_send_h = ((x_send >> 8) & 0xFF);
        x_send_l = (x_send & 0xFF);  
        Serial.write(x_send_h);
        Serial.write(x_send_l);
        Serial.write(1);
      }
      else
      {
       x_send = (int) x;
       x_send_h = ((x_send >> 8) & 0xFF);
       x_send_l = (x_send & 0xFF); 
       Serial.write(x_send_h);
       Serial.write(x_send_l);
       Serial.write(3);
     }
    
     if (y < 0) 
     {
       y_send = (int) y*(-1);
       y_send_h = ((y_send >> 8) & 0xFF);
       y_send_l = (y_send & 0xFF);
       Serial.write(y_send_h);
       Serial.write(y_send_l);  
       Serial.write(1);    
     }
     else
     {
       y_send = (int) y;
       y_send_h = ((y_send >> 8) & 0xFF);
       y_send_l = (y_send & 0xFF); 
       Serial.write(y_send_h);
       Serial.write(y_send_l);
       Serial.write(3);
     }
    
     if (phi < 0) 
     {
       phi_send = (int) (phi*(180/3.14)*(-1));
       phi_send_h = ((phi_send >> 8) & 0xFF);
       phi_send_l = (phi_send & 0xFF);  
       Serial.write(phi_send_h);
       Serial.write(phi_send_l);
       Serial.write(1);
     }
     else
     {
       phi_send = (int) (phi*(180/3.14));
       phi_send_h = ((phi_send >> 8) & 0xFF);
       phi_send_l = (phi_send & 0xFF); 
       Serial.write(phi_send_h);
       Serial.write(phi_send_l);
       Serial.write(3);
     }

     if (v0 < 0) 
      {
        v0_send = (int) 100*speed0*(-1);
        v0_send_h = ((v0_send >> 8) & 0xFF);
        v0_send_l = (v0_send & 0xFF);  
        Serial.write(v0_send_h);
        Serial.write(v0_send_l);
        Serial.write(1);
      }
      else
      {
       v0_send = (int) 100*speed0;
       v0_send_h = ((v0_send >> 8) & 0xFF);
       v0_send_l = (v0_send & 0xFF); 
       Serial.write(v0_send_h);
       Serial.write(v0_send_l);
       Serial.write(3);
     }

     if (v1 < 0) 
      {
        v1_send = (int) 100*speed1*(-1);
        v1_send_h = ((v1_send >> 8) & 0xFF);
        v1_send_l = (v1_send & 0xFF);  
        Serial.write(v1_send_h);
        Serial.write(v1_send_l);
        Serial.write(1);
      }
      else
      {
       v1_send = (int) 100*speed1;
       v1_send_h = ((v1_send >> 8) & 0xFF);
       v1_send_l = (v1_send & 0xFF); 
       Serial.write(v1_send_h);
       Serial.write(v1_send_l);
       Serial.write(3);
     }

     if (v2 < 0) 
      {
        v2_send = (int) 100*speed2*(-1);
        v2_send_h = ((v2_send >> 8) & 0xFF);
        v2_send_l = (v2_send & 0xFF);  
        Serial.write(v2_send_h);
        Serial.write(v2_send_l);
        Serial.write(1);
      }
      else
      {
       v2_send = (int) 100*speed2;
       v2_send_h = ((v2_send >> 8) & 0xFF);
       v2_send_l = (v2_send & 0xFF); 
       Serial.write(v2_send_h);
       Serial.write(v2_send_l);
       Serial.write(3);

      }

      elapsedTime0_send_h = (elapsedTime0 >> 8) & 0xFF;
      elapsedTime0_send_l = (elapsedTime0 & 0xFF);
      Serial.write(elapsedTime0_send_h);
      Serial.write(elapsedTime0_send_l);
    }
    
  }

  if (elapsedTime0 > (targetTime*1000)) // start running
  {
    /////////////////////////// Controller /////////////////////////
    if (elapsedTime3 > time_frame1) // calculate required speed
    {
      start3 = millis();

      // find dx, dy, dphi
      dx = targetX - x;
      dy = targetY - y;
      dphi = targetPhi - phi;
      
      time_limit = sqrt(dx*dx + dy*dy)/constant_linearVelocity;

      if (time_limit == 0)
      {
       time_limit = 1;
      }
      
      v_x = dx/time_limit;
      v_y = dy/time_limit;
      w = dphi/time_limit;
      
      /////////// Calculate require speed of 3 wheels //////////////

      // find v, v_n
      v = v_x*cos(phi_t(phi)) + v_y*sin(phi_t(phi));
      v_n = -v_x*sin(phi_t(phi)) + v_y*cos(phi_t(phi));

      // find v_0, v_1, v_2
      v0 = -v*sin(PI/3) + v_n*cos(PI/3) + w*rover_radius;
      v1 =              - v_n           + w*rover_radius;
      v2 =  v*sin(PI/3) + v_n*cos(PI/3) + w*rover_radius;

      // Output speed to each of the wheels


    }  

    ////////////////////////// Integration ////////////////////////
    if (elapsedTime1 > time_frame0)
    {
      start1 = millis();

      // Get encoder data
      displacement0 = (enc0.read() - posPrevious0)/countPerRotation*(2*PI*wheel_radius);
      displacement1 = (enc1.read() - posPrevious1)/countPerRotation*(2*PI*wheel_radius);
      displacement2 = (enc2.read() - posPrevious2)/countPerRotation*(2*PI*wheel_radius);

      posPrevious0 = enc0.read();
      posPrevious1 = enc1.read();
      posPrevious2 = enc2.read();

      // Done getting data, start integration
        // recalculating v_x, v_y and w
      w = 1/(3*rover_radius)*(speed0 + speed1 + speed2);
      v = (sqrt(3)/3)*(speed2 - speed0);
      v_n = 1/3.0*(speed2 + speed0) - 2/3.0*speed1;

      v_x = v*cos(phi_t(phi)) - v_n*sin(phi_t(phi));
      v_y = v*sin(phi_t(phi)) + v_n*cos(phi_t(phi));

      elapsedTime2 = millis() - start2;
      start2 = millis();

      // calculate x, y phi
      x = x + v_x*elapsedTime2/1000.0;
      y = y + v_y*elapsedTime2/1000.0;
      phi = phi + w*elapsedTime2/1000.0;

      // stopping condition
      if (abs(dx) < 0.5 && abs(dy) < 0.5 && abs(dphi) < 0.015)
      {
        // stop

      }
    }  
  }
  
}
