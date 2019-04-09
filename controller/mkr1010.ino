#include <WiFiNINA.h>
#include <SPI.h>
#include <U8glib.h>

// To connect to the server on laptop
char ssid[] = "iPhone";
char pass[] = "00000000";
int status = WL_IDLE_STATUS;

IPAddress server(172,20,10,3);
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

/////////////////////////////////////////////////// LCD DISPLAY ///////////////////////////////////////////////////
U8GLIB_NHD_C12864 u8g(9, 8, 7, 6);  // SPI Com: SCK = 9, MOSI = 8, CS = 7, CD = 6, RST = RESET

char charX[5];
char charY[5];
char charPhi[5];

char char_targetX[5];
char char_targetY[5];
char char_targetPhi[5];

char char_set_targetX[5];
char char_set_targetY[5];
char char_set_targetPhi[5];

int x = 0;
int y = 0;
int phi = 0;

int targetX = 0;
int targetY = 0;
int targetPhi = 0;

int set_targetX = 0;
int set_targetY = 0;
int set_targetPhi = 0;

void add_sign2string0()  {
  int x_fnc = x;
  int y_fnc = y;
  int phi_fnc = phi;

  if (x_fnc<0)  {
    charX[0] = 45;
  } else {
    charX[0] = 43;
  }

  if (y_fnc<0)  {
    charY[0] = 45;
  } else {
    charY[0] = 43;
  }

  if (phi_fnc<0)  {
    charPhi[0] = 45;
  } else {
    charPhi[0] = 43;
  }
}

void add_sign2string1()  {
  int targetX_fnc = targetX;
  int targetY_fnc = targetY;
  int targetPhi_fnc = targetPhi;

  if (targetX_fnc<0)  {
    char_targetX[0] = 45;
  } else {
    char_targetX[0] = 43;
  }

  if (targetY_fnc<0)  {
    char_targetY[0] = 45;
  } else {
    char_targetY[0] = 43;
  }

  if (targetPhi_fnc<0)  {
    char_targetPhi[0] = 45;
  } else {
    char_targetPhi[0] = 43;
  }
}

void add_sign2string2()  {
  int set_targetX_fnc = set_targetX;
  int set_targetY_fnc = set_targetY;
  int set_targetPhi_fnc = set_targetPhi;

  if (set_targetX_fnc<0)  {
    char_set_targetX[0] = 45;
  } else {
    char_set_targetX[0] = 43;
  }

  if (set_targetY_fnc<0)  {
    char_set_targetY[0] = 45;
  } else {
    char_set_targetY[0] = 43;
  }

  if (set_targetPhi_fnc<0)  {
    char_set_targetPhi[0] = 45;
  } else {
    char_set_targetPhi[0] = 43;
  }
}

void convert_int2string0() {
  int x_fnc = x;
  int y_fnc = y;
  int phi_fnc = phi;

  if (x_fnc<0)  {
    x_fnc = x_fnc * -1;
  }
  
  charX[1] = x_fnc/100 + 48;
  x_fnc = x_fnc - x_fnc/100*100;
  charX[2] = x_fnc/10 + 48;
  x_fnc = x_fnc - x_fnc/10*10;
  charX[3] = x_fnc + 48;
  charX[4] = '\0';    

  if (y_fnc<0)  {
    y_fnc = y_fnc * -1;
  }

  charY[1] = y_fnc/100 + 48;
  y_fnc = y_fnc - y_fnc/100*100;
  charY[2] = y_fnc/10 + 48;
  y_fnc = y_fnc - y_fnc/10*10;
  charY[3] = y_fnc + 48;
  charY[4] = '\0'; 

  if (phi_fnc<0)  {
    phi_fnc = phi_fnc * -1;
  }
  
  charPhi[1] = phi_fnc/100 + 48;
  phi_fnc = phi_fnc - phi_fnc/100*100;
  charPhi[2] = phi_fnc/10 + 48;
  phi_fnc = phi_fnc - phi_fnc/10*10;
  charPhi[3] = phi_fnc + 48;
  charPhi[4] = '\0'; 

}

