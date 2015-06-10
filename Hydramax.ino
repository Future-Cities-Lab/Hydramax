
//hydramax _ updated 3-1-12
int Col = 1; // Choose which Arduino 1 is A, 2 is B, 3 is C

#define BAUDRATE 9600          
#include "Tlc5940.h" // LED chip

float deltaTime = 15.0;            // 15 works well _ milliseconds; Delay in time

const boolean NOISE = true;
const int numReadings = 15; // 15 works well
const int SAMPLELENGTH = 5000;

int sensorCutOff = 120;//80 // 230 works well _ The sensorCutOff is the low value that will trigger the motors 
int sensorConMin = 80; //50 low constrain value (should be same as above)
int sensorConMax = 300;//300 // high constrain value
//int sensorMapMin = 150; // low map value for Migas
//int sensorMapMax = 255; // high map value for Migas
int migaHigh = 255; 
int ledMin = 200;
int ledMax = 4095;

int sensorBuffer;
int sensorBufferFactor = 0.1;

int ledArray[16] = {
  1,0,3,2,5,4,7,6,9,8,11,10,13,12,15,14                      }; // positions 0-5
//*******MOTOR ARRAY A-B*********//
int migaArray[8] = {3,2,5,4,7,6,10,8}; // positions 0-5
//*******MOTOR ARRAY C************//
//int migaArray[8] = {2,4,3,6,5,7,8,10}; // positions 0-5
int sensorCount = 0;
int sensorPin[16] = {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15}; // positions 0-15
int sensorValue[16] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0                      }; // positions 0-8
int readings[16][numReadings]; // for sensors
int total[16] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0                      };
int index = 0;  
int ledVal[16];
int currentLedVal[16]= {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0                      };
int migaVal[8];
int seriesFactor = 2; //defines upper map value for sequence triggers
int ledStep = 50;

int sensorMin[16] = {
  1025,1025,1025,1025,1025,1025,1025,1025,1025,1025,1025,1025,1025,1025,1025,1025                      };
int sensorMax[16] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0                      };

boolean START = true;

void setup() // ********************************************************
{
  if (Col == 1) {
    sensorCount = 9; //9
  }
  if (Col == 2) {
    sensorCount = 16; //16
  }
  if (Col == 3) {
    sensorCount = 9; //9
  }

  Serial.begin(BAUDRATE);
  Tlc.init(); //Initialize TLC functions for LEDs
  
  
blinkHello(5); 

learn();

  
  
blinkHello(5);
}

