#include <Arduino.h>
#include <string.h>

#define BG95    Serial1

String kinhDo, viDo;

bool askForLocation();
void sendIt(String atCommand);
String receiveIt();
String checkGPSLocation();




void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
  
}





bool askForLocation(){
  String receivedMessage;
  sendIt("AT+QGPSLOC=0");
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
  int timeOut = 300;					// From BG96 Quectel AT Commands Manual
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

	// Save data received from BG96 module
	while (BG95.available()) {
		received += (char)BG95.read ();
	}

	return received;					// Return string received
}