void convert_int2string1() {
  int targetX_fnc = targetX;
  int targetY_fnc = targetY;
  int targetPhi_fnc = targetPhi;

  if (targetX_fnc<0)  {
    targetX_fnc = targetX_fnc * -1;
  }
  
  char_targetX[1] = targetX_fnc/100 + 48;
  targetX_fnc = targetX_fnc - targetX_fnc/100*100;
  char_targetX[2] = targetX_fnc/10 + 48;
  targetX_fnc = targetX_fnc - targetX_fnc/10*10;
  char_targetX[3] = targetX_fnc + 48;
  char_targetX[4] = '\0';    

  if (targetY_fnc<0)  {
    targetY_fnc = targetY_fnc * -1;
  }

  char_targetY[1] = targetY_fnc/100 + 48;
  targetY_fnc = targetY_fnc - targetY_fnc/100*100;
  char_targetY[2] = targetY_fnc/10 + 48;
  targetY_fnc = targetY_fnc - targetY_fnc/10*10;
  char_targetY[3] = targetY_fnc + 48;
  char_targetY[4] = '\0'; 

  if (targetPhi_fnc<0)  {
    targetPhi_fnc = targetPhi_fnc * -1;
  }
  
  char_targetPhi[1] = targetPhi_fnc/100 + 48;
  targetPhi_fnc = targetPhi_fnc - targetPhi_fnc/100*100;
  char_targetPhi[2] = targetPhi_fnc/10 + 48;
  targetPhi_fnc = targetPhi_fnc - targetPhi_fnc/10*10;
  char_targetPhi[3] = targetPhi_fnc + 48;
  char_targetPhi[4] = '\0'; 

}

void convert_int2string2() {
  int set_targetX_fnc = set_targetX;
  int set_targetY_fnc = set_targetY;
  int set_targetPhi_fnc = set_targetPhi;

  if (set_targetX_fnc<0)  {
    set_targetX_fnc = set_targetX_fnc * -1;
  }
  
  char_set_targetX[1] = set_targetX_fnc/100 + 48;
  set_targetX_fnc = set_targetX_fnc - set_targetX_fnc/100*100;
  char_set_targetX[2] = set_targetX_fnc/10 + 48;
  set_targetX_fnc = set_targetX_fnc - set_targetX_fnc/10*10;
  char_set_targetX[3] = set_targetX_fnc + 48;
  char_set_targetX[4] = '\0';    

  if (set_targetY_fnc<0)  {
    set_targetY_fnc = set_targetY_fnc * -1;
  }

  char_set_targetY[1] = set_targetY_fnc/100 + 48;
  set_targetY_fnc = set_targetY_fnc - set_targetY_fnc/100*100;
  char_set_targetY[2] = set_targetY_fnc/10 + 48;
  set_targetY_fnc = set_targetY_fnc - set_targetY_fnc/10*10;
  char_set_targetY[3] = set_targetY_fnc + 48;
  char_set_targetY[4] = '\0'; 

  if (set_targetPhi_fnc<0)  {
    set_targetPhi_fnc = set_targetPhi_fnc * -1;
  }
  
  char_set_targetPhi[1] = set_targetPhi_fnc/100 + 48;
  set_targetPhi_fnc = set_targetPhi_fnc - set_targetPhi_fnc/100*100;
  char_set_targetPhi[2] = set_targetPhi_fnc/10 + 48;
  set_targetPhi_fnc = set_targetPhi_fnc - set_targetPhi_fnc/10*10;
  char_set_targetPhi[3] = set_targetPhi_fnc + 48;
  char_set_targetPhi[4] = '\0'; 

}

void draw0(void) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_5x8);

  u8g.setFontPosTop();
  u8g.drawStr((64-u8g.getStrWidth("Current Pos"))/2, 0, "Current Pos");
  u8g.drawStr(5, 10, "X: ");
  u8g.drawStr(u8g.getStrWidth("X: ") + 1, 10, charX);
  u8g.drawStr(5, 20, "Y: ");
  u8g.drawStr(u8g.getStrWidth("Y: ") + 1, 20, charY);
  u8g.drawStr(5, 30, "Phi: ");
  u8g.drawStr(u8g.getStrWidth("Phi: ") + 1, 30, charPhi);

  u8g.drawStr((64-u8g.getStrWidth("Target"))/2 + 64, 0, "Target");
  u8g.drawStr(70, 10, "X: ");
  u8g.drawStr(u8g.getStrWidth("X: ") + 66, 10, char_targetX);
  u8g.drawStr(70, 20, "Y: ");
  u8g.drawStr(u8g.getStrWidth("Y: ") + 66, 20, char_targetY);
  u8g.drawStr(70, 30, "Phi: ");
  u8g.drawStr(u8g.getStrWidth("Phi: ") + 66, 30, char_targetPhi);

  u8g.drawStr((128 - u8g.getStrWidth("Run, Forrest, Run"))/2, 47, "Run, Forrest, Run!!");
  
  u8g.drawFrame(0,0,64,40);
  u8g.drawFrame(64,0,64,40);
  u8g.drawFrame(0,40,128,24);

}