void loop() { // ********************************************************

  // Take sensor reading from input and smooth  
  
    if (START) {
     for (int i = 0; i < sensorCount; i++) {
    for (int thisReading = 0; thisReading < numReadings; thisReading++){
      readings[i][thisReading] = sensorMin[i];
      total[i] = total[i] + sensorMin[i];
     // Serial.println(sensorMin[i]);
  }
  
  }
   
    }
    for (int i = 0; i < sensorCount; i ++) {

      total[i]= total[i] - readings[i][index];         
      readings[i][index] = analogRead(sensorPin[i]); 
      total[i]= total[i] + readings[i][index];       
      sensorValue[i] = total[i] / numReadings;
      
      
    
}
    
    
    index = index + 1;                    
    if (index >= numReadings)            
      index = 0;   
    
    /*
    for (int i = 0; i < sensorCount; i ++) { 
      sensorValue[i] =  analogRead(sensorPin[i]);
    }*/
    //testSensorConstraints();
  

  // LEDS & MOTORS *************************************************

  // *** COLUMN A *************************************************
if (NOISE) { 
        //Serial.print(8);
        
    for (int m = 0; m < (sensorCount);m++) {
        Serial.print("\t");        
        Serial.print(sensorValue[m]);
        //delay(100);
      }
      //Serial.println("");
}
  if (Col == 1) {
    //Serial.print("Arduino = ");
    //Serial.println("A");

    // Section A-1-2 =========================================== 
    for (int i = 1; i < (sensorCount-1);i++) {
      //Serial.println(i);
      int val = sensorValue[i];
      
      //BEGIN DEBUG//
      /*if (NOISE) { 
        //Serial.print(i);
        Serial.print("\t");        
        Serial.print(val);
        //delay(100);
      }*/
      //END DEBUG
      
     // val = constrain(val, sensorConMin, sensorConMax);
     // val = map(val, sensorConMin, sensorConMax, ledMin, ledMax);
     //NEW MAP//
     sensorBuffer = (sensorMax[i] - sensorMin[i])*sensorBufferFactor;
     val = constrain(val, sensorMin[i]+sensorBuffer, sensorMax[i]-sensorBuffer);
      val = map(val, sensorMin[i]+sensorBuffer, sensorMax[i]-sensorBuffer, ledMin, ledMax);
      //NEW MAP//
      
      ledVal[i*2] = val;
      ledVal[((i*2)+1)] = val;
      if ((i>=3) & (i<=7)) {
        int migaIndex = i-3;
        //if (sensorValue[i] < sensorCutOff) {
        if (sensorValue[i] < sensorMin[i]+sensorBuffer) {

          migaVal[migaIndex*2] = 0;
          migaVal[((migaIndex*2)+1)] = 0;
        } 
        else { 
          migaVal[migaIndex*2] = migaHigh;
          migaVal[((migaIndex*2)+1)] = migaHigh;
        } 
      } 
    }
    if (sensorValue[8] >= sensorCutOff){
      int val = sensorValue[8];
       //BEGIN DEBUG//
    /* if (NOISE) { 
        //Serial.print(8);
        Serial.print("\t");        
        Serial.print(val);
        //delay(100);
      }*/
      //END DEBUG
      
     // val = constrain(val, sensorCutOff, (sensorCutOff*seriesFactor));
      //val = map(val, sensorCutOff, (sensorCutOff*seriesFactor), 1, 8);
    //NEW MAP//
    int sensorMinCutOff = ((sensorMax[8] - sensorMin[8]) * 0.1) + sensorMin[8];
    int sensorMaxCutOff = sensorMax[8] - ((sensorMax[8] - sensorMin[8]) * 0.1);
      val = constrain(val, sensorMinCutOff, sensorMaxCutOff);
      val = map(val, sensorMinCutOff, sensorMaxCutOff, 1, 8);
 //NEW MAP//
 
 
      //Serial.println(i);
      for (int i = 0; i < val; i++){
        migaVal[7-i] = migaHigh;
      }
//LED SERIES
      val = sensorValue[8];
//      val = constrain(val, sensorCutOff, (sensorCutOff*seriesFactor));
//      val = map(val, sensorCutOff, (sensorCutOff*seriesFactor), 0, (16*255));

     //val = constrain(val, sensorConMin, sensorConMax);
     // val = map(val, sensorConMin, sensorConMax, 0, (16*255));
     //NEW MAP//
      val = constrain(val, sensorMin[8], sensorMax[8]);
      val = map(val, sensorMin[8], sensorMax[8], 0, (16*255));
      //NEW MAP//
      
      for (int k = 0; k < 16;k++) {
        //Serial.println(val);
        int currentLED = 0;
        if ((val > 0) & (val < 255)){
          currentLED = map(val,0,255,0,4095);
        }
        else if (val > 255) {
          currentLED = 4095;
        }
        //Serial.println(currentLED);
        if (currentLED > ledVal[15-k]) {
          ledVal[15-k] = currentLED;
        }
        val -= 255;
      }
    }

    //Back Series Sensor
    if (sensorValue[0] >= sensorCutOff){
      int val = sensorValue[0];
       //BEGIN DEBUG//
      /*if (NOISE) { 
        //Serial.print(0);
        Serial.print("\t");        
        Serial.print(val);
        delay(300);
      }*/
      //END DEBUG
      //val = constrain(val, sensorCutOff, (sensorCutOff*seriesFactor));
      //val = map(val, sensorCutOff, (sensorCutOff*seriesFactor), 1, 8);
   //NEW MAP//
    int sensorMinCutOff = ((sensorMax[0] - sensorMin[0]) * 0.1) + sensorMin[0];
    int sensorMaxCutOff = sensorMax[0] - ((sensorMax[0] - sensorMin[0]) * 0.1);
      val = constrain(val, sensorMinCutOff, sensorMaxCutOff);
      val = map(val, sensorMinCutOff, sensorMaxCutOff, 1, 8);
 //NEW MAP//
   
      //Serial.println(i);
      /* DISABLE MOTOR FUNCTION
      for (int i = 0; i < val; i++){
        migaVal[i] = migaHigh;
      }
      */
//LED SERIES
      val = sensorValue[0];
    //  val = constrain(val, sensorCutOff, (sensorCutOff*seriesFactor));
      //val = map(val, sensorCutOff, (sensorCutOff*seriesFactor), 0, (16*255));
 //NEW MAP//
      val = constrain(val, sensorMin[0], sensorMax[0]);
      val = map(val, sensorMin[0], sensorMax[0], 0, (16*255));
      //NEW MAP//
      
   
      for (int k = 0; k < 16;k++) {
        int currentLED = 0;
        if ((val > 0) & (val < 255)){
          currentLED = map(val,0,255,0,4095);
        }
        else if (val > 255) {
          currentLED = 4095;
        }
        //Serial.println(currentLED);
        if (currentLED > ledVal[k]) {
          ledVal[k] = currentLED;
        }
        val -= 255;
      }
    } else {
       ledVal[0] = ledMin;
       ledVal[1] = ledMin; 
      }
    


    

 


  }
  
  // *** COLUMN B *************************************************    
   if (Col == 2) {
   Serial.print("Arduino = ");
   Serial.println("B");
  
    // Section A-1-2 =========================================== 
    for (int i = 1; i < 8;i++) {
      //Serial.println(i);
      int val = max (sensorValue[i],sensorValue[i+7]);;
      
      if ((i>=4) & (i<=8)) {
        int migaIndex = i-4;
        if (val < sensorCutOff) {

          migaVal[migaIndex*2] = 0;
          migaVal[((migaIndex*2)+1)] = 0;
        } 
        else { 
          migaVal[migaIndex*2] = migaHigh;
          migaVal[((migaIndex*2)+1)] = migaHigh;
        } 
      } 
      val = constrain(val, sensorConMin, sensorConMax);
      val = map(val, sensorConMin, sensorConMax, ledMin, ledMax);
      ledVal[i*2] = val;
      ledVal[((i*2)+1)] = val;
    }
    if (sensorValue[15] >= sensorCutOff){
      int val = sensorValue[15];
      val = constrain(val, sensorCutOff, (sensorCutOff*seriesFactor));
      val = map(val, sensorCutOff, (sensorCutOff*seriesFactor), 1, 8);
      //Serial.println(i);
      for (int i = 0; i < val; i++){
        migaVal[7-i] = migaHigh;
      }
//LED SERIES
      val = sensorValue[15];
      val = constrain(val, sensorCutOff, (sensorCutOff*seriesFactor));
      val = map(val, sensorCutOff, (sensorCutOff*seriesFactor), 0, (16*255));

      for (int i = 0; i < 16;i++) {
        //Serial.println(val);
        int currentLED = 0;
        if ((val > 0) & (val < 255)){
          currentLED = map(val,0,255,0,4095);
        }
        else if (val > 255) {
          currentLED = 4095;
        }
        //Serial.println(currentLED);
        if (currentLED > ledVal[15-i]) {
          ledVal[15-i] = currentLED;
        }
        val -= 255;
      }
    }

    //Back Series Sensor
    if (sensorValue[0] >= sensorCutOff){
      int val = sensorValue[0];
      val = constrain(val, sensorCutOff, (sensorCutOff*seriesFactor));
      val = map(val, sensorCutOff, (sensorCutOff*seriesFactor), 1, 8);
      //Serial.println(i);
      for (int i = 0; i < val; i++){
        migaVal[i] = migaHigh;
      }
//LED SERIES
      val = sensorValue[0];
      val = constrain(val, sensorCutOff, (sensorCutOff*seriesFactor));
      val = map(val, sensorCutOff, (sensorCutOff*seriesFactor), 0, (16*255));

      for (int i = 0; i < 16;i++) {
        //Serial.println(val);
        int currentLED = 0;
        if ((val > 0) & (val < 255)){
          currentLED = map(val,0,255,0,4095);
        }
        else if (val > 255) {
          currentLED = 4095;
        }
        //Serial.println(currentLED);
        if (currentLED > ledVal[i]) {
          ledVal[i] = currentLED;
        }
        val -= 255;
      }


    } else {
       ledVal[0] = ledMin;
       ledVal[1] = ledMin; 
      }
    

 


  }

   
   // *** COLUMN C *************************************************
   
   if (Col == 3) {
   Serial.print("Arduino = ");
   Serial.println("C");
  

    for (int i = 1; i < (sensorCount-1);i++) {
      //Serial.println(i);
      int val = sensorValue[i];
      val = constrain(val, sensorConMin, sensorConMax);
      val = map(val, sensorConMin, sensorConMax, ledMin, ledMax);
      ledVal[i*2] = val;
      ledVal[((i*2)+1)] = val;
      if ((i>=3) & (i<=7)) {
        int migaIndex = i-3;
        if (sensorValue[i] < sensorCutOff) {
          migaVal[migaIndex*2] = 0;
          if (migaIndex > 0){
          migaVal[((migaIndex*2)-1)] = 0;
          Serial.println(migaIndex);
          }
        } 
        else { 
          migaVal[migaIndex*2] = migaHigh; 
          if (migaIndex > 0){
          migaVal[((migaIndex*2)-1)] = migaHigh;
          }
        } 
      } 
    }
    
    if (sensorValue[8] >= sensorCutOff){//8
      int val = sensorValue[8];
      val = constrain(val, sensorCutOff, (sensorCutOff*seriesFactor));
      val = map(val, sensorCutOff, (sensorCutOff*seriesFactor), 1, 7);
      //Serial.println(i);
     
      for (int i = 0; i < val; i++){
        migaVal[6-i] = migaHigh;
      }
     
//LED SERIES
      val = sensorValue[8];
  //    val = constrain(val, sensorCutOff, (sensorCutOff*seriesFactor));
  //    val = map(val, sensorCutOff, (sensorCutOff*seriesFactor), 0, (16*255));
      val = constrain(val, sensorConMin, sensorConMax);
      val = map(val, sensorConMin, sensorConMax, 0, (16*255));

      for (int i = 0; i < 16;i++) {
        //Serial.println(val);
        int currentLED = 0;
        if ((val > 0) & (val < 255)){
          currentLED = map(val,0,255,0,4095);
        }
        else if (val > 255) {
          currentLED = 4095;
        }
        //Serial.println(currentLED);
        if (currentLED > ledVal[15-i]) {
          ledVal[15-i] = currentLED;
        }
        val -= 255;
      }
    }

    //Back Series Sensor
    
    if (sensorValue[0] >= sensorCutOff){
      int val = sensorValue[0];
      val = constrain(val, sensorCutOff, (sensorCutOff*seriesFactor));//(sensorCutOff*seriesFactor)
      val = map(val, sensorCutOff, (sensorCutOff*seriesFactor), 1, 6);
      //Serial.println(i);
      /* DISABLE MOTORS
      for (int i = 0; i < val; i++){
        migaVal[i] = migaHigh;
      }
      */
//LED SERIES
      val = sensorValue[0];
      val = constrain(val, sensorCutOff, (sensorCutOff*seriesFactor));//(sensorCutOff*seriesFactor)
      val = map(val, sensorCutOff, (sensorCutOff*seriesFactor), 0, (16*255));

      for (int i = 0; i < 16;i++) {
        //Serial.println(val);
        int currentLED = 0;
        if ((val > 0) & (val < 255)){
          currentLED = map(val,0,255,0,4095);
        }
        else if (val > 255) {
          currentLED = 4095;
        }
        //Serial.println(currentLED);
        if (currentLED > ledVal[i]) {
          ledVal[i] = currentLED;
        }
        val -= 255;
      }


    } else {
       ledVal[0] = ledMin;
       ledVal[1] = ledMin; 
      }
  
   
   }

   ///set values
      for (int i = 0; i < 16;i++) {
      int tempLedVal = currentLedVal[i];
      if (abs(currentLedVal[i]-ledVal[i]) > ledStep) {
        if (ledVal[i] > currentLedVal[i]) {
          tempLedVal = currentLedVal[i] + ledStep;
        } 
        else {
          tempLedVal = currentLedVal[i] - ledStep;
        }
      }
      currentLedVal[i] = tempLedVal;

      Tlc.set(ledArray[i],currentLedVal[i]);

    }
    for (int i = 0; i < 8;i++) {
      analogWrite(migaArray[i], migaVal[i]);
    }
    
    if (NOISE) {
      Serial.println("");
      delay(300);
    }
    
  Tlc.update();
  delay(deltaTime);
  //Serial.print("sensorValue = ");
  //Serial.println(sensorValue[8]);
  //Serial.println(max(sensorValue[7],sensorValue[14]));


}


