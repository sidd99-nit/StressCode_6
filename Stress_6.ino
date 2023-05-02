#include <Wire.h>
#include <SoftwareSerial.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;
SoftwareSerial BTSerial(10, 11);
const int BT_BAUD_RATE = 9600;

const int numReadings5 = 5;  // number of readings to average
int16_t readings5[numReadings5];   // array to store readings
int index5 = 0;              // index of the current reading
long total5 = 0;              // sum of the readings


const int numReadings10 = 10;  // number of readings to average
int16_t readings10[numReadings10];   // array to store readings
int index10 = 0;              // index of the current reading
long total10 = 0;              // sum of the readings

int16_t stressValue = 10;
int16_t prevGsrValue;
int16_t gsrValue;

float prevAvg;

int count = 0;

void setup() 
{
  Serial.begin(9600);
  BTSerial.begin(BT_BAUD_RATE);
  ads.begin();
  
  delay(6000);

  // initialize the array with 0 values
  for (int i = 0; i < numReadings5; i++) {
    readings5[i] = 0;
  }

  
  // initialize the array with 0 values
  for (int i = 0; i < numReadings10; i++) {
    readings10[i] = 0;
  }
}

void loop() 
{
  if (BTSerial.available()) 
  {
    String message = BTSerial.readString();
    if(message == "R") 
    { 
      Serial.println("Recalibrate..."); 
      setup();
    }
  }

  int pc = 0;
  
  // Read GSR sensor value and update buffer
   gsrValue = ads.readADC_SingleEnded(0);
   prevGsrValue;
    
  // subtract the oldest reading from the total
  total5 = total5 - readings5[index5];

  // subtract the oldest reading from the total
  total10 = total10 - readings10[index10];

  // add the new reading to the total
  readings5[index5] = gsrValue;
  total5 = total5 + readings5[index5];

   // add the new reading to the total
  readings10[index10] = gsrValue;
  total10 = total10 + readings10[index10];

  // increment the index
  index5 = index5 + 1;

  // increment the index
  index10 = index10 + 1;

  // if we reached the end of the array, wrap around to the beginning
  if (index5 >= numReadings5) {
    index5 = 0;
  }

  // if we reached the end of the array, wrap around to the beginning
  if (index10 >= numReadings10) {
    index10 = 0;
  }

   // calculate the average
  float average5 = total5 / numReadings5;  

   // calculate the average
  float average10 = total10 / numReadings10;  

  int diff = gsrValue - average10;

  if(diff < 0)
  {
       pc =( abs(diff) / ( (gsrValue + average10) / 2 )   ) * 100;

       pc = pc + 10;

       stressValue = stressValue * ( (100 + pc)/100 );
       
  }

  else
  {
       pc =( abs(diff) / ( (gsrValue + average10) / 2 )   ) * 100;

       pc = pc + 10;

       stressValue = stressValue * ( (100 - pc)/100 );

  }

  prevGsrValue = gsrValue;
  
  // Update stress value bounds
  if (stressValue < 10) {
    stressValue = 10;
  } 
  else if (stressValue > 100) {
    stressValue = 100;
  }

  count++;

  // Print and send stress value
  // Serial.println(average5);
  Serial.print("Average :");
  Serial.println(average10);
  Serial.print("Gsr:");
  Serial.println(prevGsrValue);

  if(count >= 10)
  {
    Serial.print("Stress:");
    Serial.println(pc);
  }
  Serial.println(".....");
  
  BTSerial.println("--------------------");
  BTSerial.print("  GSR Value: ");
  BTSerial.println(stressValue);
  BTSerial.println("--------------------");
  
  delay(2000);
}
