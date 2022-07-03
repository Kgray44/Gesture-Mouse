#include <MPU6050_light.h>
#include <Wire.h>
#include <Mouse.h>
#include <Keyboard.h>

float Ythreshhold = 5.00; //degrees
float Xthreshhold = 5.00; //degrees
float Xthreshholdtop = 30.00; //degrees
float Ythreshholdtop = 40.00; //degrees
float clickThreshhold = 1.65; //accel value
float scrollthreshhold = 30.00; //degrees; same as Xangle; i.e. Xthreshholdtop
float switchdesktopthreshhold = 350.00; //gyro value

int clickcounter;
boolean isClicked = false;

float gyroZ;
float accelZ;
float Xangle, Yangle, Zangle;
float lastXangle, lastYangle, lastZangle;

int timer;

MPU6050 mpu(Wire);

void setup(){
  Mouse.begin();
  Keyboard.begin();

  Serial.begin(115200);
  delay(1000);

  Wire.begin();
  
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  mpu.calcOffsets(true,true); // gyro and accelero
}

void loop(){
  mpu.update();

  Xangle = mpu.getAngleX();
  Yangle = mpu.getAngleY();
  Zangle = mpu.getAngleZ();
  accelZ = mpu.getAccZ();
  gyroZ = mpu.getGyroZ();
  
  Serial.println("Xangle: " + String(Xangle) + " Yangle: " + String(Yangle) + " Zangle: " + String(Zangle) + " AccelZ: " + String(accelZ) + " gyroZ: " + String(gyroZ));

  if ((abs(Xangle) > Xthreshhold && abs(Xangle) < Xthreshholdtop) || (abs(Yangle) > Ythreshhold && abs(Yangle) < Ythreshholdtop)){
    //if (abs(Yangle) > Ythreshhold){
    Mouse.move(map(Yangle,5,15,0,2),map(Xangle,5,12,0,2));
  }

  if (abs(Xangle) > scrollthreshhold){
    if (Xangle < -scrollthreshhold){
      Mouse.move(0,0,-1);
      delay(map(Xangle,-scrollthreshhold,-45,85,45));//65
    }
    else {
      Mouse.move(0,0,1);
      delay(map(Xangle,scrollthreshhold,45,85,45));//65
    }
  }
  
  if (isClicked){
    clickcounter++;
    if (clickcounter == 20){
      isClicked = false;
      clickcounter = 0;
    }
  }
  
  if ((abs(accelZ) > clickThreshhold)){// && clickcounter == 0){
    if (accelZ < -clickThreshhold){
      Mouse.click(MOUSE_RIGHT);
      while (accelZ < -clickThreshhold){ 
        mpu.update();
        accelZ = mpu.getAccZ();
        Serial.println("AccelZ: " + String(accelZ));
      }
      delay(100);
    }
    else if (accelZ > clickThreshhold){
      Mouse.click(MOUSE_LEFT);
      while (accelZ > clickThreshhold){
        mpu.update();
        accelZ = mpu.getAccZ();
        Serial.println("AccelZ: " + String(accelZ));
      }
      delay(100);
    }
  }

  if (abs(gyroZ) > switchdesktopthreshhold && clickcounter == 0){//Yangle
    if (gyroZ < -switchdesktopthreshhold){
      Keyboard.press(128);
      Keyboard.press(215);
      Keyboard.releaseAll();
    }
    else {
      Keyboard.press(128);
      Keyboard.press(216);
      Keyboard.releaseAll();
    }
    isClicked = true;
    //delay(1000);
  }

  if (abs(Zangle) > switchdesktopthreshhold){
    timer++;
    if (timer == 50){
      mpu.calcGyroOffsets(); 
      delay(1000);
      Serial.println("Reset gyro");
    }
  }
  else {
    timer=0;
  }
  
  lastXangle = Xangle;
  lastYangle = Yangle;
  lastZangle = Zangle;


  delay(1);
}
