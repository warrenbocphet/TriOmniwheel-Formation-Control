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

// Random variable
int i = 0, j = 0, k = 0;
int a;
byte trash;
byte buf0[7];
byte buf1[7];

float targetX = 50.0, targetY = -50.0, targetPhi = -360.0;


////////////////////////// setup and loop /////////////////////////
void setup() {
	SerialUSB.begin(115200);
	Serial.begin(115200);
}


void loop() {
	// Serial communication
	if (Serial.available() > 0)	{
		SerialUSB.println();
		a = Serial.read();
		if (a == 0)	{ // set target
			SerialUSB.println("They gave me new target.");
			j = 0;
			while (j<6)	{
				if (Serial.available())	{
					buf0[j] = Serial.read();
          SerialUSB.println(buf0[j]);
					j++;					
				}
			}

			targetX = twobytes1int(buf0[0], buf0[1]);
			targetY = twobytes1int(buf0[2], buf0[3]);
			targetPhi = twobytes1int(buf0[4], buf0[5]);

			SerialUSB.println("In human language:");
			SerialUSB.print(targetX);
			SerialUSB.print(", ");
			SerialUSB.print(targetY);
			SerialUSB.print(", ");
			SerialUSB.println(targetPhi);

      while (Serial.available() > 0)  {
        trash = Serial.read();
      }
      
			SerialUSB.println("I have done reading.");

		} else if (a == 1)	{ // ask coordinate
			SerialUSB.println("They asked for my coordinate.");

			xSend.number = (int) 50;
			ySend.number = (int) 300;
			phiSend.number = (int) -360; 

			int2bytes(&xSend);
			int2bytes(&ySend);
			int2bytes(&phiSend);

			buf1[0] = xSend.high;
			buf1[1] = xSend.low;
			buf1[2] = ySend.high;
			buf1[3] = ySend.low;
			buf1[4] = phiSend.high;
			buf1[5] = phiSend.low;

			SerialUSB.println("Here's the result bytes: ");

			for (i = 0; i<6; i++)	{
				Serial.write(buf1[i]);
				SerialUSB.println(buf1[i]);
			}

			SerialUSB.println("I have done sending.");

		} else {
			SerialUSB.println("Not yet implemented.");
		}
	}
}
