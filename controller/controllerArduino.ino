///////////////////////////// Library /////////////////////////////
#include <Encoder.h>
#include <PID_v1.h>
#include <SPI.h>
#include <WiFiNINA.h>

///////////////////////// TCP communication /////////////////////
char ssid[] = "iPhone";
char pass[] = "00000000";
int status = WL_IDLE_STATUS;
byte buf[7];
int i;

IPAddress server(172,20,10,3);
WiFiClient client;
float targetX = 0.0, targetY = 0.0, targetPhi = 0.0;

int twobytes1int(byte high, byte low)
{
  int number;
  int sign;

  sign = high >> 7;
  high = high << 1;
  high = high >> 1;

  number = low | (high << 8);
  number = number*(pow(-1, sign));
  return number;
}

struct byteInt  {
  	int number;
  	byte high;
  	byte low;
};

struct byteInt xSend;
struct byteInt ySend;
struct byteInt phiSend;

void int2bytes(struct byteInt* number_fnc)	{
	int magnituteNumber;
	if (number_fnc->number >= 0)	{
		number_fnc->high = (number_fnc->number >> 8) & 0xFF;
		number_fnc->low = (number_fnc->number & 0xFF);
	} else {
		magnituteNumber = number_fnc->number * -1;
		number_fnc->high = (magnituteNumber >> 8) & 0xFF;
		number_fnc->high = number_fnc->high ^ 0x80;
		number_fnc->low = magnituteNumber & 0xFF;
	}
}

///////////////////////////// DC motors /////////////////////////////
const int int1 = A0; // motor 0
const int int2 = A1;
const int enA = 2;
int pwm0;

const int int3 = A2; // motor 1
const int int4 = A3;
const int enB = 3;
int pwm1;

const int int5 = A4; // motor 2
const int int6 = A5;
const int enC = 4;
int pwm2;

float rover_radius = 15.5; // in cm
float wheel_radius = 5.22; // in cm

int constant_linearVelocity = 0.5 * (2 * PI*wheel_radius);

///////////////////////////// Encoder /////////////////////////////
Encoder enc0(0, 1);
Encoder enc1(6, 7);
Encoder enc2(8, 9);

double posPrevious0 = 0;
double posPrevious1 = 0;
double posPrevious2 = 0;

float displacement0 = 0;
float displacement1 = 0;
float displacement2 = 0;

float countPerRotation = 2797.0;

/////////////////////////// PID ////////////////////////////
struct motorPID_parameter	{
  double Setpoint;
  double Input;
  double Output;
};

struct motorPID_parameter motor0;
struct motorPID_parameter motor1;
struct motorPID_parameter motor2;

//PID parameters
double Kp = 5.5, Ki = 17.5, Kd = 0.0;

////create PID instance

PID motorPID0(&motor0.Input, &motor0.Output, &motor0.Setpoint, Kp, Ki, Kd, DIRECT);
PID motorPID1(&motor1.Input, &motor1.Output, &motor1.Setpoint, Kp, Ki, Kd, DIRECT);
PID motorPID2(&motor2.Input, &motor2.Output, &motor2.Setpoint, Kp, Ki, Kd, DIRECT);

long posPreviousA = 0;
long posPreviousB = 0;
long posPreviousC = 0;

int displacementA = 0;
int displacementB = 0;
int displacementC = 0;

float speedA = 0.0, speedB = 0.0, speedC = 0.0;

////////////////// For keeping track of time ////////////////
unsigned long start0 = 0;
float elapsedTime0 = 0; // use as a timer to work with time_frame0
unsigned long start1 = 0;
float elapsedTime1 = 0;
unsigned long start2 = 0;
float elapsedTime2 = 0; // elapsedTime type should be float because use to calculate float data

int time_frame0 = 500;
int time_frame2 = 10;
//////////////////////// For integration /////////////////////////

float v0 = 0.0; // in cm/s
float v1 = 0.0;
float v2 = 0.0;

float v = 0.0;
float v_n = 0.0;
float w = 0.0;

float v_x = 0.0;
float v_y = 0.0;

float x = 0.0;
float y = 0.0;
float phi = 0.0;

float dx = 0.0;
float dy = 0.0;
float dphi = 0.0;

float speed0 = 0.0, speed1 = 0.0, speed2 = 0.0;

float time_limit = 0.0;

float phi_initial = PI;

