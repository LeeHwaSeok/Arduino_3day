#include<SoftPWM.h>
#include <VitconBrokerComm.h>
using namespace vitcon;

#define LAMP 17
#define PUMP 16 
SOFTPWM_DEFINE_CHANNEL(A3);

//DHT (온도/습도) 
#include "DHT.h"        // DHT 사용 
#define DHTPIN A1       // 온/습도 - A1 pin
#define DHTTYPE DHT22   // DHT22 버전을 사용하겠다. 
DHT dht(DHTPIN, DHTTYPE); // 핀 = A1 , 모델 = DHT22

//SOIL-LINK (토양 습도)
#define SOILHUMI A6     // 토양 습도 - A6 pin

int soil = 0;

float Temp;     //온도 사용 위한 선언
float Humi;     //습도 사용 위한 선언


bool FAN_out_status;
bool PUMP_out_status;
bool LAMP_out_status;


void fan_out(bool val){
  FAN_out_status = val;
}
void PUMP_out(bool val){
  PUMP_out_status = val;
}
void LAMP_out(bool val){
  LAMP_out_status = val;
}


/*widget label*/
IOTItemFlo dht22_temp;
IOTItemFlo dht22_humi;
IOTItemInt soilhumi;

IOTItemBin FanStatus;
IOTItemBin Fan(fan_out);

IOTItemBin PUMPStatus;
IOTItemBin Pump(PUMP_out);

IOTItemBin LAMPStatus;
IOTItemBin Lamp(LAMP_out);


/* A set of definition for IOT items */
#define ITEM_COUNT 6

IOTItem *items[ITEM_COUNT] = { &FanStatus, &Fan, &PUMPStatus,&Pump, &LAMPStatus,&Lamp}; //서버 위젯 인덱스와 연동

/* IOT server communication manager */
const char device_id[]="9dc5b397ddb97621d089094159669c1b"; //Change device_id to yours
BrokerComm comm(&Serial, device_id, items, ITEM_COUNT);

void setup() {
  Serial.begin(250000); //
  comm.SetInterval(200);
  dht.begin();
  SoftPWM.begin(490);
  pinMode(SOILHUMI,INPUT);
  pinMode(PUMP, OUTPUT);
  pinMode(LAMP, OUTPUT);
}          

void loop() {
  Humi = dht.readHumidity();
  Temp = dht.readTemperature();
  soil = map(analogRead(SOILHUMI),0,1023,100,0);  
  
  if(FAN_out_status == true) {
    if (Temp < 29){
       SoftPWM.set(80);
    }  else if (Temp > 20) {
       SoftPWM.set(0);
    }  else {
       SoftPWM.set(65);
    }SoftPWM.set(80); //fan 속도가 빨라서 속도 줄였습니다.
  }
  else SoftPWM.set(0);

  if (PUMP_out_status == true){  
    if (30 < soil && soil < 60) {
      digitalWrite(PUMP,LOW);
    }
    else if(soil < 30) {
      digitalWrite(PUMP,HIGH);
    }
  }
  else digitalWrite(PUMP,LOW);


  digitalWrite(LAMP,LAMP_out_status);  
  
  //MODLINK에서 읽은 값을 서버로 전송
  FanStatus.Set(FAN_out_status);
  LAMPStatus.Set(digitalRead(LAMP));
  PUMPStatus.Set(digitalRead(PUMP));


  comm.Run();
}      
