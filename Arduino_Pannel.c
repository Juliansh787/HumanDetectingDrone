#include <SoftwareSerial.h>
#include <string.h>

//SoftwareSerial BTSerial(2,3);

//led 핀 
#define pin_light_1 7
#define pin_light_2 8
#define pin_light_3 9
#define pin_light_4 10
#define pin_light_5 11
#define pin_light_6 12 
int led_switch_1 = 0 ; //토글 스위치 
int led_switch_2 = 0 ; //토글 스위치 
int led_switch_3 = 0 ; //토글 스위치 
int led_switch_4 = 0 ; //토글 스위치 
int led_switch_5 = 0 ; //토글 스위치 
int led_switch_6 = 0 ; //토글 스위치 

void setup()
{
  pinMode(pin_light_1, OUTPUT);
  pinMode(pin_light_2, OUTPUT);
  pinMode(pin_light_3, OUTPUT);
  pinMode(pin_light_4, OUTPUT);
  pinMode(pin_light_5, OUTPUT);
  pinMode(pin_light_6, OUTPUT);
  digitalWrite(pin_light_1, 0);
  digitalWrite(pin_light_2, 0);
  digitalWrite(pin_light_3, 0);
  digitalWrite(pin_light_4, 0);
  digitalWrite(pin_light_5, 0);
  digitalWrite(pin_light_6, 0);
  //통신설정
  Serial.begin(9600);
  //BTSerial.begin(9600);
}

void loop()
{
  digitalWrite(pin_light_1, led_switch_1);
  digitalWrite(pin_light_2, led_switch_2);
  digitalWrite(pin_light_3, led_switch_3);
  digitalWrite(pin_light_4, led_switch_4);
  digitalWrite(pin_light_5, led_switch_5);
  digitalWrite(pin_light_6, led_switch_6);
 
 // if(BTSerial.available()){
   char data = Serial.read();

      
      switch (data) {
        case '1' : //LED 토글
        if(led_switch_1==0){
          Serial.print("LED_1 ON");
          led_switch_1 =1;
        }
        else if (led_switch_1==1){
          Serial.println("LED_1 OFF");
          led_switch_1 = 0;
        }break;
        
        case '2' : //LED 토글
        if(led_switch_2==0){
          Serial.print("LED_2 ON");
          led_switch_2 =1;
        }
        else if (led_switch_2 ==1){
          Serial.println("LED_2 OFF");
          led_switch_2 = 0;
        }break;

        case '3' : //LED 토글
        if(led_switch_3==0){
          Serial.print("LED_3 ON");
          led_switch_3 =1;
        }
        else if (led_switch_3 ==1){
          Serial.println("LED_3 OFF");
          led_switch_3 = 0;
        }break;

        case '4' : //LED 토글
        if(led_switch_4==0){
          Serial.print("LED_4 ON");
          led_switch_4 =1;
        }
        else if (led_switch_4 ==1){
          Serial.println("LED_4 OFF");
          led_switch_4 = 0;
        }break;
        
        case '5' : //LED 토글
        if(led_switch_5==0){
          Serial.print("LED_5 ON");
          led_switch_5 =1;
        }
        else if (led_switch_5 ==1){
          Serial.println("LED_5 OFF");
          led_switch_5 = 0;
        }break;

        case '6' : //LED 토글
        if(led_switch_6==0){
          Serial.print("LED_6 ON");
          led_switch_6 =1;
        }
        else if (led_switch_6 ==1){
          Serial.println("LED_6 OFF");
          led_switch_6 = 0;
        }
}
}
