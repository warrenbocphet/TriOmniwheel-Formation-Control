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
int threshold_velocity = 400; // in step/s
float rover_radius = 19.3; // in cm
float wheel_radius = 5.22; // in cm
float constant_linearVelocity = threshold_velocity*((wheel_radius*PI)/(resolution*100)); // in cm/s

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

byte timer_send_h;
byte timer_send_l;

int start_flag = 0;

int twobytes1int(byte high, byte low)
{
  int number;

  number = low | (high << 8);
  return number;
}


//////////////////////// For integration /////////////////////////

float v0 = 0; // in cm/s
float v1 = 0;
float v2 = 0;

float v0s = 0; // in step/s
float v1s = 0;
float v2s = 0;
float largest_vel;

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
int speed0s = 0, speed1s = 0, speed2s = 0;

float time_limit = 0.0;
unsigned long timer = 0;

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

void setupDevice(LSM9DS1 imu)
{
  // [commInterface] determines whether we'll use I2C or SPI
  // to communicate with the LSM9DS1.
  // Use either IMU_MODE_I2C or IMU_MODE_SPI
  imu.settings.device.commInterface = IMU_MODE_I2C;

  // [agAddress] sets the I2C address or SPI CS pin of the
  // LSM9DS1's accelerometer/gyroscope.
  imu.settings.device.agAddress = 0x6B; // I2C address 0x6B
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
  imu.enableFIFO();
  imu.setFIFO(FIFO_CONT, 0x01);
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
unsigned long start2 = 0;
unsigned int elapsedTime2 = 0;

unsigned int time_frame = 25;


////////////////// For Filtering ////////////////////////////
int filter_flag = 0;

float filtered0 = 0;
float filtered1 = 0;
float filtered2 = 0;

struct KF_parameter	{
	float Q = 1e-4;
	float R = 0.1*0.1;

	float xhat = 0;
	float P = 0;
	float xhatminus = 0;
	float Pminus = 0;
	float K = 0;

	float z;
};

struct KF_parameter KF_0;
struct KF_parameter KF_1;
struct KF_parameter KF_2;

float KF_filter(int id, float data)	{
	struct KF_parameter* KF_fnc;

	if (id == 0) {
		KF_fnc = &KF_0;
	} else if (id == 1)	{
		KF_fnc = &KF_1;
	} else if (id == 2)	{
		KF_fnc = &KF_2;
	}

	KF_fnc->z = data;	

	// time update
	KF_fnc->xhatminus = KF_fnc->xhat;
	KF_fnc->Pminus = KF_fnc->P + KF_fnc->Q;

	// measurement update
	KF_fnc->K = (KF_fnc->Pminus)/(KF_fnc->Pminus + KF_fnc->R);
	KF_fnc->xhat = KF_fnc->xhatminus + (KF_fnc->K)*(KF_fnc->z - KF_fnc->xhatminus);
	KF_fnc->P = (1 - KF_fnc->K)*(KF_fnc->Pminus);

	return KF_fnc->xhat;
}

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

  stepper0.setMaxSpeed(800);
  stepper1.setMaxSpeed(800);
  stepper2.setMaxSpeed(800);

  delay(1000);

  // for IMU sensor
  /* Initialise the 1st sensor */
  tcaselect(0);
  setupDevice(imu0);
  while(!imu0.begin());  
  setupGyro(imu0);
  gyroOffset0 = calibrateGyro(imu0);
  
  /* Initialise the 2nd sensor */
  tcaselect(1);
  setupDevice(imu1);
  while(!imu1.begin());
  setupGyro(imu1);
  gyroOffset1 = calibrateGyro(imu1);
  
   /* Initialise the 3rd sensor */
  tcaselect(2);
  setupDevice(imu2);
  while(!imu2.begin());
  setupGyro(imu2);
  gyroOffset2 = calibrateGyro(imu2);

  start1 = millis();
  start2 = millis();
}

