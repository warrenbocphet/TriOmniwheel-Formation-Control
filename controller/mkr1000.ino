#include <WiFi101.h>
//#include <WiFiNINA.h>

// To connect to the server on laptop
char ssid[] = "EEELAB";
char pass[] = "@adelaide";
//char ssid[] = "iPhone";
//char pass[] = "00000000";
int status = WL_IDLE_STATUS;

//IPAddress server(129,127,225,77);  //EEELAB - window
//IPAddress server(172,20,10,3);     //iPhone hotspot
IPAddress server(129,127,225,25);    //EEELAB - linux
WiFiClient client;

///////////////////////// communication /////////////////////
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

void int2bytes(struct byteInt* number_fnc)  {
  int magnituteNumber;
  if (number_fnc->number >= 0)  {
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
int a, i, j, k, m, n;
byte buf0[7]; // store target from host
byte buf1[7]; // store coordinate from Due
byte buf2[7]; // store start0ing point from host
byte buf3[7]; // store new coordinate from host

long start0 = millis(); // timeout timing
int elapsedTime0 = 0; 
int timeout = 0;


//////////////////////////////////////////// SETUP AND LOOP //////////////////////////////////////////

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(115200);

  // Setup wifi connection
  Serial.println("Connecting to wifi.");
  while (status != WL_CONNECTED) {
    Serial.println("Can't connect to wifi, try again.");
    status = WiFi.begin(ssid, pass);
  }

  delay(10000); // If don't include this, it won't work
                // don't know why, can anyone figure out??
  
  // Setup server connection
  Serial.println("Connecting to server.");
  j = client.connect(server, 1234);
  while (j != 1)  {
    Serial.println("Can't connect to server, try again.");
    j = client.connect(server, 1234);
  }

}

void loop()
{
  // Communicate with server and Due
	if (client.available()) {   
		a = client.read();
		if (a == 0)	{ // Send new target to Due
      j = 0;
      start0 = millis();
			while(j<6)	{
        elapsedTime0 = millis() - start0;

        if (elapsedTime0 > 1000) {
          timeout = 1;
          break;
        }
        
				if (client.available()>0)	{
					buf0[j] = client.read();
					j++;        
				}
			}

     if (timeout != 1)  {
      Serial1.write((byte) 0);
      // Send coordinate back to Due
      for (i = 0; i<6; i++) {
        Serial1.write(buf0[i]);
      }

     } else  {
      timeout = 0;
     }

		} else if (a == 1)	{
      // Get the coordinate from the Due
      Serial1.write((byte) 1);
      k = 0;
      start0 = millis();
      while(k < 6) {
        elapsedTime0 = millis() - start0; 

        if (elapsedTime0 > 1000) {
          timeout = 1;
          break;
        }
               
        if (Serial1.available() > 0)  {
          buf1[k] = Serial1.read();
          k++;          
        }      
      }

      if (timeout != 1) {
        client.write(buf1,6);
        client.write((byte) 0);  // fill in the blank size
        client.write((byte) 0);

      } else {
        timeout = 0;
      }

		} else if (a == 2) { // set the new starting point
      m = 0;
      while(m<6)  {
        if (client.available()>0) {
          buf2[m] = client.read();
          m++;
        }
      }

      Serial1.write((byte) 2);
      // Send coordinate back to Due
      for (i = 0; i<6; i++) {
        Serial1.write(buf2[i]);
      }

    } else if (a == 3) { // set the new coordinate
      n = 0;
      while(n<6)  {
        if (client.available()>0) {
          buf3[n] = client.read();
          n++;
        }
      }

      Serial1.write((byte) 3);
      // Send coordinate back to Due
      for (i = 0; i<6; i++) {
        Serial1.write(buf3[i]);
      }

    } else if (a == 4) { // Identify yourself
      client.write((byte) 4);
    }
	}
}
