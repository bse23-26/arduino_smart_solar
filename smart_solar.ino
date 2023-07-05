#include <SoftwareSerial.h>
#include <Servo.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

//global objects and pins attached
Servo servo;
SoftwareSerial bluetooth(10, 11); // RX, TX
// const int LED_BLUETOOTH = 8;
const short servoPin = 8;
const int ldrPin1 = A0;
const int ldrPin2 = A1;

// const int currentPin = A2;
// const int voltagePin = A3;

const int line1 = 2;
const int line2 = 3;

int ldrPin1Status = 0;
int ldrPin2Status = 0;
// int voltage = 0;

const int minAngle = 0;
const int maxAngle = 180;
int currAngle = 90;

const int baud = 9600;

// unsigned long readingsTime = millis();

struct Configuration {
  char deviceId[37];
  short powerlines;
  short powerline1;
  short powerline2;
  float maxVoltage;
  float minVoltage;
  float nominalVoltage;
  float rating;
};

Configuration configurations;

bool populated;
// bool readingsNeeded = true;

void setup() {
  //pin setup
  // pinMode(LED_BLUETOOTH, OUTPUT);
  // pinMode(LED_BUILTIN, OUTPUT);
  pinMode(line1, OUTPUT);
  pinMode(line2, OUTPUT);
  // pinMode(servoPin, OUTPUT);
  pinMode(ldrPin1, INPUT);
  pinMode(ldrPin2, INPUT);
  // pinMode(currentPin, INPUT);
  // pinMode(voltagePin, INPUT);
  servo.attach(servoPin);
  
  Serial.begin(baud);
  while (!Serial) {
    ;
  }

  //initialize
  bluetooth.begin(baud);
  digitalWrite(LED_BUILTIN, LOW);
  servo.write(currAngle);
  populated = false;
  readConfigurations();
  switchPowerlines();
  delay(15);
}

void loop() {
  rotateServo();
  interfaceBluetooth();
  // if(readingsNeeded){
  // determineElectricReadings();
  // }
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
    // readingsNeeded = false;
    String dataReceived = bluetooth.readString();
    // delay(100);
    Serial.println(dataReceived);
    dataReceived.trim();
    if(dataReceived.length()>1){
      takeAction(dataReceived);
    }
  }
}

void sendBluetooth(String dataPrepared){
  bluetooth.println(dataPrepared);
  // delay(100);
}

void takeAction(String received){
  String comm = received.substring(0,3);
  for(byte i=0;i<3;i++){
    if(!isDigit(comm.charAt(i))) return;
  }
  int command = stringToInt(comm);

  //100 write configurations
  //101 change state of the powerlines

  //200 read configurations

  if(command==100){
      String sent = received.substring(3);
      writeConfigurations(sent);
      sendConfigurations();
      return;
  }

  if(command==200){
    sendConfigurations();
  }
}

void sendConfigurations(){
  String conf = readConfigurations();
  makeCurrentData("configurations", conf);
}

// add backslash if data is string
void makeCurrentData(String type, String data){
  String dataCurrent = "{\"type\": \""+type+"\", \"data\": "+data+"}";
  sendBluetooth(dataCurrent);
}

void rotateServo(){
  ldrPin1Status = analogRead(ldrPin1);
  ldrPin2Status = analogRead(ldrPin2);

  int max = ldrPin2Status;
  int maxPin = 2;

  if(ldrPin1Status>max){
    max = ldrPin1Status;
    maxPin = 1;
  }

  if(abs(ldrPin1Status-ldrPin2Status)<=30){
    return;
  }

  if(maxPin==1 && currAngle>minAngle){
    currAngle--;
  }else if(currAngle<maxAngle){
    currAngle++;
  }

  servo.write(currAngle);
  delay(15);
}

