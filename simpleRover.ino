#include <AccelStepper.h>

//Stepper X, Y, Z are the same as the X, Y, Z on Protoneer shield
AccelStepper stepperX(AccelStepper::DRIVER, 2, 5); // pin 2 for step, pin 5 for direction
AccelStepper stepperY(AccelStepper::DRIVER, 3, 6);
AccelStepper stepperZ(AccelStepper::DRIVER, 4, 7);

byte start_byte; //notify that this is the beginning of the command.

byte x_spd_H;
byte x_spd_L;
byte x_dir; // direction should be either 1 for CCW or 3 for CW. This number will later be...
            // ...subtracted to 2, which then create -1 and 1. Remember, byte is an unsigned number.
byte y_spd_H; 
byte y_spd_L;
byte y_dir;

byte z_spd_H;
byte z_spd_L;
byte z_dir;

byte stop_byte;

float x_speed;
float y_speed;
float z_speed;

byte buf[10];

int twobytes1int(byte high, byte low)
{
  int number;

  number = low | (high << 8);
  return number;
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

  stepperX.setMaxSpeed(8000);
  stepperY.setMaxSpeed(8000);
  stepperZ.setMaxSpeed(8000);

  stepperX.setSpeed(0);
  stepperY.setSpeed(0);
  stepperZ.setSpeed(0);
}

void loop()
{
  // Receiving the command through serial port and setting up the motor
  if (Serial.available())
  {
    if (Serial.read() == 255)
    {
      Serial.readBytes(buf, 10);
    }
  	  x_spd_H = buf[0];
      x_spd_L = buf[1];
      x_dir = buf[2];

      y_spd_H = buf[3];
      y_spd_L = buf[4];
      y_dir = buf[5];

      z_spd_H = buf[6];
      z_spd_L = buf[7];
      z_dir = buf[8];
      
      stop_byte = buf[9];

      x_speed = twobytes1int(x_spd_H, x_spd_L);
      y_speed = twobytes1int(y_spd_H, y_spd_L);
      z_speed = twobytes1int(z_spd_H, z_spd_L);
      
      
      stepperX.setSpeed((x_speed)*(x_dir-2));
      stepperY.setSpeed((y_speed)*(y_dir-2));
      stepperZ.setSpeed((z_speed)*(z_dir-2));
  	}

  if (stop_byte == 0)
  {
    stepperX.runSpeed();
    stepperY.runSpeed();
    stepperZ.runSpeed(); 
  }
  else
  {
    stepperX.stop();
    stepperY.stop();
    stepperZ.stop();
  }
  
}
