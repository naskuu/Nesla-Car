

#include <SoftwareSerial.h>
// DC motor  control
#define P1A 10 // define pin 10as for P1A
#define P2A 5 // define pin 11 as for P2A
#define EN12 12 // define pin 9 as for 1,2EN enable

#define P1B 11 // define pin 10as for P1A
#define P2B 6 // define pin 11 as for P2A
#define EN34 8 // define pin 9 as for 1,2EN enable

#define frontLight 13 // front Lite Pin
#define backLight 3 // back light pin


#define  trig 4  // define the triger for the ultrasonic sensor
#define  echo 7  // define the echo input of the Sensor

long travel_time, distance;



const int max_dis = 40; // 40 sm maximum acceptable distance
const int speedStep = 15;
const int speedDelay = 1000;// delay between speed increment

SoftwareSerial HM10(9, 3); //rx , tx

char appData;
int pitch, roll;

String inData = "";
int incomingByte;
byte naso[2];


void setup() {
  Serial.begin(9600);// setup Serial Monitor to display information

  //Drive wheels
  pinMode(P1A, OUTPUT);// define pin as OUTPUT for P1A
  pinMode(P2A, OUTPUT);// define pin as OUTPUT for P2A
  pinMode(EN12, OUTPUT);// define pin as OUTPUT for 1,2EN
  //Stearing wheels
  pinMode(P1B, OUTPUT);// define pin as OUTPUT for P1A
  pinMode(P2B, OUTPUT);// define pin as OUTPUT for P2A
  pinMode(EN34, OUTPUT);// define pin as OUTPUT for 1,2EN

  //
  // pinMode(trig, OUTPUT);
  // pinMode(echo, INPUT);

  Serial.println("L293D Motor Speed Control");

  Serial.println("HM10 serialstared at 9600");
  HM10.begin(9600);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  lightShow();
}

void loop() {
  Serial.println("LOOP RUN ***** ");

  //lightShow();
  //delay(1000);

  HM10.listen();
  //  forward(0, 0);
  //  turn('S');

  appData = HM10.read();
  inData = String(appData);
  Serial.println(appData);

  switch (appData) {
    case 'G':
      Serial.println("autopilot on");
      gyroDrive();


      break;
    case 'A':
      autopilot();
      break;
    case 'L':
      lightShow();
      break;
    default:
      // if nothing else matches, do the default
      // default is optional
      break;
  }


  delay(300);
  if (HM10.overflow()) {
    Serial.println("SoftwareSerial overflow!");
  }

}



void gyroDrive() {
  Serial.println("GYRO Drive  ");
  delay(400);
  HM10.listen();
  appData = HM10.read();
  inData = String(appData);


  while (HM10.read() != -1) {
    Serial.print("print HM.10 - ");
    Serial.println(HM10.read());
    Serial.println("GYRO DOO   ");
    HM10.listen();
    while (HM10.available() > 0 ) {
      Serial.println("GYRO while RUN * ");



      // pitch = HM10.parseInt();
      // roll = HM10.parseFloat();

      //incomingByte = HM10.read();
      //Serial.println(incomingByte);

      HM10.readBytes(naso, 2);
      Serial.print("Pitch: ");
      Serial.println(naso[0]);
      pitch = naso[0];
      roll = naso[1];
      Serial.print("Roll: ");
      Serial.println(naso[1]);

      if ( pitch < 60 && pitch > 5) {
        forward((195 + pitch), 1);
        //forward(255, 1);
        if (roll > 15 && roll < 90) {
          turn('L');
        }
        else if (roll > 170 && roll < 240) {
          turn('R');
        }
        else {
          turn('S');
        }

      }
      else if (pitch < 250 && pitch > 190) {

        backward((195 + (250 - pitch)), 1);
        //backward(255, 1);
        if (roll > 15 && roll < 90) {
          turn('L');
        }
        else if (roll > 170 && roll < 240) {
          turn('R');
        }
        else {
          turn('S');
        }
      } else {
        forward(0, 0);
        turn('S');
      }
    }

  }


}

void autopilot() {

  do {
    HM10.listen();
    if (ObsticalDetection()) {
      forward(200, 1);
      delay(400);

    } else {
      turn('L');
      backward(200, 1);
      delay(400);

    }
    forward(0, 0);
    turn('S');

  } while (HM10.read() == 'T');
}

bool ObsticalDetection() {

  Serial.println("obstical detection is called");

  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  //Sending a high puls
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  travel_time = pulseIn(echo, HIGH);

  //calculation the distance

  distance = (travel_time * 0.034) / 2 ; // Distance = Time * Speed / 2
  // The speed of sound is 340.29 m/s or 0.034 cm/µs.
  Serial.println(distance);
  if (distance > max_dis)
    return true;
  else
    return false;
}

void lightShow() {
  for (int i = 1; i < 6 ; i++) {
    digitalWrite(frontLight, HIGH);
    digitalWrite(backLight, LOW);
    delay(400);
    digitalWrite(frontLight, LOW);
    digitalWrite(backLight, HIGH);
    delay(400);
  }
  for (int i = 1; i < 10; i++) {
    digitalWrite(frontLight, HIGH);
    digitalWrite(backLight, HIGH);
    delay(1000 / i);
    digitalWrite(frontLight, LOW);
    digitalWrite(backLight, LOW);
    delay(500 / i);
  }
}


void forward (int spd, int en) {
  digitalWrite(frontLight, HIGH);
  digitalWrite(backLight, LOW);
  digitalWrite(EN12 , en); // Enable 1A and 2A
  analogWrite(P1A, spd); // send PWM with spd value to P1A
  digitalWrite(P2A, LOW); // LOW signal to P2A

}

void backward (int spd, int en) {
  digitalWrite(backLight, HIGH);
  digitalWrite(frontLight, LOW);
  digitalWrite(EN12 , en); //enable 1A and 2A
  digitalWrite(P1A, LOW); // Keep it LOW P1A
  analogWrite(P2A, spd); // send PWM with spd value to  P2A

}
void turn(char dir) {

  if (dir == 'L') {

    digitalWrite(EN34 , 1); // Enable 1A and 2A
    analogWrite(P1B, 250); // send PWM with spd value to P1A
    digitalWrite(P2B, LOW); // LOW signal to P2A

  }
  if (dir == 'R') {

    digitalWrite(EN34 , 1); //enable 1A and 2A
    digitalWrite(P1B, LOW); // Keep it LOW P1A
    analogWrite(P2B, 250); // send PWM with spd value to  P2A

  }

  if (dir == 'S') {
    digitalWrite(EN34 , LOW); //enable 1A and 2A
    digitalWrite(P1B, LOW); // Keep it LOW P1A
    analogWrite(P2B, LOW); // send PWM with spd value to  P2A
  }
}