void draw1(void) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_5x8);

  u8g.setFontPosTop();
  u8g.drawStr((64-u8g.getStrWidth("Current Pos"))/2, 0, "Current Pos");
  u8g.drawStr(5, 10, "X: ");
  u8g.drawStr(u8g.getStrWidth("X: ") + 1, 10, charX);
  u8g.drawStr(5, 20, "Y: ");
  u8g.drawStr(u8g.getStrWidth("Y: ") + 1, 20, charY);
  u8g.drawStr(5, 30, "Phi: ");
  u8g.drawStr(u8g.getStrWidth("Phi: ") + 1, 30, charPhi);

  u8g.drawStr((64-u8g.getStrWidth("Target"))/2 + 64, 0, "Target");
  u8g.drawStr(70, 10, "X: ");
  u8g.drawStr(u8g.getStrWidth("X: ") + 66, 10, char_set_targetX);
  u8g.setFont(u8g_font_m2icon_5);
  u8g.drawStr(u8g.getStrWidth("X: ") + u8g.getStrWidth(char_set_targetX) + 66 + 38, 16, "\x61");
  u8g.setFont(u8g_font_5x8);
  u8g.drawStr(70, 27, "Y: ");
  u8g.drawStr(u8g.getStrWidth("Y: ") + 66, 27, char_set_targetY);
  u8g.drawStr(70, 37, "Phi: ");
  u8g.drawStr(u8g.getStrWidth("Phi: ") + 66, 37, char_set_targetPhi);

  u8g.drawStr((128 - u8g.getStrWidth("Run, Forrest, Run"))/2, 54, "Run, Forrest, Run!!");
  
  u8g.drawFrame(0,0,64,40);
  u8g.drawFrame(64,0,64,40);
  u8g.drawFrame(0,40,128,24);

}

void draw2(void) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_5x8);

  u8g.setFontPosTop();
  u8g.drawStr((64-u8g.getStrWidth("Current Pos"))/2, 0, "Current Pos");
  u8g.drawStr(5, 10, "X: ");
  u8g.drawStr(u8g.getStrWidth("X: ") + 1, 10, charX);
  u8g.drawStr(5, 20, "Y: ");
  u8g.drawStr(u8g.getStrWidth("Y: ") + 1, 20, charY);
  u8g.drawStr(5, 30, "Phi: ");
  u8g.drawStr(u8g.getStrWidth("Phi: ") + 1, 30, charPhi);

  u8g.drawStr((64-u8g.getStrWidth("Target"))/2 + 64, 0, "Target");
  u8g.drawStr(70, 10, "X: ");
  u8g.drawStr(u8g.getStrWidth("X: ") + 66, 10, char_set_targetX);
  u8g.setFont(u8g_font_5x8);
  u8g.drawStr(70, 20, "Y: ");
  u8g.drawStr(u8g.getStrWidth("Y: ") + 66, 20, char_set_targetY);
  u8g.setFont(u8g_font_m2icon_5);
  u8g.drawStr(u8g.getStrWidth("Y: ") + u8g.getStrWidth(char_set_targetY) + 66 + 38, 26, "\x61");
  u8g.setFont(u8g_font_5x8);
  u8g.drawStr(70, 37, "Phi: ");
  u8g.drawStr(u8g.getStrWidth("Phi: ") + 66, 37, char_set_targetPhi);

  u8g.drawStr((128 - u8g.getStrWidth("Run, Forrest, Run"))/2, 54, "Run, Forrest, Run!!");
  
  u8g.drawFrame(0,0,64,40);
  u8g.drawFrame(64,0,64,40);
  u8g.drawFrame(0,40,128,24);

}