void loop()
{
   elapsedTime1 = millis() - start1;

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

      start_flag = 1;
      start1 = millis();
      start2 = millis();

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
        v0_send = (int) 100*speed0s*(-1);
        v0_send_h = ((v0_send >> 8) & 0xFF);
        v0_send_l = (v0_send & 0xFF);  
        Serial.write(v0_send_h);
        Serial.write(v0_send_l);
        Serial.write(1);
      }
      else
      {
       v0_send = (int) 100*speed0s;
       v0_send_h = ((v0_send >> 8) & 0xFF);
       v0_send_l = (v0_send & 0xFF); 
       Serial.write(v0_send_h);
       Serial.write(v0_send_l);
       Serial.write(3);
     }

     if (v1 < 0) 
      {
        v1_send = (int) 100*speed1s*(-1);
        v1_send_h = ((v1_send >> 8) & 0xFF);
        v1_send_l = (v1_send & 0xFF);  
        Serial.write(v1_send_h);
        Serial.write(v1_send_l);
        Serial.write(1);
      }
      else
      {
       v1_send = (int) 100*speed1s;
       v1_send_h = ((v1_send >> 8) & 0xFF);
       v1_send_l = (v1_send & 0xFF); 
       Serial.write(v1_send_h);
       Serial.write(v1_send_l);
       Serial.write(3);
     }

     if (v2 < 0) 
      {
        v2_send = (int) 100*speed2s*(-1);
        v2_send_h = ((v2_send >> 8) & 0xFF);
        v2_send_l = (v2_send & 0xFF);  
        Serial.write(v2_send_h);
        Serial.write(v2_send_l);
        Serial.write(1);
      }
      else
      {
       v2_send = (int) 100*speed2s;
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

   if (elapsedTime1 > time_frame && start_flag == 1)
   {
      start1 = millis();

      stepper0.runSpeed();
      stepper1.runSpeed();
      stepper2.runSpeed();

    ///////////// Controller (calculate linear velocity) ////////////

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
	  v = v_x*cos(phi) + v_y*sin(phi);
	  v_n = -v_x*sin(phi) + v_y*cos(phi);

	  // find v_0, v_1, v_2
	  v0 = -v*sin(PI/3) + v_n*cos(PI/3) + w*rover_radius;
	  v1 =              - v_n           + w*rover_radius;
	  v2 =  v*sin(PI/3) + v_n*cos(PI/3) + w*rover_radius;

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

      stepper0.runSpeed();
      stepper1.runSpeed();
      stepper2.runSpeed();
      

      ///////////// Getting data and filtering data /////////////
      ///////////// wheel 0 ///////////////
      tcaselect(0);
      if (imu0.gyroAvailable())
       imu0.readGyro();
      gyroReading0 = imu0.calcGyro(imu0.gz - gyroOffset0);

      if (filter_flag == 1)
      {
        // kalman filter
      	filtered0 = KF_filter(0, gyroReading0); // degree/s

        // converting unit
        speed0 = ((filtered0/360.0)*(2*PI))*wheel_radius; // convert to rad/s then cm/s
        speed0s = (filtered0/360.0)*1600;
      } 

      stepper0.runSpeed();
      stepper1.runSpeed();
      stepper2.runSpeed();

      ///////////// wheel 1 ///////////////
      tcaselect(1);
      if (imu1.gyroAvailable())
       imu1.readGyro();
      gyroReading1 = imu1.calcGyro(imu1.gz - gyroOffset1);

      if (filter_flag == 1)
      {
        // kalman filter
        filtered1 = KF_filter(1, gyroReading1); // degree/s
      
        // converting unit
        speed1 = ((filtered1/360.0)*(2*PI))*wheel_radius; // convert to rad/s then cm/s
        speed1s = (filtered1/360.0)*1600;
      } 

      stepper0.runSpeed();
      stepper1.runSpeed();
      stepper2.runSpeed();

      ///////////// wheel 2 ///////////////
      tcaselect(2);
      if (imu2.gyroAvailable())
       imu2.readGyro();
      gyroReading2 = imu2.calcGyro(imu2.gz - gyroOffset2);

      if (filter_flag == 1)
      {
        // kalman filter
        filtered2 = KF_filter(2, gyroReading2); // degree/s
    
        // converting unit
        speed2 = ((filtered2/360.0)*(2*PI))*wheel_radius; // convert to rad/s then cm/s
        speed2s = (filtered2/360.0)*1600;
      } 

      stepper0.runSpeed();
      stepper1.runSpeed();
      stepper2.runSpeed();
          
      filter_flag = 1; // After get the first data, this flag set to 1. 
                       // Having this flag helps me to know which data is the previous data and which is the current, just aquired data.

      ///////// Done getting and filtering data ///////////
      ///////// Start integration to find (x,y) ///////////

      // recalculate v_x, v_y, w
	  w = 1/(3*rover_radius)*(speed0 + speed1 + speed2);
	  v = (sqrt(3)/3)*(speed2 - speed0);
	  v_n = 1/3.0*(speed2 + speed0) - 2/3.0*speed1;

	  v_x = v*cos(phi) - v_n*sin(phi);
	  v_y = v*sin(phi) + v_n*cos(phi);

	  elapsedTime2 = millis() - start2;
	  start2 = millis();

	  // calculate x, y phi
	  x = x + v_x*elapsedTime2/1000.0;
	  y = y + v_y*elapsedTime2/1000.0;
	  phi = phi + w*elapsedTime2/1000.0;

  	stepper0.setSpeed(v0s);
	  stepper1.setSpeed(v1s);
	  stepper2.setSpeed(v2s);

	  // stopping condition
	  if (abs(dx) < 0.5 && abs(dy) < 0.5 && abs(dphi) < 0.015)
	  {
	    // stop
	    stepper0.stop();
	    stepper1.stop();
	    stepper2.stop();

	    stepper0.setSpeed(0);
	    stepper1.setSpeed(0);
	    stepper2.setSpeed(0);

      start_flag = 0;
	  } 
   }

   stepper0.runSpeed();
   stepper1.runSpeed();
   stepper2.runSpeed();
}
