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
  Serial.begin(115200);
  Serial1.begin(115200);
  
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
    j = client.connect(server, 1234);
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

  while(Serial.available() > 0) {
    trash = Serial.read();
  }
  
}

void loop()
{
	if (client.available()) {
		a = client.read();

		if (a == 0)	{
			Serial.println("I got new target.");
      Serial1.write((byte) 0);
			j = 0;
			while(j<6)	{
				if (client.available()>0)	{
					buf0[j] = client.read();
					Serial.println(buf0[j]);
					j++;
				}
			}

      // Send coordinate back to Due
      for (i = 0; i<6; i++) {
        Serial1.write(buf0[i]);
      }

		} else if (a == 1)	{
			Serial.println("I have to send coordinate back.");

      // Get the coordinate from the Due
      Serial.println("Bytes received from Due: ");
      Serial1.write((byte) 1); 
      k = 0;
      while(k < 6) {
          if (Serial1.available() > 0)  {
            buf1[k] = Serial1.read();
            Serial.println(buf1[k]);
            k++;
          }      
      }

      while(Serial.available() > 0) {
        trash = Serial.read();
        trash_count++;
      }

      Serial.print("Trash count: ");
      Serial.println(trash_count);
      trash_count = 0;

      Serial.println("Bytes sent to client: ");

			for (i=0;i<6;i++)	{
				client.write(buf1[i]);
        Serial.println(buf1[i]);
			}

      client.write((byte) 0);  // fill in the blank size
      client.write((byte) 0);
		} else {
			Serial.println("Command not yet implement.");
		}
	}
}
