// AccelStepper to control stepper motors
#include <AccelStepper.h>
#include <math.h>
// The SFE_LSM9DS1 library requires both Wire and SPI be
// included BEFORE including the 9DS1 library.
#include <Wire.h>
#include <SparkFunLSM9DS1.h>

//Stepper X, Y, Z are the same as the X, Y, Z on Protoneer shield
AccelStepper stepper0(AccelStepper::DRIVER, 2, 5); // pin 2 for step, pin 5 for direction
AccelStepper stepper1(AccelStepper::DRIVER, 3, 6);
AccelStepper stepper2(AccelStepper::DRIVER, 4, 7);

//////////////////////// For filtering ///////////////////////////

// gyroscope
float gyro0_previous, gyro1_previous, gyro2_previous;
float gyro0_now, gyro1_now, gyro2_now;
float HPF_gyro0 = 0, HPF_gyro1 = 0, HPF_gyro2 = 0;

// accelerometer
float rollAngle_previous0 = 0, rollAngle_now0 = 0;
float rollAngle_previous1 = 0, rollAngle_now1 = 0;
float rollAngle_previous2 = 0, rollAngle_now2 = 0;

float deltaAngle0 = 0, deltaAngle1 = 0, deltaAngle2 = 0;
float LPF_accel0 = 0, LPF_accel1 = 0, LPF_accel2 = 0;

// filtered variable
float filteredX, filteredY, filteredZ;
int filter_flag = 0;

// high pass and low pass filter
const float alpha = 0.7;

// complimentary filter
const float gyroCoef = 0.6;
const float accCoef = 0.4;

float comWheel0 = 0, comWheel1 = 0, comWheel2 = 0;


////////////////////////////////// For serial communication //////////////////////////////////
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

float targetX = 0;
float targetY = 0;
float targetPhi = 0;
int targetTime = 0;

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

byte timer_send_h;
byte timer_send_l;

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

unsigned long start1 = 0; // when to run
unsigned long timeNow = 0;
unsigned long timePrevious = 0;
float elapsedTime = 0;
int timeFlag = 0;
unsigned long timer = 0;
int go_flag = 0;

int resolution = 8;
float rover_radius = 15.6; // in cm
float wheel_radius = 5.22; // in cm
float constant_linearVelocity = 400*((wheel_radius*PI)/(resolution*100)); // in cm/s
int threshold_velocity = 400;

float time_limit = 0.0;

int twobytes1int(byte high, byte low)
{
  int number;

  number = low | (high << 8);
  return number;
}

// for speed scaling
float largest_of_three(float first, float second, float third)
{
  float largest = first;
  if (abs(second) > abs(largest))
  {
    largest = second;
  }

  if (abs(third) > abs(largest))
  {
    largest = third;
  }

  return abs(largest);
}

//////////////////////////// For IMU sensor ////////////////////////
// objects of classes
LSM9DS1 imu0;
LSM9DS1 imu1;
LSM9DS1 imu2;

#define LSM9DS1_AG  0x6B // accelerometer and gyrometer
#define LSM9DS1_M  0x1E // magnetometer
#define TCAADDR 0x70 // multiplexer i2c

// gyroscope
float gyroOffset0, gyroOffset1, gyroOffset2;
float gyro0, gyro1, gyro2;

// accelerometer
float accelX0 = 0, accelY0 = 0, accelZ0 = 0;
float accelX1 = 0, accelY1 = 0, accelZ1 = 0;
float accelX2 = 0, accelY2 = 0, accelZ2 = 0;
float accelOffsetX0 = 0, accelOffsetY0 = 0, accelOffsetZ0 = 0;
float accelOffsetX1 = 0, accelOffsetY1 = 0, accelOffsetZ1 = 0;
float accelOffsetX2 = 0, accelOffsetY2 = 0, accelOffsetZ2 = 0;

struct accelOffset {
	float x;
	float y;
	float z;
};

float DegreesToRadians(float degrees)
{
    return degrees*M_PI/180;
}