void draw3(void) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_5x8);

  u8g.setFontPosTop();
  u8g.drawStr((64-u8g.getStrWidth("Current Pos"))/2, 0, "Current Pos");
  u8g.drawStr(5, 10, "X: ");
  u8g.drawStr(u8g.getStrWidth("X: ") + 1, 10, charX);
  u8g.drawStr(5, 20, "Y: ");
  u8g.drawStr(u8g.getStrWidth("Y: ") + 1, 20, charY);
  u8g.drawStr(5, 30, "Phi: ");
  u8g.drawStr(u8g.getStrWidth("Phi: ") + 1, 30, charPhi);

  u8g.drawStr((64-u8g.getStrWidth("Target"))/2 + 64, 0, "Target");
  u8g.drawStr(70, 10, "X: ");
  u8g.drawStr(u8g.getStrWidth("X: ") + 66, 10, char_set_targetX);
  u8g.setFont(u8g_font_5x8);
  u8g.drawStr(70, 20, "Y: ");
  u8g.drawStr(u8g.getStrWidth("Y: ") + 66, 20, char_set_targetY);
  u8g.drawStr(70, 30, "Phi: ");
  u8g.drawStr(u8g.getStrWidth("Phi: ") + 66, 30, char_set_targetPhi);
  u8g.setFont(u8g_font_m2icon_5);
  u8g.drawStr(u8g.getStrWidth("Phi: ") + u8g.getStrWidth(char_set_targetPhi) + 66 + 38, 37, "\x61");
  u8g.setFont(u8g_font_5x8);

  u8g.drawStr((128 - u8g.getStrWidth("Run, Forrest, Run"))/2, 54, "Run, Forrest, Run!!");
  
  u8g.drawFrame(0,0,64,40);
  u8g.drawFrame(64,0,64,40);
  u8g.drawFrame(0,40,128,24);

}

void draw4(void) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_5x8);

  u8g.setFontPosTop();
  u8g.drawStr((64-u8g.getStrWidth("Current Pos"))/2, 0, "Current Pos");
  u8g.drawStr(5, 10, "X: ");
  u8g.drawStr(u8g.getStrWidth("X: ") + 1, 10, charX);
  u8g.drawStr(5, 20, "Y: ");
  u8g.drawStr(u8g.getStrWidth("Y: ") + 1, 20, charY);
  u8g.drawStr(5, 30, "Phi: ");
  u8g.drawStr(u8g.getStrWidth("Phi: ") + 1, 30, charPhi);

  u8g.drawStr((64-u8g.getStrWidth("Target"))/2 + 64, 0, "Target");
  u8g.drawStr(70, 10, "X: ");
  u8g.drawStr(u8g.getStrWidth("X: ") + 66, 10, char_set_targetX);
  u8g.setFont(u8g_font_5x8);
  u8g.drawStr(70, 20, "Y: ");
  u8g.drawStr(u8g.getStrWidth("Y: ") + 66, 20, char_set_targetY);
  u8g.drawStr(70, 30, "Phi: ");
  u8g.drawStr(u8g.getStrWidth("Phi: ") + 66, 30, char_set_targetPhi);

  u8g.drawStr(u8g.getStrWidth("OK?") + (28 - u8g.getStrWidth("OK?")), 47, "OK?");
  u8g.drawStr(u8g.getStrWidth("Cancel?") + (72 - u8g.getStrWidth("Cancel")), 47, "Cancel?");
  u8g.setFont(u8g_font_m2icon_5);
  u8g.drawStr(u8g.getStrWidth("OK?") + (28 - u8g.getStrWidth("OK?")) + 2*u8g.getStrWidth("\x61"), 54, "\x61");
  u8g.setFont(u8g_font_5x8);
  
  u8g.drawFrame(0,0,64,40);
  u8g.drawFrame(64,0,64,40);
  u8g.drawFrame(0,40,128,24);

}

