// AccelStepper to control stepper motors
#include <AccelStepper.h>
#include <math.h>
// The SFE_LSM9DS1 library requires both Wire and SPI be
// included BEFORE including the 9DS1 library.
#include <Wire.h>
#include <SparkFunLSM9DS1.h>

////////////////////// For stepper motors /////////////////////////
AccelStepper stepper0(AccelStepper::DRIVER, 2, 5); // pin 2 for step, pin 5 for direction
AccelStepper stepper1(AccelStepper::DRIVER, 3, 6);
AccelStepper stepper2(AccelStepper::DRIVER, 4, 7);

int resolution = 8;
int threshold_velocity = 400;
float rover_radius = 19.3; // in cm
float wheel_radius = 5.22; // in cm
float constant_linearVelocity = 400*((wheel_radius*PI)/(resolution*100)); // in cm/s

int i=0;
int j=0;
int a=-1;

int speed0 = -1;

//////////////////////// For integration /////////////////////////



//////////////////////// For filtering ///////////////////////////

// gyroscope

struct complimentary_gyro // keep track of all the variable for the complimentary filter on each gyroscope
{
  float gyro_now = 0, gyro_previous = 0;
  float HPF_gyro = 0, LPF_gyro = 0;
  float comWheel = 0;
  float filtered = 0;
};

struct complimentary_gyro gyro0;
struct complimentary_gyro gyro1;
struct complimentary_gyro gyro2;

// float gyro0_previous, gyro1_previous, gyro2_previous;
// float gyro0_now, gyro1_now, gyro2_now;
// float HPF_gyro0 = 0, HPF_gyro1 = 0, HPF_gyro2 = 0;
// float LPF_accel0 = 0, LPF_accel1 = 0, LPF_accel2 = 0;

// filtered variable
float filtered0, filtered1, filtered2;
int filter_flag = 0;

// high pass and low pass filter
const float alpha = 0.7;

// complimentary filter
const float gyroCoef = 0.6;
const float accCoef = 0.4;

// float comWheel0 = 0, comWheel1 = 0, comWheel2 = 0;

void complimentary_filter (int id, float gyro_reading)
{
  struct complimentary_gyro* gyro_fnc; 
  float gyro_reading_fnc = gyro_reading;

  if (id == 0) {
    gyro_fnc = &gyro0;
  } else if (id == 1) {
    gyro_fnc = &gyro1;
  } else if (id == 2) {
    gyro_fnc = &gyro2;
  }

  gyro_fnc->gyro_now = gyro_reading_fnc;
  gyro_fnc->HPF_gyro = gyro_fnc->HPF_gyro*alpha + alpha*(gyro_fnc->gyro_now - gyro_fnc->gyro_previous);

  gyro_fnc->LPF_gyro = gyro_fnc->gyro_now*alpha + (gyro_fnc->LPF_gyro*(1.0 - alpha)); // in degree/s
  gyro_fnc->comWheel = (gyroCoef*(gyro_fnc->comWheel + gyro_fnc->HPF_gyro) + (accCoef*gyro_fnc->LPF_gyro));
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
float gyroReading0, gyroReading1, gyroReading2;

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
      value += imu.az;
    }
  }

  return value/limit;
}

////////////////// For keeping track of time ////////////////
unsigned long start1 = 0;
unsigned int elapsedTime1 = 0;
unsigned int time_frame = 25;

////////////////// Setup and Loop ///////////////////////////

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

  // for IMU sensor
//  imu0.settings.device.commInterface = IMU_MODE_I2C;
//  imu0.settings.device.mAddress = LSM9DS1_M;
//  imu0.settings.device.agAddress = LSM9DS1_AG;
//  
//  imu1.settings.device.commInterface = IMU_MODE_I2C;
//  imu1.settings.device.mAddress = LSM9DS1_M;
//  imu1.settings.device.agAddress = LSM9DS1_AG;
//  
//  imu2.settings.device.commInterface = IMU_MODE_I2C;
//  imu2.settings.device.mAddress = LSM9DS1_M;
//  imu2.settings.device.agAddress = LSM9DS1_AG;

  /* Initialise the 1st sensor */
  tcaselect(0);
  while(!imu0.begin());  
  gyroOffset0 = calibrateGyro(imu0);
  
  /* Initialise the 2nd sensor */
  tcaselect(1);
  while(!imu1.begin());
  gyroOffset1 = calibrateGyro(imu1);
  
   /* Initialise the 3rd sensor */
  tcaselect(2);
  while(!imu2.begin());
  gyroOffset2 = calibrateGyro(imu2);

  start1 = millis();
}

void loop()
{
   elapsedTime1 = millis() - start1;

   if (Serial.available())
   {
    a = Serial.read();
    if (a == 0)
    {
      i++;
      j = (i%8)*100;
      stepper0.setSpeed(j);
    } else if (a == 1) {    
      Serial.println(speed0);
    }
   }

   if (elapsedTime1 > time_frame) // get IMU reading
   {
      start1 = millis();
      // wheel 0
      tcaselect(0);
      if (imu0.gyroAvailable())
       imu0.readGyro();
      gyroReading0 = imu0.calcGyro(imu0.gz - gyroOffset0);

      if (filter_flag == 1)
      {
        // complimentary filter
        // gyro0_now = gyro0;
        // HPF_gyro0 = HPF_gyro0*alpha + alpha*(gyro0_now - gyro0_previous);

        // LPF_accel0 = gyro0_now*alpha + (LPF_accel0*(1.0 - alpha)); // in degree/s
        // comWheel0 = (gyroCoef*(comWheel0 + HPF_gyro0) + (accCoef*LPF_accel0));
        complimentary_filter(0, gyroReading0);

        // kalman filter


        // converting unit
        filtered0 = ((gyro0.comWheel/180.0)*PI)*wheel_radius; // convert to rad/s and then cm/s 
        speed0 = (gyro0.comWheel/360.0)*1600.0; // from degree/s to step/s
      } 

      gyro0.gyro_previous = gyroReading0;
      filter_flag = 1;
   }
    
  
   stepper0.runSpeed();
}