// change channel in multiplexer 0 to 7
void tcaselect(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

// find average value of 100 reading samples
float calibrateGyro(LSM9DS1 imu)
{
  float valueX = 0;
  float valueY = 0;
  float valueZ = 0;
  
  float value = 0;
  float limit = 100;
  int i = 0;

  // find mean value 
  while (i <= 100)
  {
    if (imu.gyroAvailable()) 
    {
      i++;
      imu.readGyro();
      valueX += imu.ax;
      valueY += imu.ay;
      valueZ += imu.az;
    }
  }

  return value/limit;
  
}

struct accelOffset calibrateAccel(LSM9DS1 imu)
{
  float valueX = 0;
  float valueY = 0;
  float valueZ = 0;

  float offsetX = 0;
  float offsetY = 0;
  float offsetZ = 0;

  int limit = 100;
  int i = 0;

  struct accelOffset offset;

  // find mean value
  while (i <= limit)
  {
    if (imu.accelAvailable()) 
    {
      i++;
      imu.readAccel();
      valueX += imu.ax;
      valueY += imu.ay;
      valueZ += imu.az;
    }
  }

  offset.x = valueX/limit;
  offset.y = valueY/limit;
  offset.z = valueZ/limit;

  return offset;
}

void setup()
{
  Wire.begin();  
  Serial.begin(9600);

  // for stepper motor control
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

  // for IMU sensor
  imu0.settings.device.commInterface = IMU_MODE_I2C;
  imu0.settings.device.mAddress = LSM9DS1_M;
  imu0.settings.device.agAddress = LSM9DS1_AG;
  
  imu1.settings.device.commInterface = IMU_MODE_I2C;
  imu1.settings.device.mAddress = LSM9DS1_M;
  imu1.settings.device.agAddress = LSM9DS1_AG;
  
  imu2.settings.device.commInterface = IMU_MODE_I2C;
  imu2.settings.device.mAddress = LSM9DS1_M;
  imu2.settings.device.agAddress = LSM9DS1_AG;

  /* Initialise the 1st sensor */
  tcaselect(0);
  while(!imu0.begin());  
  gyroOffset0 = calibrateGyro(imu0);
  struct accelOffset offset0;
  offset0 = calibrateAccel(imu0);
  accelOffsetX0 = offset0.x;
  accelOffsetY0 = offset0.y;
  accelOffsetZ0 = offset0.z;
  
  /* Initialise the 2nd sensor */
  tcaselect(1);
  while(!imu1.begin());
  gyroOffset1 = calibrateGyro(imu1);
  struct accelOffset offset1;
  offset1 = calibrateAccel(imu1);
  accelOffsetX1 = offset1.x;
  accelOffsetY1 = offset1.y;
  accelOffsetZ1 = offset1.z;
  
   /* Initialise the 3rd sensor */
  tcaselect(2);
  while(!imu2.begin());
  gyroOffset2 = calibrateGyro(imu2);
  struct accelOffset offset2;
  offset2 = calibrateAccel(imu2);
  accelOffsetX2 = offset2.x;
  accelOffsetY2 = offset2.y;
  accelOffsetZ2 = offset2.z;

  start1 = millis();
}


void loop()
{
  if (timeFlag == 1)
  {
    timeNow = millis();
    elapsedTime = (timeNow - timePrevious)/1000.0;
  }
  timePrevious = millis();
  timeFlag = 1;
  
  
  timer = (millis() - start1)/1000;
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

      go_flag = 1;

    }

    if (a == 254) // reset
    {
      stepper0.stop();
      stepper1.stop();
      stepper2.stop();

      x = 0;
      y = 0;
      phi = 0;
      start1 = millis();
      timer = 0;

      v0 = 0;
      v1 = 0;
      v2 = 0;
      v0s = 0;
      v1s = 0;
      v2s = 0;
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

     if (filteredX < 0) 
      {
        v0_send = (int) 100*filteredX*(-1);
        v0_send_h = ((v0_send >> 8) & 0xFF);
        v0_send_l = (v0_send & 0xFF);  
        Serial.write(v0_send_h);
        Serial.write(v0_send_l);
        Serial.write(1);
      }
      else
      {
       v0_send = (int) 100*filteredX;
       v0_send_h = ((v0_send >> 8) & 0xFF);
       v0_send_l = (v0_send & 0xFF); 
       Serial.write(v0_send_h);
       Serial.write(v0_send_l);
       Serial.write(3);
     }

     if (filteredY < 0) 
      {
        v1_send = (int) 100*filteredY*(-1);
        v1_send_h = ((v1_send >> 8) & 0xFF);
        v1_send_l = (v1_send & 0xFF);  
        Serial.write(v1_send_h);
        Serial.write(v1_send_l);
        Serial.write(1);
      }
      else
      {
       v1_send = (int) 100*filteredY;
       v1_send_h = ((v1_send >> 8) & 0xFF);
       v1_send_l = (v1_send & 0xFF); 
       Serial.write(v1_send_h);
       Serial.write(v1_send_l);
       Serial.write(3);
     }

     if (filteredZ < 0) 
      {
        v2_send = (int) 100*filteredZ*(-1);
        v2_send_h = ((v2_send >> 8) & 0xFF);
        v2_send_l = (v2_send & 0xFF);  
        Serial.write(v2_send_h);
        Serial.write(v2_send_l);
        Serial.write(1);
      }
      else
      {
       v2_send = (int) 100*filteredZ;
       v2_send_h = ((v2_send >> 8) & 0xFF);
       v2_send_l = (v2_send & 0xFF); 
       Serial.write(v2_send_h);
       Serial.write(v2_send_l);
       Serial.write(3);

      }

      timer_send_h = (timer >> 8) & 0xFF;
      timer_send_l = (timer & 0xFF);
      Serial.write(timer_send_h);
      Serial.write(timer_send_l);
    }
  }
 if (timer > targetTime && go_flag == 1)
 {
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

  // find v, v_n
  v = v_x*cos(phi) + v_y*sin(phi);
  v_n = -v_x*sin(phi) + v_y*cos(phi);

  // find v_0, v_1, v_2
  v0 = -v*sin(PI/3) + v_n*cos(PI/3) + w*rover_radius;
  v1 =              - v_n           + w*rover_radius;
  v2 = v*sin(PI/3)  + v_n*cos(PI/3) + w*rover_radius;

  // convert it back to step/s
   v0s = v0 * ((resolution*100)/(wheel_radius*PI));
   v1s = v1 * ((resolution*100)/(wheel_radius*PI));
   v2s = v2 * ((resolution*100)/(wheel_radius*PI));

  // scale down v_0, v_1, v_2 if necessary
  if (abs(v0s) > threshold_velocity || abs(v1s) > threshold_velocity || abs(v2s) > threshold_velocity)
  {
     // find the largest velocity
     largest_vel = largest_of_three(v0s, v1s, v2s);
   
     // scale down
     v0s = v0s/largest_vel*threshold_velocity;
     v1s = v1s/largest_vel*threshold_velocity;
     v2s = v2s/largest_vel*threshold_velocity;
  }

  stepper0.setSpeed(v0s);
  stepper1.setSpeed(v1s);
  stepper2.setSpeed(v2s);

  // get IMU reading
  // wheel 0
    tcaselect(0);
    if (imu0.gyroAvailable())
    	imu0.readGyro();
    gyro0 = imu0.calcGyro(imu0.gz - gyroOffset0);

    if (imu0.accelAvailable())
    	imu0.readAccel();

    accelX0 = imu0.calcAccel(imu0.ax - accelOffsetX0);
    accelY0 = imu0.calcAccel(imu0.ay - accelOffsetY0);
    accelZ0 = imu0.calcAccel(imu0.az - accelOffsetZ0);

    // linear_X = DegreesToRadians(gyro0) * wheel_radius; // deg/s to rad/s

    if (filter_flag == 1)
    {
      gyro0_now = gyro0;
      HPF_gyro0 = HPF_gyro0*alpha + alpha*(gyro0_now - gyro0_previous);

      rollAngle_now0 = atan2(accelX0, accelY0);
      deltaAngle0 = rollAngle_now0 - rollAngle_previous0;
      LPF_accel0 = (deltaAngle0/elapsedTime)*alpha + (LPF_accel0*(1.0 - alpha)); // in degree/s
      comWheel0 = (gyroCoef*(comWheel0 + HPF_gyro0) + (accCoef*LPF_accel0));
    } 

    gyro0_previous = gyro0;
    rollAngle_previous0 = atan2(accelX0, accelY0);

  // wheel 1
    tcaselect(1);
    if (imu1.gyroAvailable())
    	imu1.readGyro();
    gyro1 = imu1.calcGyro(imu1.gz - gyroOffset1);

    if (imu1.accelAvailable())
    	imu1.readAccel();

    accelX1 = imu1.calcAccel(imu1.ax - accelOffsetX1);
    accelY1 = imu1.calcAccel(imu1.ay - accelOffsetY1);
    accelZ1 = imu1.calcAccel(imu1.az - accelOffsetZ1);

    // linear_Y = DegreesToRadians(gyro1) * wheel_radius;

    if (filter_flag == 1)
    {
      gyro1_now = gyro1;
      HPF_gyro1 = HPF_gyro1*alpha + alpha*(gyro1_now - gyro1_previous);

      rollAngle_now1 = atan2(accelX1, accelY1);
      deltaAngle1 = rollAngle_now1 - rollAngle_previous1;
      LPF_accel1 = (deltaAngle0/elapsedTime)*alpha + (LPF_accel1*(1.0 - alpha)); // in degree/s
      comWheel1 = (gyroCoef*(comWheel1 + HPF_gyro1) + (accCoef*LPF_accel1));
    }

    gyro1_previous = gyro1;
    rollAngle_previous1 = atan2(accelX1, accelY1);

  // wheel 2
    tcaselect(2);
    if (imu2.gyroAvailable())
    	imu2.readGyro();
    gyro2 = imu2.calcGyro(imu2.gz - gyroOffset2);

    if (imu2.accelAvailable())
    	imu2.readAccel();

    accelX2 = imu2.calcAccel(imu2.ax - accelOffsetX2);
    accelY2 = imu2.calcAccel(imu2.ay - accelOffsetY2);
    accelZ2 = imu2.calcAccel(imu2.az - accelOffsetZ2);

    // linear_Z = DegreesToRadians(gyro2) * wheel_radius;

    if (filter_flag == 1)
    {
      gyro2_now = gyro2;
      HPF_gyro2 = HPF_gyro2*alpha + alpha*(gyro2_now - gyro2_previous);

      rollAngle_now2 = atan2(accelX2, accelY2);
      deltaAngle2 = rollAngle_now2 - rollAngle_previous2;
      LPF_accel2 = (deltaAngle0/elapsedTime)*alpha + (LPF_accel2*(1.0 - alpha)); // in degree/s
      comWheel2 = (gyroCoef*(comWheel2 + HPF_gyro2) + (accCoef*LPF_accel2));
    }

    gyro2_previous = gyro2;
    rollAngle_previous2 = atan2(accelX2, accelY2);

    filter_flag = 1;

  // recalculate v_x, v_y, w
  w = 1/(3*rover_radius)*(filteredX + filteredY + filteredZ);
  v = (sqrt(3)/3)*(filteredZ - filteredX);
  v_n = 1/3.0*(filteredZ + filteredX) - 2/3.0*filteredY;

  v_x = v*cos(phi) - v_n*sin(phi);
  v_y = v*sin(phi) + v_n*cos(phi);

  // calculate x, y phi
  x = x + v_x*elapsedTime;
  y = y + v_y*elapsedTime;
  phi = phi + w*elapsedTime;

  // stopping condition
  if (abs(dx) < 1 && abs(dy) < 1 && abs(dphi) < 0.018)
  {
    // stop
    stepper0.stop();
    stepper1.stop();
    stepper2.stop();
    v0 = 0;
    v1 = 0;
    v2 = 0;
    v0s = 0;
    v1s = 0;
    v2s = 0;

    go_flag = 0;
  }  
   
 } else {
    stepper0.stop();
    stepper1.stop();
    stepper2.stop();

    v0 = 0;
    v1 = 0;
    v2 = 0;
    v0s = 0;
    v1s = 0;
    v2s = 0;

    go_flag = 0;
 }

 if (go_flag == 1)
 {
  stepper0.runSpeed();
  stepper1.runSpeed();
  stepper2.runSpeed();
 }



}