void draw5(void) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_5x8);

  u8g.setFontPosTop();
  u8g.drawStr((64-u8g.getStrWidth("Current Pos"))/2, 0, "Current Pos");
  u8g.drawStr(5, 10, "X: ");
  u8g.drawStr(u8g.getStrWidth("X: ") + 1, 10, charX);
  u8g.drawStr(5, 20, "Y: ");
  u8g.drawStr(u8g.getStrWidth("Y: ") + 1, 20, charY);
  u8g.drawStr(5, 30, "Phi: ");
  u8g.drawStr(u8g.getStrWidth("Phi: ") + 1, 30, charPhi);

  u8g.drawStr((64-u8g.getStrWidth("Target"))/2 + 64, 0, "Target");
  u8g.drawStr(70, 10, "X: ");
  u8g.drawStr(u8g.getStrWidth("X: ") + 66, 10, char_set_targetX);
  u8g.setFont(u8g_font_5x8);
  u8g.drawStr(70, 20, "Y: ");
  u8g.drawStr(u8g.getStrWidth("Y: ") + 66, 20, char_set_targetY);
  u8g.drawStr(70, 30, "Phi: ");
  u8g.drawStr(u8g.getStrWidth("Phi: ") + 66, 30, char_set_targetPhi);

  u8g.drawStr(u8g.getStrWidth("OK?") + (28 - u8g.getStrWidth("OK?")), 47, "OK?");
  u8g.drawStr(u8g.getStrWidth("Cancel?") + (72 - u8g.getStrWidth("Cancel")), 47, "Cancel?");
  u8g.setFont(u8g_font_m2icon_5);
  u8g.drawStr(u8g.getStrWidth("Cancel?") + (96 - u8g.getStrWidth("Cancel?")) + 2*u8g.getStrWidth("\x61") + 2, 54, "\x61");
  u8g.setFont(u8g_font_5x8);
  
  u8g.drawFrame(0,0,64,40);
  u8g.drawFrame(64,0,64,40);
  u8g.drawFrame(0,40,128,24);

}

/////////////////////////////////////////////// LCD BUTTON ////////////////////////////////////////////////////
int button_reading;
int state = 0;

int elapsedTime = 0;
long start = 0;

//////////////////////////////////////////// SETUP AND LOOP //////////////////////////////////////////

void setup() {
  // put your setup code here, to run once:
  Serial1.begin(115200);

  pinMode(A0,INPUT);
  start = millis();
  u8g.setContrast(0); // Config the contrast to the best effect
  u8g.setRot180();// rotate screen, if required
  // set SPI backup if required
  //u8g.setHardwareBackup(u8g_backup_avr_spi);

  // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }
  
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
	elapsedTime = millis() - start;
  if (elapsedTime > 200)  {
    start = millis();
    button_reading = analogRead(A0);

    // change the state
    if (800<button_reading && button_reading<900) { // up
      state--;
    } else if (350<button_reading && button_reading<450)  { // down
      state++;
    }

    if (state<0)  {
      state = 0;
    }

    if (state<0)  {
      state = 0;
    }

    // change the target
    if (state%6 == 1) { // change target X
      if (600<button_reading && button_reading<650) { // right
        set_targetX++;
      } else if (button_reading<50)  { // left
        set_targetX--;
      }
    } else if (state%6 == 2)  { // change target Y
      if (600<button_reading && button_reading<650) { // right
        set_targetY++;
      } else if (button_reading<50)  { // left
        set_targetY--;
      }
    } else if (state%6 == 3)  { // change target Phi
      if (600<button_reading && button_reading<650) { // right
        set_targetPhi++;
      } else if (button_reading<50)  { // left
        set_targetPhi--;
      }
    }

    // OK with the target or Cancel (reset) the target
    if (state%6 == 4) {
      if (170<button_reading && button_reading<250) { // Press the button
        targetX = set_targetX;
        targetY = set_targetY;
        targetPhi = set_targetPhi;

        set_targetX = 0;
        set_targetY = 0;
        set_targetPhi = 0;

        state = 0;
      }
    } else if(state%6 == 5) {
      if (170<button_reading && button_reading<250) { // Press the button
        set_targetX = 0;
        set_targetY = 0;
        set_targetPhi = 0;

        state = 0;
      }
    }

  }
  
  convert_int2string0();
  add_sign2string0();
  convert_int2string1();
  add_sign2string1();
  convert_int2string2();
  add_sign2string2();
  
  // picture loop
  u8g.firstPage();  

  do {
    if (state%6 == 0) {
      draw0();
    } else if (state%6 == 1)  {
      draw1();
    } else if (state%6 == 2) {
      draw2();
    } else if (state%6 == 3) {
      draw3();
    } else if (state%6 == 4) {
      draw4();
    } else if (state%6 == 5) {
      draw5();
    }
    
  } 
  while( u8g.nextPage() );

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

			targetX = twobytes1int(buf0[0], buf0[1]);
			targetY = twobytes1int(buf0[2], buf0[3]);
			targetPhi = twobytes1int(buf0[4], buf0[5]);

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
