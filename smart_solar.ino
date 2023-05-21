#include <SoftwareSerial.h>
#include <Servo.h>

Servo servo;
int pos = 0;
SoftwareSerial bluetooth(10, 11); // RX, TX
const int LED_BLUETOOTH = 8;
const int servoPin = 9;
const int ldrPin1 = A0;
const int ldrPin2 = A1;
const int ldrPin3 = A2;

int ldrPin1Status = 0;
int ldrPin2Status = 0;
int ldrPin3Status = 0;

const int minAngle = 0;
const int maxAngle = 180;
int currAngle = 90;

const int baud = 9600;
String dataCurrent = "";
String dataPrepared = "";

void setup() {
  //pin setup
  pinMode(LED_BLUETOOTH, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ldrPin1, INPUT);
  pinMode(ldrPin2, INPUT);
  pinMode(ldrPin3, INPUT);
  servo.attach(servoPin);
  
  Serial.begin(baud);
  while (!Serial) {
    ;
  }

  Serial.println("Goodnight moon!");

  //initialize
  bluetooth.begin(baud);
  // bluetooth.println("Hello, world?");
  digitalWrite(LED_BUILTIN, LOW);
  servo.write(currAngle);
  delay(15);
}

void loop() {
  rotateServo();
  // rotate();
  // interfaceBluetooth();
  // rotate();
  // servo.write(90);
  // delay(15);
}

long stringToInt(String str){
  int strlen = str.length();
  
  String digitString = "";
  for(int i=0; i < strlen; i++ ) {
    char c = str[i];
    if(isDigit(c)){
      digitString += c;
    }else{
      return -1;
    }
  }
  return (strlen==0)?-1:digitString.toInt();
}

//add to dataCurrent for sending

void interfaceBluetooth(){
  if (bluetooth.available()) {
    String dataReceived = bluetooth.readString();
    dataReceived.trim();
    if(dataReceived.length()>1){
      takeAction(dataReceived);
    }
  }

  if(dataPrepared.length()==0 && dataCurrent.length()>0){
    dataPrepared = "" + dataCurrent;
    dataCurrent = "";
    bluetooth.println(dataPrepared);
  }
}

void takeAction(String received){
  int command = stringToInt(received.substring(0,2));

  switch(command){
    case 0:
      dataPrepared = "";
      break;
    case 1:
      break;
    default:
      break;
  }
}

void rotateServo(){
  ldrPin1Status = analogRead(ldrPin1);
  ldrPin2Status = analogRead(ldrPin2);
  ldrPin3Status = analogRead(ldrPin3);

  int max = ldrPin2Status;
  int maxPin = 2;

  if(ldrPin1Status>max){
    max = ldrPin1Status;
    maxPin = 1;
  }

  if(ldrPin3Status>max){
    max = ldrPin3Status;
    maxPin = 3;
  }

  if(maxPin==2){
    return;
  }
  if(maxPin==1){
    currAngle--;
  }else{
    currAngle++;
  }

  servo.write(currAngle);
  delay(150);
}

