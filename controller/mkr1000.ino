#include <WiFi101.h>
#include <SPI.h>

// To connect to the server on laptop
char ssid[] = "iPhone";
char pass[] = "00000000";
int status = WL_IDLE_STATUS;

IPAddress server(172,20,10,3);
WiFiClient client;


// Random variable
int a, i, j, k, m;
byte buf0[7];
byte buf1[7];
byte buf2[7];

long start = millis();
int elapsedTime = 0;
int timeout = 0;

void setup() {
  // put your setup code here, to run once:
  Serial1.begin(115200);

  
//  status = WiFi.begin(ssid, pass);

  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
  }

  delay(10000);
  
  j = client.connect(server, 1234);
  while (j != 1)  {
    j = client.connect(server, 1234);
  }

}

void loop()
{
	if (client.available()) {   
		a = client.read();
		if (a == 0)	{ // Send new target to Due
      j = 0;
      start = millis();
			while(j<6)	{
        elapsedTime = millis() - start;

        if (elapsedTime > 1000) {
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
      start = millis();
      while(k < 6) {
        elapsedTime = millis() - start; 

        if (elapsedTime > 1000) {
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

		} else if (a == 2) { // set the current coordinnate to be something else.
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

    } else if (a == 3) { // identify yourself
      client.write((byte) 1);
    }
	}
}
