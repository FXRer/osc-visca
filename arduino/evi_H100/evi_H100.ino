#include <Ethernet.h>
#include <SPI.h>
#include <ArdOSC.h>
#include <string.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(6, 7); // RX, TX

/************* Variables ****************/
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 
  192, 168, 0, 17 };
int serverPort = 10000;
int incomingByte = 0;   // for incoming serial data
OSCMessage global_mes;
int destPort = 12000;
OSCServer server;
OSCClient client;
byte sourceIp[]  = { 
  192, 168, 0, 4 };
uint8_t ViscaMsg[6] = {  
  0x81, 0x01, 0x04, 0x00, 0x00, 0xFF      };
uint8_t ViscaLongMsg[9] = {  
  0x81, 0x01, 0x04, 0x47, 0x00, 0x00, 0x00, 0x00, 0xFF            };
uint8_t ViscaMemMsg[7] = {  
  0x81, 0x01, 0x04, 0x47, 0x00, 0x00, 0xFF            };
int TiltSpeed = 3;
int PanSpeed = 3;
void setup()
{
  /************* Launch ethernet server  ****************/
  Ethernet.begin(mac, ip);
  server.begin(serverPort);  
  /************* Launch Serial Communication for visca commands ****************/
  mySerial.begin(9600);
  /************* Add some osc messages ****************/
  server.addCallback("/visca/AddressSet",&ViscaAddressSet);
  server.addCallback("/visca/ifclear",&ViscaIfClear);
  server.addCallback("/visca/cancel",&ViscaCancel);
  server.addCallback("/visca.1/power",&ViscaSw);
  server.addCallback("/visca.1/zoom/stop",&ViscaZoomStop);
  server.addCallback("/visca.1/zoom/standart",&ViscaZoomStandard);
  server.addCallback("/visca.1/zoom/variable/wide",&ViscaZoomVariableWide);
  server.addCallback("/visca.1/zoom/variable/tele",&ViscaZoomVariableTele);
  server.addCallback("/visca.1/zoom",&ViscaZoom);
  server.addCallback("/visca.1/focus/stop",&ViscaFocusStop);
  server.addCallback("/visca.1/focus/standart",&ViscaFocusStandard);
  server.addCallback("/visca.1/focus/variable/far",&ViscaFocusVariableFar);
  server.addCallback("/visca.1/focus/variable/near",&ViscaFocusVariableNear);
  server.addCallback("/visca.1/focus",&ViscaFocus);
  server.addCallback("/visca.1/focus/mode",&ViscaFocusMode);
  server.addCallback("/visca.1/focus/onepushtrigger",&ViscaFocusTrigger);
  server.addCallback("/visca.1/focus/infinity",&ViscaFocusInfinity);
  server.addCallback("/visca.1/focus/nearlimit",&ViscaFocusNearLimit);
  server.addCallback("/visca.1/focus/sensitivity",&ViscaFocusAFSens);
  server.addCallback("/visca.1/focus/auto/mode",&ViscaFocusAFMode);
  server.addCallback("/visca.1/init/internal",&ViscaInit);
  server.addCallback("/visca.1/WB",&ViscaWB);
  server.addCallback("/visca.1/AE",&ViscaExposure);
  server.addCallback("/visca.1/slowshutter",&ViscaSlowShutter);
  server.addCallback("/visca.1/shutter/value",&ViscaShutter);
  server.addCallback("/visca.1/iris/value",&ViscaIris);
  server.addCallback("/visca.1/gain/value",&ViscaGain);
  server.addCallback("/visca.1/IR",&ViscaIR);
  server.addCallback("/visca.1/IR/correction",&ViscaIRCorrection);
  server.addCallback("/visca.1/gamma",&ViscaGamma);
  server.addCallback("/visca.1/WD",&ViscaWD);
  server.addCallback("/visca.1/chromasupress",&ViscaChromaSuppress);
  server.addCallback("/visca.1/speed",&ViscaHome);  
  server.addCallback("/visca.1/home",&ViscaHome);  
  server.addCallback("/visca.1/reset",&ViscaReset);  
  server.addCallback("/visca.1/pan/speed",&ViscaPanSpeed);  
  server.addCallback("/visca.1/tilt/speed",&ViscaTiltSpeed);  
  server.addCallback("/visca.1/left",&ViscaLeft);  
  server.addCallback("/visca.1/upleft",&ViscaUpLeft);  
  server.addCallback("/visca.1/up",&ViscaUp);  
  server.addCallback("/visca.1/upright",&ViscaUpRight);  
  server.addCallback("/visca.1/right",&ViscaRight);  
  server.addCallback("/visca.1/downright",&ViscaDownRight);  
  server.addCallback("/visca.1/down",&ViscaDown);  
  server.addCallback("/visca.1/downleft",&ViscaDownLeft);  
  server.addCallback("/visca.1/stop",&ViscaStop);  
}
void loop()
{ 
  /************* // Check OSC messages ****************/
  int result = server.aviableCheck();
  if(result>0) {
  }
  /************* // Check Serial messages for loopback ****************/
  if (mySerial.available() > 0) {
    // read the incoming byte:
    incomingByte = mySerial.read();
  /************* // Send Serial messages to OSC ****************/
    global_mes.setAddress(sourceIp,destPort);
    global_mes.beginMessage("/visca/from");
    global_mes.addArgInt32(incomingByte);
    client.send(&global_mes);
    global_mes.flush(); //object data clear
  }
}
/************* ViscaAddressSet ****************/
void ViscaAddressSet(OSCMessage *_mes) {
  mySerial.write(0x88);
  mySerial.write(0x30);
  mySerial.write(0x01);
  mySerial.write(0xFF); 
}
/************* IfClear ****************/
void ViscaIfClear(OSCMessage *_mes) {
  mySerial.write(0x88);
  mySerial.write(0x01);
  mySerial.write((uint8_t) 0);
  mySerial.write(0x01);
  mySerial.write(0xFF); 
}
/************* Cancel ****************/
void ViscaCancel(OSCMessage *_mes) {
  /* Buffer 1 */
  mySerial.write(0x81);
  mySerial.write(0x21);
  mySerial.write(0xFF); 
  /* Buffer 2 */
  delay(200);
  mySerial.write(0x81);
  mySerial.write(0x22);
  mySerial.write(0xFF); 
}
/************* Power ****************/
void ViscaSw(OSCMessage *_mes) {
      ViscaMsg[3] =  0x00;
  int strSize=_mes->getArgStringSize(0);
  char value[strSize]; //string memory allocation
  _mes->getArgString(0,value);
  if ( memcmp(value,"on",2) == 0) {
    ViscaMsg[4] =  0x02;
  } 
  else {
    ViscaMsg[4] =  0x03;
  } 
  mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
}
/************* Zoom Stop ****************/
void ViscaZoomStop(OSCMessage *_mes) {
  ViscaMsg[3] =  0x07;
  ViscaMsg[4] = ((uint8_t) 0);
  mySerial.write( ViscaMsg, sizeof(ViscaMsg) );

}
/************* Zoom Standard ****************/
void ViscaZoomStandard(OSCMessage *_mes) {
  ViscaMsg[3] =  0x07;
  int strSize=_mes->getArgStringSize(0);
  char value[strSize]; //string memory allocation
  _mes->getArgString(0,value);  
  if ( memcmp(value,"tele",4) == 0) {
    ViscaMsg[4] =  0x02;
  }
  if ( memcmp(value,"wide",4) == 0) {
    ViscaMsg[4] =  0x03;
  }
  mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
}
void ViscaZoomVariableTele(OSCMessage *_mes) {
  ViscaMsg[3] =  0x07;
  int value = _mes->getArgInt32(0);
  if (value >= 0 && value <= 7) {
    ViscaMsg[4] =  0x20 | value;
  mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
  }
}
void ViscaZoomVariableWide(OSCMessage *_mes) {
  ViscaMsg[3] =  0x07;
  int value = _mes->getArgInt32(0);
  if (value >= 0 && value <= 7) {
    ViscaMsg[4] =  0x30 | value;
  mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
  }
}
/************* Zoom Direct ****************/
void ViscaZoom(OSCMessage *_mes) {
  int value = _mes->getArgInt32(0);
  int valuea = value & 15; 
  int valuebZ = value >> 4; 
  int valuecZ = value >> 8; 
  int valuedZ = value >> 12; 
  int valueb = valuebZ & 15  ; 
  int valuec = valuecZ & 15 ; 
  int valued = valuedZ & 15 ; 
  ViscaLongMsg[2] =  0x04;
  ViscaLongMsg[3] =  0x47;
  ViscaLongMsg[4] =  valued;
  ViscaLongMsg[5] =  valuec;
  ViscaLongMsg[6] =  valueb;
  ViscaLongMsg[7] =  valuea;
  mySerial.write( ViscaLongMsg, sizeof(ViscaLongMsg) );
}
/************* Focus Stop ****************/
void ViscaFocusStop(OSCMessage *_mes) {
  ViscaMsg[3] =  0x08;
  ViscaMsg[4] =  0x00;
  mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
}
/************* Focus Standard ****************/
void ViscaFocusStandard(OSCMessage *_mes) {
  ViscaMsg[3] =  0x08;
  int strSize=_mes->getArgStringSize(0);
  char value[strSize]; //string memory allocation
  _mes->getArgString(0,value);
  if ( memcmp(value,"far",3) == 0) {
    ViscaMsg[4] =  0x02;

  }
  if ( memcmp(value,"near",4) == 0) {
    ViscaMsg[4] =  0x03;
  }
  mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
}
/************* Focus Variable Near ****************/
void ViscaFocusVariableNear(OSCMessage *_mes) {
  ViscaMsg[3] =  0x08;
  int value = _mes->getArgInt32(0);
  if (value >= 0 && value <= 7) {
    ViscaMsg[4] =  0x20 | value;
  mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
  }
}
/************* Focus Variable Far ****************/
void ViscaFocusVariableFar(OSCMessage *_mes) {
  ViscaMsg[3] =  0x08;
  int value = _mes->getArgInt32(0);
  if (value >= 0 && value <= 7) {
    ViscaMsg[4] =  0x30 | value;
  mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
  }
}
/************* Focus Direct ****************/
void ViscaFocus(OSCMessage *_mes) {
  int value = _mes->getArgInt32(0);
  int valuea = value & 15; 
  int valuebZ = value >> 4; 
  int valuecZ = value >> 8; 
  int valuedZ = value >> 12; 
  int valueb = valuebZ & 15  ; 
  int valuec = valuecZ & 15 ; 
  int valued = valuedZ & 15 ; 
  ViscaLongMsg[2] =  0x04;
  ViscaLongMsg[3] =  0x48;
  ViscaLongMsg[4] =  valued;
  ViscaLongMsg[5] =  valuec;
  ViscaLongMsg[6] =  valueb;
  ViscaLongMsg[7] =  valuea;
  mySerial.write( ViscaLongMsg, sizeof(ViscaLongMsg) );
}
/************* Focus Auto ****************/
void ViscaFocusMode(OSCMessage *_mes) {
  ViscaMsg[3] =  0x38;
  int strSize=_mes->getArgStringSize(0);
  char value[strSize]; //string memory allocation
  _mes->getArgString(0,value);
  if ( memcmp(value,"auto",4) == 0) {
   ViscaMsg[4] =  0x02;
  } 
  if ( memcmp(value,"manual",6) == 0) {
   ViscaMsg[4] =  0x03;
  } 
  mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
}
/************* Focus Trigger ****************/
void ViscaFocusTrigger(OSCMessage *_mes) {
  ViscaMsg[3] =  0x18;
  ViscaMsg[4] =  0x01;
  mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
}
/************* Focus Infinity ****************/
void ViscaFocusInfinity(OSCMessage *_mes) {
  ViscaMsg[3] =  0x18;
  ViscaMsg[4] =  0x02;
  mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
}
/************* Focus Near Limit ****************/
void ViscaFocusNearLimit(OSCMessage *_mes) {
  int value = _mes->getArgInt32(0);
  int valuea = value & 15; 
  int valuebZ = value >> 4; 
  int valuecZ = value >> 8; 
  int valuedZ = value >> 12; 
  int valueb = valuebZ & 15  ; 
  int valuec = valuecZ & 15 ; 
  int valued = valuedZ & 15 ; 
  ViscaLongMsg[2] =  0x04;
  ViscaLongMsg[3] =  0x28;
  ViscaLongMsg[4] =  valued;
  ViscaLongMsg[5] =  valuec;
  ViscaLongMsg[6] =  valueb;
  ViscaLongMsg[7] =  valuea;
  mySerial.write( ViscaLongMsg, sizeof(ViscaLongMsg) );
}
/************* Focus Sensitivity ****************/
void ViscaFocusAFSens(OSCMessage *_mes) {
  ViscaMsg[3] =  0x58;
  int strSize=_mes->getArgStringSize(0);
  char value[strSize]; //string memory allocation
  _mes->getArgString(0,value);
  if ( memcmp(value,"normal",6) == 0) {
  ViscaMsg[4] =  0x02;

  } 
  if ( memcmp(value,"low",3) == 0) {
  ViscaMsg[4] =  0x03;
  } 
  mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
}
/************* Focus Auto Mode ****************/
void ViscaFocusAFMode(OSCMessage *_mes) {
  ViscaMsg[3] =  0x57;
  int strSize=_mes->getArgStringSize(0);
  char value[strSize]; //string memory allocation
  _mes->getArgString(0,value);
  if ( memcmp(value,"normal",6) == 0) {
  ViscaMsg[4] =  0x00;
  } 
  if ( memcmp(value,"interval",8) == 0) {
  ViscaMsg[4] =  0x01;
  } 
  mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
}
/************* Focus Zoom Trigger ****************/
void ViscaFocusAFZoomTrigger(OSCMessage *_mes) {
  ViscaMsg[3] =  0x57;
  ViscaMsg[4] =  0x02;
  mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
}
/************* Zoom Focus ****************/
void ViscaZoomFocus(OSCMessage *_mes) {
  int value = _mes->getArgInt32(0);
  int value2 = _mes->getArgInt32(1);
  int ZFZooma = value & 15 ; 
  int ZFZoomb = value >> 4 & 15  ; 
  int ZFZoomc = value >> 8 & 15 ; 
  int ZFZoomd = value >> 12 & 15 ; 
  int ZFFocusa = value & 15 ; 
  int ZFFocusb = value >> 4 & 15  ; 
  int ZFFocusc = value >> 8 & 15 ; 
  int ZFFocusd = value >> 12 & 15 ; 
  mySerial.write(0x81);
  mySerial.write(0x01);
  mySerial.write(0x04);
  mySerial.write(0x47);
  mySerial.write((byte)ZFZoomd);
  mySerial.write((byte)ZFZoomc);
  mySerial.write((byte)ZFZoomb);
  mySerial.write((byte)ZFZooma);
  mySerial.write((byte)ZFFocusd);
  mySerial.write((byte)ZFFocusc);
  mySerial.write((byte)ZFFocusb);
  mySerial.write((byte)ZFFocusa);
}
/************* Init ****************/
void ViscaInit(OSCMessage *_mes) {
  ViscaMsg[3] =  0x19;
  int strSize=_mes->getArgStringSize(0);
  char value[strSize]; //string memory allocation
  _mes->getArgString(0,value);
  if ( memcmp(value,"lens",4) == 0) {
  ViscaMsg[4] =  0x01;
  } 
  if ( memcmp(value,"camera",6) == 0) {
  ViscaMsg[4] =  0x03;
  } 
  mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
}  
/************* White Balance ****************/
void ViscaWB(OSCMessage *_mes) {
    ViscaMsg[3] =  0x35;
  int strSize=_mes->getArgStringSize(0);
  char value[strSize]; //string memory allocation
  _mes->getArgString(0,value);  
  if ( memcmp(value,"auto",4) == 0) {
    ViscaMsg[4] =  0x00;
  }
  if ( memcmp(value,"indoor",6) == 0) {
    ViscaMsg[4] =  0x01;
  }
  if ( memcmp(value,"outdoor",7) == 0) {
    ViscaMsg[4] =  0x02;
  }
  if ( memcmp(value,"onepush",7) == 0) {
    ViscaMsg[4] =  0x03;
  }
  if ( memcmp(value,"ATW",3) == 0) {
    ViscaMsg[4] =  0x04;
  }
  if ( memcmp(value,"manual",6) == 0) {
    ViscaMsg[4] =  0x05;
  }
  if ( memcmp(value,"onepushtrigger",14) == 0) {
    ViscaMsg[3] =  0x10;
    ViscaMsg[4] =  0x05;
  }
  if ( memcmp(value,"outdoor auto",12) == 0) {
    ViscaMsg[4] =  0x06;
  }
  if ( memcmp(value,"sodium lamp auto",16) == 0) {
    ViscaMsg[4] =  0x07;
  }
  if ( memcmp(value,"sodium lamp",11) == 0) {
    ViscaMsg[4] =  0x08;
  }
    mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
}
/************* AE Exposure Mode ****************/
void ViscaExposure(OSCMessage *_mes) {
    ViscaMsg[3] =  0x39;
  int strSize=_mes->getArgStringSize(0);
  char value[strSize]; //string memory allocation
  _mes->getArgString(0,value);
  if ( memcmp(value,"auto",4) == 0) {
    ViscaMsg[4] =  0x00;
  }
  if ( memcmp(value,"manual",6) == 0) {
    ViscaMsg[4] =  0x03;
  }
  if ( memcmp(value,"shutter",7) == 0) {
    ViscaMsg[4] =  0x0A;
  }
  if ( memcmp(value,"iris",4) == 0) {
    ViscaMsg[4] =  0x0B;
  }
  if ( memcmp(value,"bright",6) == 0) {
    ViscaMsg[4] =  0x0D;
  }
    mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
}
/************* Slow Shutter ****************/
void ViscaSlowShutter(OSCMessage *_mes) {
  ViscaMsg[3] =  0x5A;
  int strSize=_mes->getArgStringSize(0);
  char value[strSize]; //string memory allocation
  _mes->getArgString(0,value);
  if ( memcmp(value,"auto",4) == 0) {
  ViscaMsg[4] =  0x02;
  } 
  if ( memcmp(value,"manual",6) == 0) {
  ViscaMsg[4] =  0x03;
  } 
    mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
}
/************* Shutter ****************/
void ViscaShutter(OSCMessage *_mes) {
  int value = _mes->getArgInt32(0);
  int valuea = value & 15; 
  int valuebZ = value >> 4; 
  int valueb = valuebZ & 15  ; 
  ViscaLongMsg[2] =  0x04;
  ViscaLongMsg[3] =  0x4A;
  ViscaLongMsg[4] =  0x00;
  ViscaLongMsg[5] =  0x00;
  ViscaLongMsg[6] =  valueb;
  ViscaLongMsg[7] =  valuea;
  mySerial.write( ViscaLongMsg, sizeof(ViscaLongMsg) );
}
/************* Auto Response ****************/
void ViscaAutoResponse(OSCMessage *_mes) {
  int value = _mes->getArgInt32(0);
  int valuea = value & 15; 
  int valuebZ = value >> 4; 
  int valueb = valuebZ & 15  ; 
  ViscaLongMsg[2] =  0x04;
  ViscaLongMsg[3] =  0x5D;
  ViscaLongMsg[4] =  0x00;
  ViscaLongMsg[5] =  0x00;
  ViscaLongMsg[6] =  valueb;
  ViscaLongMsg[7] =  valuea;
  mySerial.write( ViscaLongMsg, sizeof(ViscaLongMsg) );
}
/************* IRIS ****************/
void ViscaIris (OSCMessage *_mes) {
  int value = _mes->getArgInt32(0);
  int valuea = value & 15; 
  int valuebZ = value >> 4; 
  int valueb = valuebZ & 15  ; 
  ViscaLongMsg[2] =  0x04;
  ViscaLongMsg[3] =  0x4B;
  ViscaLongMsg[4] =  0x00;
  ViscaLongMsg[5] =  0x00;
  ViscaLongMsg[6] =  valueb;
  ViscaLongMsg[7] =  valuea;
  mySerial.write( ViscaLongMsg, sizeof(ViscaLongMsg) );
}
/************* Gain ****************/
void ViscaGain(OSCMessage *_mes) {
  int value = _mes->getArgInt32(0);
  int valuea = value & 15; 
  int valuebZ = value >> 4; 
  int valueb = valuebZ & 15  ; 
  ViscaLongMsg[2] =  0x04;
  ViscaLongMsg[3] =  0x4C;
  ViscaLongMsg[4] =  0x00;
  ViscaLongMsg[5] =  0x00;
  ViscaLongMsg[6] =  valueb;
  ViscaLongMsg[7] =  valuea;
  mySerial.write( ViscaLongMsg, sizeof(ViscaLongMsg) );
}
/************* Exposition Compensation ****************/
void ViscaExpComp(OSCMessage *_mes) {
  ViscaMsg[3] =  0x3E;
  int value = _mes->getArgInt32(0);
  if ( value == 1 ) {
  ViscaMsg[4] =  0x02;
  } 
  else { 
  ViscaMsg[4] =  0x03;
  } 
    mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
}
/************* Exposition Compensation ****************/
void ViscaExpCompDirect(OSCMessage *_mes) {
  int value = _mes->getArgInt32(0);
  int valuea = value & 15; 
  int valuebZ = value >> 4; 
  int valueb = valuebZ & 15  ; 
  ViscaLongMsg[2] =  0x04;
  ViscaLongMsg[3] =  0x4E;
  ViscaLongMsg[4] =  0x00;
  ViscaLongMsg[5] =  0x00;
  ViscaLongMsg[6] =  valueb;
  ViscaLongMsg[7] =  valuea;
  mySerial.write( ViscaLongMsg, sizeof(ViscaLongMsg) );
}
/************* Backlight ****************/
void ViscaBackLight(OSCMessage *_mes) {
  ViscaMsg[3] =  0x33;
  int value = _mes->getArgInt32(0);
  if ( value == 1 ) {
  ViscaMsg[4] =  0x02;
  } 
  else { 
  ViscaMsg[4] =  0x03;
  } 
    mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
}
/************* Wide Dynamic ****************/
void ViscaWD(OSCMessage *_mes) {
  ViscaMsg[3] =  0x3D;
  int value = _mes->getArgInt32(0);
  if ( value == 1 ) {
  ViscaMsg[4] =  0x02;
  } 
  else { 
  ViscaMsg[4] =  0x03;
  } 
    mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
}
/************* Aperture ****************/
void ViscaAperture(OSCMessage *_mes) {
  int value = _mes->getArgInt32(0);
  int valuea = value & 15; 
  int valuebZ = value >> 4; 
  int valueb = valuebZ & 15  ; 
  ViscaLongMsg[2] =  0x04;
  ViscaLongMsg[3] =  0x42;
  ViscaLongMsg[4] =  0x00;
  ViscaLongMsg[5] =  0x00;
  ViscaLongMsg[6] =  valueb;
  ViscaLongMsg[7] =  valuea;
  mySerial.write( ViscaLongMsg, sizeof(ViscaLongMsg) );
}
/************* Noise Reduction ****************/
void ViscaNR(OSCMessage *_mes) {
  ViscaMsg[3] =  0x53;
  int value = _mes->getArgInt32(0);
  ViscaMsg[4] =  value;
    mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
}
/************* High Resolution ****************/
void ViscaHR(OSCMessage *_mes) {
  ViscaMsg[3] =  0x52;
  int value = _mes->getArgInt32(0);
  if ( value == 1 ) {
  ViscaMsg[4] =  0x02;
  } 
  else { 
  ViscaMsg[4] =  0x03;
  } 
    mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
}
/************* Gamma ****************/
void ViscaGamma(OSCMessage *_mes) {
  ViscaMsg[3] =  0x5B;
  int value = _mes->getArgInt32(0);
  ViscaMsg[4] =  value;
  mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
}
/************* High Sensitivity ****************/
void ViscaHS(OSCMessage *_mes) {
  ViscaMsg[3] =  0x5E;
  int value = _mes->getArgInt32(0);
  if ( value == 1 ) {
  ViscaMsg[4] =  0x02;
  } 
  else { 
  ViscaMsg[4] =  0x03;
  } 
    mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
}
/************* Horizontal Reverse ****************/
void ViscaReverse(OSCMessage *_mes) {
  ViscaMsg[3] =  0x61;
  int value = _mes->getArgInt32(0);
  if ( value == 1 ) {
  ViscaMsg[4] =  0x02;
  } 
  else { 
  ViscaMsg[4] =  0x03;
  } 
    mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
}
/************* Freeze ****************/
void ViscaFreeze(OSCMessage *_mes) {
  ViscaMsg[3] =  0x62;
  int value = _mes->getArgInt32(0);
  if ( value == 1 ) {
  ViscaMsg[4] =  0x02;
  } 
  else { 
  ViscaMsg[4] =  0x03;
  } 
    mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
}
/************* Image effects ****************/
void ViscaFX(OSCMessage *_mes) {
  ViscaMsg[3] =  0x63;
  int strSize=_mes->getArgStringSize(0);
  char value[strSize]; //string memory allocation
  _mes->getArgString(0,value);
  if ( memcmp(value,"off",3) == 0) {
  ViscaMsg[4] = ((uint8_t) 0);
  } 
  if ( memcmp(value,"negart",7) == 0) {
  ViscaMsg[4] =  0x02;
  } 
  if ( memcmp(value,"bw",2) == 0) {
  ViscaMsg[4] =  0x04;
  } 
  mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
}
/************* Vertical Flip****************/
void ViscaFlip(OSCMessage *_mes) {
  ViscaMsg[3] =  0x66;
  int value = _mes->getArgInt32(0);
  if ( value == 1 ) {
  ViscaMsg[4] =  0x02;
  } 
  else { 
  ViscaMsg[4] =  0x03;
  } 
    mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
}
/************* Infra-Red ****************/
void ViscaIR(OSCMessage *_mes) {
  ViscaMsg[3] =  0x01;
  int strSize=_mes->getArgStringSize(0);
  char value[strSize]; //string memory allocation
  _mes->getArgString(0,value);
  if ( memcmp(value,"on",2) == 0) {
  ViscaMsg[4] =  0x02;
  } 
  else { 
  ViscaMsg[4] =  0x03;
  }
  mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
} 
/************* Infra-Red Correction ****************/
void ViscaIRCorrection(OSCMessage *_mes) {
  ViscaMsg[3] =  0x11;
  int strSize=_mes->getArgStringSize(0);
  char value[strSize]; //string memory allocation
  _mes->getArgString(0,value);
  if ( memcmp(value,"on",2) == 0) {
  ViscaMsg[4] =  0x01;
  } 
  else { 
  ViscaMsg[4] =  0x00;
  }
  mySerial.write( ViscaMsg, sizeof(ViscaMsg) );

}   
/************* Stabilizer****************/
void ViscaStab(OSCMessage *_mes) {
  ViscaMsg[3] =  0x34;
  int value = _mes->getArgInt32(0);
  if ( value == 1 ) {
  ViscaMsg[4] =  0x02;
  } 
  else { 
  ViscaMsg[4] =  0x03;
  } 
    mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
}
/************* Memory Reset ****************/
void ViscaMemReset(OSCMessage *_mes) {
uint8_t ViscaMemMsg[7] = {  
  0x81, 0x01, 0x04, 0x3F, 0x00, 0x00, 0xFF      };
  int value = _mes->getArgInt32(0);
  ViscaMemMsg[5] =  value;
  mySerial.write( ViscaMemMsg, sizeof(ViscaMemMsg) );
}
/************* Memory Set ****************/
void ViscaMemSet(OSCMessage *_mes) {
uint8_t ViscaMemMsg[7] = {  
  0x81, 0x01, 0x04, 0x3F, 0x01, 0x00, 0xFF      };
  int value = _mes->getArgInt32(0);
  ViscaMemMsg[5] =  value;
  mySerial.write( ViscaMemMsg, sizeof(ViscaMemMsg) );
}
/************* Memory Recall  ****************/
void ViscaMemRecall(OSCMessage *_mes) {
uint8_t ViscaMemMsg[7] = {  
  0x81, 0x01, 0x04, 0x3F, 0x02, 0x00, 0xFF      };
  int value = _mes->getArgInt32(0);
  ViscaMemMsg[5] =  value;
  mySerial.write (ViscaMemMsg, sizeof(ViscaMemMsg) );
}
/************* Chroma Supress  ****************/
void ViscaChromaSuppress(OSCMessage *_mes) {
  ViscaMsg[3] =  0x5F;
  int value = _mes->getArgInt32(0);
  ViscaMsg[4] =  value;
  mySerial.write( ViscaMsg, sizeof(ViscaMsg) );
}
/************* Home (pan-tilt) ****************/
void ViscaHome(OSCMessage *_mes) {
  mySerial.write(0x81);
  mySerial.write(0x01);
  mySerial.write(0x06);
  mySerial.write(0x04);
  mySerial.write(0xFF); 
  }
  /************* Reset (pan-tilt) ****************/