void learn() {
  delay(SAMPLELENGTH);
  while(millis() < SAMPLELENGTH*2){
    for(int i = 0; i < sensorCount;i++){
     int sensorTemp = analogRead(sensorPin[i]);
     //Serial.println(sensorTemp);
     if (sensorTemp < sensorMin[i]) {
       sensorMin[i] = sensorTemp;
     }
     
     if (sensorTemp > sensorMax[i]) {
       sensorMax[i] = sensorTemp;
     }
    }
  }
   if (NOISE) {
     Serial.print("sensorMin = ");
      for(int i = 0; i < sensorCount;i++){
        Serial.print(sensorMin[i]);
        Serial.print("\t");
      }
      
     Serial.println();
     Serial.print("sensorMax = ");
      for(int i = 0; i < sensorCount;i++){
        Serial.print(sensorMax[i]);
        Serial.print("\t");
      }
    }
}

void testSensorConstraints() {
  
  for(int i = 0; i < sensorCount;i++){
    if (sensorValue[i] < sensorMin[i]) {
       sensorMin[i] = sensorValue[i];
      /* if (NOISE) {
           Serial.print(i);
           Serial.print(": sensorMin = ");
           Serial.println( sensorValue[i]);
       }*/
     }
     
     if (sensorValue[i] > sensorMax[i]) {
       sensorMax[i] = sensorValue[i];
      /*  if (NOISE) {
           Serial.print(i);
           Serial.print(": sensorMax = ");
           Serial.println( sensorValue[i]);
       }*/
     }
    }
     if (NOISE) {
        Serial.println("SENSOR MIN");
          for(int i = 0; i < sensorCount;i++){
             Serial.print(sensorMin[i]);
              Serial.print("\t");
          }
        Serial.println("");
        Serial.println("SENSOR MAX");
          for(int i = 0; i < sensorCount;i++){
             Serial.print(sensorMax[i]);
              Serial.print("\t");
          }
          
        Serial.println("");
     }
  }
    

void blinkHello(int num) {
  for (int j = 0; j<num;j++) {
    for(int i = 0; i < 16;i++){
  
      Tlc.set(ledArray[i],4095);
   
    }
    Tlc.update();
    delay(300);
    for(int i = 0; i < 16;i++){
  
      Tlc.set(ledArray[i],0);
   
    }
    Tlc.update();
    delay(300);
  }
}

















