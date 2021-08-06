#include <AccelStepper.h>

#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SSB_READ 7 // START STOP BUTTON PIN 6
#define MODEB_READ 5 // MODE BUTTON PIN 8
#define UPB_READ 8 // UP BUTTON PIN (SPEED / ANGLE)5
#define DOWNB_READ  6 // DOWN BUTTON PIN (SPEED / ANGLE) 7

int  incrementor = 25;
int  angler  =15;

byte SSB = 0; // Start / Stop button Variable
byte MODEB = 0; // Mode button variable
byte UPB = 0; // Up button variable
byte DOWNB = 0; // Down button variable
byte MODE = 1; // Mode button variable
int speeder = 200; // Default speed.

int stepsfromangle = 0;
int angleincrement = 5;
int stepsper5deg = 63;
int maxspeeder = 1000;
int maxangle = 360;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
AccelStepper stepper(AccelStepper::DRIVER, 10, 11);
void updatescreen(String s1, String s2 ="",String s3 ="", int ts = 2);

void setup()
{ 
  pinMode(SSB_READ, INPUT);
  pinMode(MODEB_READ, INPUT);
  pinMode(UPB_READ, INPUT);
  pinMode(DOWNB_READ, INPUT);
 
  stepper.setEnablePin(9);
  stepper.setPinsInverted(false,false,true);
  stepper.disableOutputs();
  stepper.setMaxSpeed(maxspeeder);
  stepper.setAcceleration(1500);
  Serial.begin(115200); //Open Serial connection for debugging
  Serial.println("Begin motor control");
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.display();
  updatescreen("Rotary Table","SW Rev. 08-01-21","HW Rev. 08-01-21",1);
}

void loop()
{
//Serial.println("Reading Buttons...");
readbuttons();
  
}
void gotoangle(int angletarg)

{
  stepsfromangle = angler * stepsper5deg / 5;
  int newpos = stepper.currentPosition()+stepsfromangle;
  stepper.enableOutputs();
 // delay(50);
  while (stepper.currentPosition() != newpos)
 
{
 if (stepper.distanceToGo() == 0)
  {
    stepper.moveTo(newpos);
  }
  stepper.run();   
}
  stepper.disableOutputs();
  delay(50);
  updatescreen("Tot.Angle:",String(stepper.currentPosition()/stepsper5deg*5));
 }

void gotospeed (long speed1)
{
  while (digitalRead(SSB_READ)== 0)
  {
  stepper.enableOutputs();
  stepper.setSpeed(speed1);
  stepper.runSpeed();
  }
  //Serial.println("Completed speed...");
  stepper.disableOutputs();
  delay(250);
}

void readbuttons()
{
  int SSB_LAST = SSB;
  int MODEB_LAST = MODEB;
  int UPB_LAST = UPB;
  int DOWNB_LAST = DOWNB;
  
  SSB = digitalRead(SSB_READ);
  MODEB = digitalRead(MODEB_READ);
  UPB = digitalRead(UPB_READ);
  DOWNB = digitalRead(DOWNB_READ);

  if (SSB == 1)
  {
    while (digitalRead(SSB_READ) == 1)
    {
    delay(0);
    }
    switch(MODE)
    {
      case 1:
      delay(250);
      updatescreen("Speed:",String(speeder));
      gotospeed(speeder);
      break;
      case 2:
      gotoangle(angler);
      break;
    }
  }
    if (MODEB == 1)
  {
     switch(MODE)
    {
      case 1:
      
      delay(250);
      MODE = 2;
      stepper.setCurrentPosition(0);
      updatescreen("Mode:" ,String(MODE));
      break;
      case 2:
      
      delay(250);
      MODE=1;
      updatescreen("Mode:" ,String(MODE));
      break;  
    }
  }
    if (UPB==1)
  {
     switch(MODE)
    {
      case 1:
      speeder = speeder + incrementor;
            if (speeder > maxspeeder)
      {
        speeder = maxspeeder;
      }
      updatescreen("Speed:", String(speeder));
      delay(250);
      break;
      case 2:
      angler = angler + angleincrement;
       if (angler > maxangle)
      {
        angler= maxangle;
      }
      stepper.setCurrentPosition(0);
      updatescreen("Angle:",String(angler));
      delay(250);
      break;
    }
  }
    if (DOWNB==1)
  {
     switch(MODE)
    {
       case 1:
      speeder = speeder - incrementor;
      if (speeder < -maxspeeder)
      {
        speeder = -maxspeeder;
      }
      
      updatescreen("Speed:", String(speeder));
      delay(250);
      break;
      case 2:
      angler = angler - angleincrement;
       if (angler < -maxangle)
      {
        angler= -maxangle;
      }
      stepper.setCurrentPosition(0);
      updatescreen("Angle:", String(angler));
      delay(250);
      break;
    }
  }
}

void updatescreen(String s1, String s2 ="",String s3 ="", int ts = 2)
{
  byte ls1 = s1.length();
  byte ls2 = s2.length();
  byte ls3 = s3.length();  
  display.clearDisplay();
  display.setTextSize(ts); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(64-(((6*ts*ls1)-ts)/2), 0);
  display.println(s1); 
  display.setCursor(64-(((6*ts*ls2)-ts)/2), 18);
  display.println(s2);
  display.setCursor(64-(((6*ts*ls3)-ts)/2), 36);
  display.println(s3);
  display.display();     
}
