#include<SoftPWM.h>
#include <VitconBrokerComm.h>
using namespace vitcon;

#define LAMP 17
#define PUMP 16 
SOFTPWM_DEFINE_CHANNEL(A3);

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
//IOTItemFlo dht22_temp;
//IOTItemFlo dht22_humi;
//IOTItemInt soilhumi;
IOTItemBin FanStatus;
IOTItemBin Fan(fan_out);

IOTItemBin PUMPStatus;
IOTItemBin Pump(PUMP_out);

IOTItemBin LAMPStatus;
IOTItemBin Lamp(LAMP_out);


/* A set of definition for IOT items */
#define ITEM_COUNT 6

IOTItem *items[ITEM_COUNT] = { &FanStatus, &Fan, &PUMPStatus,&Pump, &LAMPStatus,&Lamp }; //서버 위젯 인덱스와 연동

/* IOT server communication manager */
const char device_id[]="2695f40fb56fe61428b4e09b2a6219ec"; //Change device_id to yours
BrokerComm comm(&Serial, device_id, items, ITEM_COUNT);

void setup() {
  Serial.begin(250000); //
  comm.SetInterval(200);

  SoftPWM.begin(490);
  pinMode(PUMP, OUTPUT);
  pinMode(LAMP, OUTPUT);
}          

void loop() {
  if(FAN_out_status == true) SoftPWM.set(80); //fan 속도가 빨라서 속도 줄였습니다.
  else SoftPWM.set(0);

  digitalWrite(PUMP,PUMP_out_status);
  digitalWrite(LAMP,LAMP_out_status);  
  
  //MODLINK에서 읽은 값을 서버로 전송
  FanStatus.Set(FAN_out_status);
  LAMPStatus.Set(digitalRead(LAMP));
  PUMPStatus.Set(digitalRead(PUMP));
  comm.Run();
}      