float phi_t(float phi)
{
  return (-phi + phi_initial);
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

  // PID0 initialize
  motorPID0.SetMode(AUTOMATIC);
  motorPID0.SetTunings(Kp, Ki, Kd);
  motorPID0.SetOutputLimits(-255,255);
  motorPID0.SetSampleTime(10);

  // PID1 initialize
  motorPID1.SetMode(AUTOMATIC);
  motorPID1.SetTunings(Kp, Ki, Kd);
  motorPID1.SetOutputLimits(-255,255);
  motorPID0.SetSampleTime(10);

  // PID2 initialize
  motorPID2.SetMode(AUTOMATIC);
  motorPID2.SetTunings(Kp, Ki, Kd);
  motorPID2.SetOutputLimits(-255,255);
  motorPID0.SetSampleTime(10);
  
  start0 = millis();
  start1 = millis();

  status = WiFi.begin(ssid, pass);
  if (status != WL_CONNECTED) {
    while (status != WL_CONNECTED) {
    Serial.println(" No Wifi !");
    status = WiFi.begin(ssid, pass);
    delay(1000);
    }
  } else  {
    Serial.println(" YES WIFI!!! ");
    Serial.println("Now connecting to laptop.");
    int j = client.connect(server, 1234);
    if (j == 1)  {
      Serial.println("I'm innnnnnnnnnn!");
    } else if (j == -1) {
      while(true) {
        Serial.println("Time out!");
      }
    } else if (j == -2) {
      while(true) {
        Serial.println("Invalid_server");
      }
    } else if (j == -3) {      
      while(true) {
        Serial.println("Truncated.");
      }
    } else if (j == -4) {
      while(true) {
        Serial.println("Invalid response!");
      }
    }
  }

  delay(500);

}

