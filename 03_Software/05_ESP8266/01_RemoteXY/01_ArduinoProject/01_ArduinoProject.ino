/*
   -- New project --
   
   This source code of graphical user interface 
   has been generated automatically by RemoteXY editor.
   To compile this code using RemoteXY library 2.4.3 or later version 
   download by link http://remotexy.com/en/library/
   To connect using RemoteXY mobile app by link http://remotexy.com/en/download/                   
     - for ANDROID 4.5.1 or later version;
     - for iOS 1.4.1 or later version;
    
   This source code is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.    
*/

//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// RemoteXY select connection mode and include library 
#define REMOTEXY_MODE__ESP8266WIFI_LIB_POINT
#include <ESP8266WiFi.h>

#include <RemoteXY.h>

// RemoteXY connection settings 
#define REMOTEXY_WIFI_SSID "RemoteXY"
#define REMOTEXY_WIFI_PASSWORD "12345678"
#define REMOTEXY_SERVER_PORT 6377


// RemoteXY configurate  
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =
  { 255,48,0,0,0,66,0,10,66,1,
  1,1,26,63,12,12,36,8,83,101,
  116,0,7,4,2,26,59,7,2,27,
  38,36,129,0,20,34,24,6,8,78,
  101,116,119,111,114,107,0,129,0,17,
  51,28,6,8,80,97,115,115,119,111,
  114,100,0,7,4,3,42,57,7,2,
  27,2,11 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  uint8_t button_1; // =1 if button pressed, else =0 
  char edit_1[36];  // string UTF8 end zero  
  char edit_2[11];  // string UTF8 end zero  

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;
#pragma pack(pop)

/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////

#define PIN_BUTTON_1 D0


void setup() 
{
  Serial.begin(115200);
  RemoteXY_Init (); 
  
  pinMode (PIN_BUTTON_1, OUTPUT);
  
  // TODO you setup code
  
}

void loop() 
{ 
  RemoteXY_Handler ();
  if(RemoteXY.button_1==1){
    Serial.printf("SSID=%s",strcat(RemoteXY.edit_1,"\n"));
    Serial.printf("PW=%s",strcat(RemoteXY.edit_2,"\n"));
    
    RemoteXY.button_1=0;
  }
  
  // TODO you loop code
  // use the RemoteXY structure for data transfer
  // do not call delay() 


}
