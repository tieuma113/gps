#include <Arduino.h>
#include <string.h>

#define BG95    Serial1

String kinhDo, viDo;

bool askForLocation();
void sendIt(String atCommand);
String receiveIt();
String checkGPSLocation();
void GPSsetup();




void setup() {
  // put your setup code here, to run once:
  GPSsetup();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (askForLocation()){
    Serial.println(kinhDo);
    Serial.println(viDo);
  }else{
    Serial.println("fail");
  }
}


void GPSsetup() {
  BG95.begin(115200);
  while(!BG95.available());
  Serial.begin(9600);
  while(Serial.available());
  sendIt("AT+QGPSCFG=\"outport\",\"uartnmea\"");
  Serial.println(receiveIt());
  sendIt("AT+QGPS=1");
  Serial.println(receiveIt());
}


bool askForLocation(){
  String receivedMessage;
  sendIt("AT+QGPSLOC=0,360");
  String received = checkGPSLocation();
  if (received.length() > 0) {
    viDo = received.substring(0, received.indexOf(","));
    kinhDo = received.substring(1 + received.indexOf(","));
    return true;
  }else{
    return false;
  }
}

void sendIt ( String atCommand ) 
{
	atCommand += "\r\n";
  BG95.print(atCommand);
}

String checkGPSLocation(){
  int timeOut = 360;					// From BG95 Quectel AT Commands Manual
	String response;
	String received = "";
	bool ok = false;
	unsigned long startTime = millis();	// Take time at start for time out

	while ( !ok && ((millis()-startTime) <= (unsigned long) timeOut) ) {
    response = receiveIt();
    if (response.indexOf("\r\nOK\r\n") >=0 ){
      if(response.indexOf("\r\n+QGPSLOC:")>=0){
        received = response.substring(10,32);
      }
    }
  }
  return received;
}

String receiveIt ( ) 
{
	String received;

	// Save data received from BG95 module
	while (BG95.available()) {
		received += (char)BG95.read ();
	}

	return received;					// Return string received
}