void loop() {
  // put your main code here, to run repeatedly:
  elapsedTime0 = millis() - start0;
  elapsedTime2 = millis() - start2;
  
  if (client.available()) {
    int a = client.read();
    if (a == 0)	{ // receive new target
      Serial.println("I received a coordinate.");

      while (i<6) {
        if (client.available()) {
          buf[i] = client.read();
          Serial.print("buf[");
          Serial.print(i);
          Serial.print("]: ");
          Serial.println(buf[i]);
          i++;
        }
      }
      
//	    while (client.available())  {
//	      buf[i] = client.read();
//        Serial.print("buf[");
//        Serial.print(i);
//        Serial.print("]: ");
//        Serial.println(buf[i]);
//	      i++;
//	    }

      Serial.print("I received: ");
      Serial.print(i);
      Serial.println(" bytes");

	    targetX = twobytes1int(buf[0], buf[1]);
	    targetY = twobytes1int(buf[2], buf[3]);
	    targetPhi = twobytes1int(buf[4], buf[5]);
	    Serial.print("target: ");
	    Serial.print(targetX);
	    Serial.print(" ");
	    Serial.print(targetY);
	    Serial.print(" ");
	    Serial.println(targetPhi);
      Serial.println();
      targetPhi = targetPhi/180*PI;
	    i = 0;
	} else if (a == 1) { // send back current coordinate
		xSend.number = x;
		ySend.number = y;
		phiSend.number = (int) phi/PI*180.0;

		int2bytes(&xSend);
		int2bytes(&ySend);
		int2bytes(&phiSend);

		byte sendCoorArray[] = {xSend.high, xSend.low, ySend.high, ySend.low, phiSend.high, phiSend.low};	
		client.write(sendCoorArray, 6);

	} else if (a == 2) { // something else. (set current coordinate to be sth else?)

	}
 }

  /////////////////////////// Controller /////////////////////////
  if (elapsedTime0 > time_frame0) // calculate required speed
  {
    start0 = millis();

    // find dx, dy, dphi
    dx = targetX - x;
    dy = targetY - y;
    dphi = targetPhi - phi;

    time_limit = sqrt(dx * dx + dy * dy) / constant_linearVelocity;

    if (time_limit < 1.0)
    {
      time_limit = 1.0;
    }

    v_x = dx / time_limit;
    v_y = dy / time_limit;
    w = dphi / time_limit;

    /////////// Calculate require speed of 3 wheels //////////////

    // find v, v_n
    v = v_x * cos(phi_t(phi)) + v_y * sin(phi_t(phi));
    v_n = -v_x * sin(phi_t(phi)) + v_y * cos(phi_t(phi));

    // find v_0, v_1, v_2
    v0 = -v * sin(PI / 3) + v_n * cos(PI / 3) + w * rover_radius;
    v1 =              - v_n           + w * rover_radius;
    v2 =  v * sin(PI / 3) + v_n * cos(PI / 3) + w * rover_radius;

    // PID control
    motor0.Setpoint = v0; // in cm/s
    motor1.Setpoint = v1;
    motor2.Setpoint = v2;

//    Serial.print("v0: ");
//    Serial.print(v0);
//    Serial.print(", v1: ");
//    Serial.print(v1);
//    Serial.print(", v2: ");
//    Serial.println(v2);

    ////////////////////////// Integration ////////////////////////

    // Get encoder data
    displacement0 = (enc0.read() - posPrevious0); //in encoder count
    displacement1 = (enc1.read() - posPrevious1);
    displacement2 = (enc2.read() - posPrevious2);

    posPrevious0 = enc0.read(); //in encoder count
    posPrevious1 = enc1.read();
    posPrevious2 = enc2.read();

    elapsedTime1 = micros() - start1;
    start1 = micros();

    speed0 = (displacement0 / countPerRotation) * (2 * PI * wheel_radius) / (elapsedTime1 / 1000000.0); // convert to cm/s
    speed1 = (displacement1 / countPerRotation) * (2 * PI * wheel_radius) / (elapsedTime1 / 1000000.0);
    speed2 = (displacement2 / countPerRotation) * (2 * PI * wheel_radius) / (elapsedTime1 / 1000000.0);

    // Done getting data, start integration
    // recalculating v_x, v_y and w

    w = 1 / (3 * rover_radius) * (speed0 + speed1 + speed2);
    v = (sqrt(3) / 3) * (speed2 - speed0);
    v_n = 1 / 3.0 * (speed2 + speed0) - 2 / 3.0 * speed1;

    v_x = v * cos(phi_t(phi)) - v_n * sin(phi_t(phi));
    v_y = v * sin(phi_t(phi)) + v_n * cos(phi_t(phi));

    // calculate x, y phi
    x = x + v_x * elapsedTime1 / 1000000.0;
    y = y + v_y * elapsedTime1 / 1000000.0;
    phi = phi + w * elapsedTime1 / 1000000.0;

//     Serial.print("X, Y, Phi: ");
//     Serial.print(x);
//     Serial.print(", ");
//     Serial.print(y);
//     Serial.print(", ");
//     Serial.println(phi);
//     Serial.println(" ");

    // stopping condition
    if (abs(dx) < 1 && abs(dy) < 1 && abs(dphi) < 0.035)
    {
      // stop
      analogWrite(enA, 0);
      analogWrite(enB, 0);
      analogWrite(enC, 0);
      v0 = 0;
      v1 = 0;
      v2 = 0;

//      Serial.println("I'm here!");
    }
  }

  if (elapsedTime2 > time_frame2) // PID control
  {
    start2 = millis();
    // Output speed to each of the wheels
    displacementA = (enc0.read() - posPreviousA); //in encoder count
    displacementB = (enc1.read() - posPreviousB);
    displacementC = (enc2.read() - posPreviousC);

//    Serial.print(elapsedTime2 / 1000.0,10);

//    Serial.print("displacementA: ");
//    Serial.print(displacementA);
//    Serial.print(", displacementB: ");
//    Serial.print(displacementB);
//    Serial.print(", displacementC: ");
//    Serial.println(displacementC);

    posPreviousA = enc0.read(); //in encoder count
    posPreviousB = enc1.read();
    posPreviousC = enc2.read();

    speedA = (displacementA / countPerRotation) * (2 * PI * wheel_radius) / (elapsedTime2 / 1000.0); // convert to cm/s
    speedB = (displacementB / countPerRotation) * (2 * PI * wheel_radius) / (elapsedTime2 / 1000.0);
    speedC = (displacementC / countPerRotation) * (2 * PI * wheel_radius) / (elapsedTime2 / 1000.0);

//    Serial.print("speedA: ");
//    Serial.print(speedA, 10);
//    Serial.print(", speedB: ");
//    Serial.print(speedB, 10);
//    Serial.print(", speedC: ");
//    Serial.println(speedC, 10);

    motor0.Input = speedA; // in cm/s
    motor1.Input = speedB;
    motor2.Input = speedC;

    motorPID0.Compute();
    motorPID1.Compute();
    motorPID2.Compute();

    float pwm0 = motor0.Output;
    float pwm1 = motor1.Output;
    float pwm2 = motor2.Output;

    if (pwm0 > 0) {
      digitalWrite(int1, 1);
      digitalWrite(int2, 0);
      analogWrite(enA, pwm0);
    } else {
      digitalWrite(int1, 0);
      digitalWrite(int2, 1);
      analogWrite(enA, -pwm0);
    }

    if (pwm1 > 0) {
      digitalWrite(int3, 1);
      digitalWrite(int4, 0);
      analogWrite(enB, pwm1);

    } else {
      digitalWrite(int3, 0);
      digitalWrite(int4, 1);
      analogWrite(enB, -pwm1);
    }

    if (pwm2 > 0) {
      digitalWrite(int5, 1);
      digitalWrite(int6, 0);
      analogWrite(enC, pwm2);
    } else {
      digitalWrite(int5, 0);
      digitalWrite(int6, 1);
      analogWrite(enC, -pwm2);
    }

  }

}
