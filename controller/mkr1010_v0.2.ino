#include <WiFiNINA.h>
#include <SPI.h>

// To connect to the server on laptop
char ssid[] = "iPhone";
char pass[] = "00000000";
int status = WL_IDLE_STATUS;

IPAddress server(172,20,10,3);
WiFiClient client;


// Random variable
int a, i, j, k;
int trash_count = 0;
byte trash;
byte buf0[7];
byte buf1[7];

void setup() {
  // put your setup code here, to run once:
  Serial1.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  
  status = WiFi.begin(ssid, pass);
  if (status != WL_CONNECTED) {
    while (status != WL_CONNECTED) {
      status = WiFi.begin(ssid, pass);
    }
  } else  {
    j = client.connect(server, 1234);
    while (j != 1)  {
      j = client.connect(server, 1234);
    }
  }

  while(Serial1.available() > 0) {
    trash = Serial1.read();
  }
}

void loop()
{
	if (client.available()) {
		a = client.read();

		if (a == 0)	{ // Send new target to Due
      j = 0;
			while(j<6)	{
				if (client.available()>0)	{
					buf0[j] = client.read();
					j++;
				}
			}

      Serial1.write((byte) 0);
      // Send coordinate back to Due
      for (i = 0; i<6; i++) {
        Serial1.write(buf0[i]);
      }

		} else if (a == 1)	{

      // Get the coordinate from the Due
      Serial1.write((byte) 1);
      k = 0;
      while(k < 6) {
        if (Serial1.available() > 0)  {
          buf1[k] = Serial1.read();
          k++;
        }      
      }

			for (i=0;i<6;i++)	{
				client.write(buf1[i]);
			}

      client.write((byte) 0);  // fill in the blank size
      client.write((byte) 0);
		} else {
		}
	}
}
