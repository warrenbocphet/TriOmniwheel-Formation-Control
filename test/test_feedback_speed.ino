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
int m=0;
int n=0;

int a=-1;

int speed0 = -1;
int speed1 = -1;
int speed2 = -1;

int filter_flag = 0;

//////////////////////// For integration /////////////////////////


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

void setupGyro(LSM9DS1 imu)
{
  // [enabled] turns the gyro on or off.
  imu.settings.gyro.enabled = true;  // Enable the gyro
  // [scale] sets the full-scale range of the gyroscope.
  // scale can be set to either 245, 500, or 2000
  imu.settings.gyro.scale = 245; // Set scale to +/-245dps
  // [sampleRate] sets the output data rate (ODR) of the gyro
  // sampleRate can be set between 1-6
  // 1 = 14.9    4 = 238
  // 2 = 59.5    5 = 476
  // 3 = 119     6 = 952
  imu.settings.gyro.sampleRate = 2; // 59.5Hz ODR
  // [bandwidth] can set the cutoff frequency of the gyro.
  // Allowed values: 0-3. Actual value of cutoff frequency
  // depends on the sample rate. (Datasheet section 7.12)
  imu.settings.gyro.bandwidth = 0;
  // [lowPowerEnable] turns low-power mode on or off.
  imu.settings.gyro.lowPowerEnable = false; // LP mode off
  // [HPFEnable] enables or disables the high-pass filter
  imu.settings.gyro.HPFEnable = true; // HPF disabled
  // [HPFCutoff] sets the HPF cutoff frequency (if enabled)
  // Allowable values are 0-9. Value depends on ODR.
  // (Datasheet section 7.14)
  imu.settings.gyro.HPFCutoff = 1; // HPF cutoff = 4Hz
  // [flipX], [flipY], and [flipZ] are booleans that can
  // automatically switch the positive/negative orientation
  // of the three gyro axes.
  imu.settings.gyro.flipX = false; // Don't flip X
  imu.settings.gyro.flipY = false; // Don't flip Y
  imu.settings.gyro.flipZ = false; // Don't flip Z
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
  /* Initialise the 1st sensor */
  tcaselect(0);
  while(!imu0.begin());  
  setupGyro(imu0);
  gyroOffset0 = calibrateGyro(imu0);
  
  /* Initialise the 2nd sensor */
  tcaselect(1);
  while(!imu1.begin());
  setupGyro(imu1);
  gyroOffset1 = calibrateGyro(imu1);
  
   /* Initialise the 3rd sensor */
  tcaselect(2);
  while(!imu2.begin());
  setupGyro(imu2);
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
      j = (i%11)*100;
      m = -(i%8)*100;
      n = pow(-1,i)*(i%8)*50;
      
      stepper0.setSpeed(j);
      stepper1.setSpeed(m);
      stepper2.setSpeed(n);

    } else if (a == 1) {    
      Serial.println(speed0);
      Serial.println(speed1);
      Serial.println(speed2);
    }
   }

   if (elapsedTime1 > time_frame) // get IMU reading
   {
      start1 = millis();
      
      ///////////// wheel 0 ///////////////
      tcaselect(0);
      if (imu0.gyroAvailable())
       imu0.readGyro();
      gyroReading0 = imu0.calcGyro(imu0.gz - gyroOffset0);

      if (filter_flag == 1)
      {
        // kalman filter

        // converting unit
        speed0 = (gyroReading0/360.0)*1600.0; // from degree/s to step/s
      } 

      ///////////// wheel 1 ///////////////
      tcaselect(1);
      if (imu1.gyroAvailable())
       imu1.readGyro();
      gyroReading1 = imu1.calcGyro(imu1.gz - gyroOffset1);

      if (filter_flag == 1)
      {
        // kalman filter
      
        // converting unit
        speed1 = (gyroReading1/360.0)*1600.0; // from degree/s to step/s
      } 

      ///////////// wheel 2 ///////////////
      tcaselect(2);
      if (imu2.gyroAvailable())
       imu2.readGyro();
      gyroReading2 = imu2.calcGyro(imu2.gz - gyroOffset2);

      if (filter_flag == 1)
      {
        // kalman filter
    
        // converting unit
        speed2 = (gyroReading2/360.0)*1600.0; // from degree/s to step/s
      } 
          
      filter_flag = 1; // After get the first data, this flag set to 1. 
                       // Having this flag helps me to know which data is the previous data and which is the current, just aquired data.

      ///////// Done getting and filtering data ///////////
   }
    
  
   stepper0.runSpeed();
   stepper1.runSpeed();
   stepper2.runSpeed();
}
