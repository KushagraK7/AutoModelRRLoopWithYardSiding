/*
 * Arduino code to run a train in a simple oval layout with a yard siding in an automated sequence.
 * 
 * Made by Tech Build: https://www.youtube.com/channel/UCNy7DyfhSD9jsQEgNwETp9g?sub_confirmation=1
 * 
 * Previous version:https://www.youtube.com/watch?v=HqcdEgkYbN8
 *  
 */
#include<Wire.h>
#include<Adafruit_MotorShield.h>//Make sure this library is installed in your IDE.

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor *loco = AFMS.getMotor(1);//Track power connected to the terminal 'M1'.
Adafruit_DCMotor *turnout = AFMS.getMotor(3);//Turnout connected to the terminal 'M4'.

int s, a;
int q;// Variable to store the number of rounds made by the train. It is used at various for() loops in the code.
int r = 2;//Number of rounds the train will take around the loop at the set MaxSpeed.
int m = 2;//Number of rounds the train will take around the loop at the set MidSpeed.

int MinSpeed = 70;
int MidSpeed = 100;
int MaxSpeed = 150;

int firstSensor = A0;//Sensor installed just after the turnout in the mainline with respect to the trains direction of motion.
int secondSensor = A1;//Sensor installed somewhere midway in the mainline.
/*
 * Custom function made to set the speed and direction of the train 
 * depending on the input integer and sign(-255 to 255).
 */
void loco_go(int i){
 if(i>=1&&i<=255){
  loco->setSpeed(i);
  loco->run(FORWARD);
 }
 if(i<=-1&&i>=-255){
  loco->setSpeed(-i);
  loco->run(BACKWARD);
 }
 if(i==0){
  loco->setSpeed(i);
  loco->run(RELEASE);
 }
}

void turnout_side(){
turnout->setSpeed(255);
turnout->run(FORWARD);
delay(100);
turnout->setSpeed(0);
turnout->run(RELEASE);
}

void turnout_straight(){
turnout->setSpeed(255);
turnout->run(BACKWARD);
delay(100);
turnout->setSpeed(0);
turnout->run(RELEASE);
}
/*
 * Custom function made to wait for the train to pass the 'sensored' track connected to the 
 * input pin declared in the bracket(in the place of int j in void loop()).
 * 
 * The sensor's output does not remains to be steady HIGH when a train is passing over it.
 * This can be observed by the flickering light of the sensor when the train crosses over it.
 * 
 * This means that the earlier used while() statement can't be used here.
 * 
 * To slove, it a for() loop is made which waits for the sensor output to remain low for 
 * a long enough time to ensure that the train has crossed the 'sensored' track.
 * 
 * When the train crosses the 'sensored' track, the ocasional flicker of the LED(Meaning that
 * sensor is sensding digital HIGH signals to the Aruino board.) causes the loop to reset.
 * This loop ends only sfter the complete train has crossed the 'sensored' track.
 */

void WaitToPass(int j){
  b:
  for(a=0;a!=500;a++){
    if(digitalRead(j)==HIGH) goto b;//If the sensor output turns HIGH in between the for() loop, it will start over.
    delay(10);
  }
}

void setup() {
  // put your setup code here, to run once:

  pinMode(firstSensor, INPUT);
  pinMode(secondSensor, INPUT);
  AFMS.begin();

  turnout_side();

}

void loop() {
  // put your main code here, to run repeatedly:


  for(s=0;s!=30;s++){//The locomotive does nothing till about 30, so increasing it quickly.
    loco_go(s);
    delay(60);
  }

  for(s=s;s!=MinSpeed;s++){//Increasing the speed to MinSpeed for the train to start moving slowly.
    loco_go(s);
    delay(500);
  }

  while(digitalRead(firstSensor)!=HIGH);//Waiting for the train to reach the 'sensored' track to reach the turnout inatalled just after the turnout on the mainline.

  for(s=s;s!=80;s++){//Speeding up the train a bit.
    loco_go(s);
    delay(250);
  }

  WaitToPass(firstSensor);//Waiting for the train to completely cross the first ' sensored' track.

  turnout_straight();//Switching the turnout to straight.

  while(digitalRead(secondSensor)!=HIGH);//Waiting for the train to reach the second 'sensored' track.

  for(s=s;s!=MidSpeed;s++){//Increasing the speed of the train to MidSpeed.
    loco_go(s);
    delay(500);
  }

  WaitToPass(secondSensor);//Waiting for the train to pass over the second 'sensored' track.

  while(digitalRead(firstSensor)!=HIGH);//Waiting for the train to reach the first 'sensored' track, completing one round.

  for(s=s;s!=MaxSpeed;s++){//Increasing the speed of the train to the maximum set value.
    loco_go(s);
    delay(250);
  }

  WaitToPass(firstSensor);//Waiting for the train to pass the first 'sensored' track.

 /*
  * for() loop for the train to make number of rounds in the loop at set MaxSpeed, dtermined
  * by the variable 'r'.
  */
  for(q=1;q!=r;q++){//
  while(digitalRead(A0)!=HIGH);

  WaitToPass(firstSensor);
  }

  while(digitalRead(firstSensor)!=HIGH);//Waiting for the train to reach the first 'sensored' track.

  for(s=s;s!=MidSpeed;s--){//Reducing the train's speed to set MidSpeed.
    loco_go(s);
    delay(250);
  }

  WaitToPass(firstSensor);//Waiting for the train to pass over the first 'sensored' track.

  
  for(q=0;q!=m;q++){//for() loop to run the trains for 'm' number of rounds st MidSpeed.
  while(digitalRead(secondSensor)!=HIGH);

  WaitToPass(secondSensor);
  }

  while(digitalRead(secondSensor)!=HIGH);//Waiting for the train to reach the second sensor.

  for(s=s;s!=MinSpeed;s--){//Reducing the speed of the train to the set MinSpeed.
    loco_go(s);
    delay(250);
  }

  while(digitalRead(firstSensor)!=HIGH);

  WaitToPass(firstSensor);

  for(s=s;s!=30;s--){//Slowing down the train.
    loco_go(s);
    delay(250);
  }

  for(s=s;s!=0;s--){//Bringing the train to a halt.
    loco_go(s);
    delay(60);
  }

  turnout_side();//Switching the turnout to side to connect the yard siding to the mainline.

  delay(2000);//Waiting for a few seconds, here 2.

  for(s=s;s!=-30;s--){//Starting the train backwards.
    loco_go(s);
    delay(60);
  }

  for(s=s;s!=-MinSpeed;s--){//Speeding it up till set MinSpeed in backward direction.
    loco_go(s);
    delay(250);
  }

  while(digitalRead(firstSensor)!=HIGH);

  WaitToPass(firstSensor);

  for(s=s;s!=-30;s++){//Slowing down the train.
    loco_go(s);
    delay(250);
  }

  for(s=s;s!=0;s++){//Bringing the train to a halt.
    loco_go(s);
    delay(60);
  }

  delay(5000);//Witing for a few seconds, here 5, before starting the whole operation again.
  
  
  
}