///////////////////////////////////////////////////////////////
// void determineElectricReadings(){
//   unsigned long newReadingsTime = millis();
//   if((newReadingsTime-readingsTime)<2000){
//     return;
//   }
//   readingsTime = newReadingsTime;
//   Serial.println("readings...");
//   voltage = analogRead(voltagePin);
//   Serial.println("battery voltage is: "+String((voltage/1023.0)*25)+"V");

//   voltage = analogRead(currentPin);
//   Serial.println("battery current is: "+String((voltage/1023.0)*5)+"A");

////////////////////////////////////////
  // bluetooth.println("{\"type\": \"current\", \"data\": "+String((voltage/1023.0)*5)+"}");
  // delay(100);

//   float maxAnalogValue = 1015.0;
// float maxVoltage = 25.0;
  // float volt = (voltage*25.0)/1023.0;
  // float diff1 = configurations.maxVoltage-configurations.minVoltage;
  // float diff2 = volt-configurations.minVoltage;
  
  // Serial.println("percentage is:"+String((diff2/diff1)*100));
  // Serial.println(String((voltage/maxAnalogValue)*maxVoltage));
  // makeCurrentData("voltage", String(voltage));
// }

String readConfigurations(){
  // String confStr;
  // EEPROM.get(0, confStr);
  // int strFirstIndex;
  // String charNum = confStr.substring(0, strFirstIndex);
  // int strLen = charNum.toInt();
  // confStr = confStr.substring(strFirstIndex, strFirstIndex+strLen);

  String output = "";
  if(!populated){
    EEPROM.get(0, configurations);
    populated = true;
    switchPowerlines();
  }
  else{
    // doc["deviceId"] = configurations.deviceId;
    // doc["powerlines"] = configurations.powerlines;
    // doc["powerline1"] = configurations.powerline1;
    // doc["powerline2"] = configurations.powerline2;
    // doc["maxVoltage"] = configurations.maxVoltage;
    // doc["minVoltage"] = configurations.minVoltage;
    // doc["nominalVoltage"] = configurations.nominalVoltage;
    // doc["rating"] = configurations.rating;

    // \"deviceId\":\""+configurations.deviceId+"\",
    output = "{\"powerlines\":"+String(configurations.powerlines)+",\"powerline1\":"+String(configurations.powerline1)+",\"powerline2\":"+String(configurations.powerline2)+",\"maxVoltage\":"+String(configurations.maxVoltage)+",\"minVoltage\":"+String(configurations.minVoltage)+",\"nominalVoltage\":"+String(configurations.nominalVoltage)+",\"rating\":"+String(configurations.rating)+"}";

  }
  return output;
}

void writeConfigurations(String confStr){
  int str_len = confStr.length() + 1; 
  char char_array[str_len];
  confStr.toCharArray(char_array, str_len);

  StaticJsonDocument<210> doc;

  DeserializationError error = deserializeJson(doc, char_array, str_len);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  configurations.powerline1 = doc["powerline1"];
  configurations.powerline2 = doc["powerline2"];
  configurations.maxVoltage = doc["maxVoltage"];
  configurations.minVoltage = doc["minVoltage"];
  configurations.nominalVoltage = doc["nominalVoltage"];
  configurations.rating = doc["rating"];
  switchPowerlines();

  EEPROM.put(0, configurations);
}

void switchPowerlines(){
  digitalWrite(line1, configurations.powerline1);
  digitalWrite(line2, configurations.powerline2);
}

// setups
// void setup() {
//   String id = "0921a3e3-7a1f-4a0f-9ce9-8bd98bad7bc2";
//   id.toCharArray(configurations.deviceId, 37);
//   configurations.powerline1 = 0;
//   configurations.powerline2 = 0;
//   configurations.maxVoltage = 0;
//   configurations.minVoltage = 0;
//   configurations.nominalVoltage = 0;
//   configurations.rating = 0;
//   configurations.powerlines = 2;
//   EEPROM.put(0, configurations);
// }

// void setup() {
// }

// void loop() {
// }