void ViscaReset(OSCMessage *_mes) {
  mySerial.write(0x81);
  mySerial.write(0x01);
  mySerial.write(0x06);
  mySerial.write(0x05);
  mySerial.write(0xFF); 
  }
/************* PanSpeed (pan-tilt) ****************/
void ViscaPanSpeed (OSCMessage *_mes) {
  PanSpeed = _mes->getArgInt32(0);
  }
/************* TiltSpeed (pan-tilt) ****************/
void ViscaTiltSpeed (OSCMessage *_mes) {
  TiltSpeed = _mes->getArgInt32(0);
  }
/************* Up (pan-tilt) ****************/
  void ViscaUp(OSCMessage *_mes) {
  ViscaLongMsg[2] =  0x06;
  ViscaLongMsg[3] =  0x01;
  ViscaLongMsg[4] =  PanSpeed;
  ViscaLongMsg[5] =  TiltSpeed;
  ViscaLongMsg[6] =  0x03;
  ViscaLongMsg[7] =  0x01;
  mySerial.write( ViscaLongMsg, sizeof(ViscaLongMsg) );
}
/************* Down (pan-tilt) ****************/
  void ViscaDown(OSCMessage *_mes) {
  ViscaLongMsg[2] =  0x06;
  ViscaLongMsg[3] =  0x01;
  ViscaLongMsg[4] =  PanSpeed;
  ViscaLongMsg[5] =  TiltSpeed;
  ViscaLongMsg[6] =  0x03;
  ViscaLongMsg[7] =  0x02;
  mySerial.write( ViscaLongMsg, sizeof(ViscaLongMsg) );
}
/************* Left (pan-tilt) ****************/
  void ViscaLeft(OSCMessage *_mes) {
  ViscaLongMsg[2] =  0x06;
  ViscaLongMsg[3] =  0x01;
  ViscaLongMsg[4] =  PanSpeed;
  ViscaLongMsg[5] =  TiltSpeed;
  ViscaLongMsg[6] =  0x01;
  ViscaLongMsg[7] =  0x03;
  mySerial.write( ViscaLongMsg, sizeof(ViscaLongMsg) );
}
/************* Right (pan-tilt) ****************/
  void ViscaRight(OSCMessage *_mes) {
  ViscaLongMsg[2] =  0x06;
  ViscaLongMsg[3] =  0x01;
  ViscaLongMsg[4] =  PanSpeed;
  ViscaLongMsg[5] =  TiltSpeed;
  ViscaLongMsg[6] =  0x02;
  ViscaLongMsg[7] =  0x03;
  mySerial.write( ViscaLongMsg, sizeof(ViscaLongMsg) );
}
/************* UpLeft (pan-tilt) ****************/
  void ViscaUpLeft(OSCMessage *_mes) {
  ViscaLongMsg[2] =  0x06;
  ViscaLongMsg[3] =  0x01;
  ViscaLongMsg[4] =  PanSpeed;
  ViscaLongMsg[5] =  TiltSpeed;
  ViscaLongMsg[6] =  0x01;
  ViscaLongMsg[7] =  0x01;
  mySerial.write( ViscaLongMsg, sizeof(ViscaLongMsg) );
}
/************* UpRight (pan-tilt) ****************/
  void ViscaUpRight(OSCMessage *_mes) {
  ViscaLongMsg[2] =  0x06;
  ViscaLongMsg[3] =  0x01;
  ViscaLongMsg[4] =  PanSpeed;
  ViscaLongMsg[5] =  TiltSpeed;
  ViscaLongMsg[6] =  0x02;
  ViscaLongMsg[7] =  0x01;
  mySerial.write( ViscaLongMsg, sizeof(ViscaLongMsg) );
}
/************* DownLeft (pan-tilt) ****************/
  void ViscaDownLeft(OSCMessage *_mes) {
  ViscaLongMsg[2] =  0x06;
  ViscaLongMsg[3] =  0x01;
  ViscaLongMsg[4] =  PanSpeed;
  ViscaLongMsg[5] =  TiltSpeed;
  ViscaLongMsg[6] =  0x01;
  ViscaLongMsg[7] =  0x02;
  mySerial.write( ViscaLongMsg, sizeof(ViscaLongMsg) );
}
/************* DownRight (pan-tilt) ****************/
  void ViscaDownRight(OSCMessage *_mes) {
  ViscaLongMsg[2] =  0x06;
  ViscaLongMsg[3] =  0x01;
  ViscaLongMsg[4] =  PanSpeed;
  ViscaLongMsg[5] =  TiltSpeed;
  ViscaLongMsg[6] =  0x02;
  ViscaLongMsg[7] =  0x02;
  mySerial.write( ViscaLongMsg, sizeof(ViscaLongMsg) );
}
/************* Stop (pan-tilt) ****************/
  void ViscaStop(OSCMessage *_mes) {
  ViscaLongMsg[2] =  0x06;
  ViscaLongMsg[3] =  0x01;
  ViscaLongMsg[4] =  PanSpeed;
  ViscaLongMsg[5] =  TiltSpeed;
  ViscaLongMsg[6] =  0x03;
  ViscaLongMsg[7] =  0x03;
  mySerial.write( ViscaLongMsg, sizeof(ViscaLongMsg) );
}
