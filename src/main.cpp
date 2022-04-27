#include <Arduino.h>
#include <string.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define   BACKLIGHT_PIN  5
LiquidCrystal_I2C lcd(0x27,16,2);

#define BG95    Serial2
#define DEBUG   Serial

String kinhDo, viDo;

bool askForLocation();
void sendIt(String atCommand);
String receiveIt();
String checkGPSLocation();
void GPSsetup();
bool checkRespForOk( int timeOut ) ;




void setup() {
  // put your setup code here, to run once:
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  lcd.print("GPS"); 
  DEBUG.begin(9600);
  while(!DEBUG.available());
  DEBUG.println("Start");
  GPSsetup();
}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.clear();
  if (askForLocation()){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.println("kd = " + kinhDo);
    lcd.setCursor(0,1);
    lcd.println("Vd = "+ viDo);
    DEBUG.println("kinh do = " + kinhDo);
    DEBUG.println("Vi do = "+ viDo);
  }else{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.println("fail");
    DEBUG.println("fail");
  }
  // bool flag = false;
  // while(!flag){
  //   sendIt("AT");
  //   flag = checkRespForOk(30);
  //   Serial.println("reconnect");
  // }
  // Serial.println("connect");
  // sendIt("AT+QGPSLOC=0,36");
  // String data = checkGPSLocation();
  // Serial.println(data);
  // sendIt("AT+QGPSLOC=0,36");
  // String data = receiveIt();
  // Serial.println(data);
  delay(5000);

}


void GPSsetup() {
  bool flag = false;
  BG95.begin(115200);
  while(!BG95.available());
  // Serial.begin(9600);
  // while(Serial.available());
  while(!flag){
    sendIt("AT");
    flag = checkRespForOk(30);
    if (!flag){
      DEBUG.println("AT");
      DEBUG.println("sending");
    }
  }
  flag = false;
  while(!flag){
    sendIt("AT+QGPSCFG=\"outport\",\"uartnmea\"");
    flag = checkRespForOk(30);
    if (!flag){
      DEBUG.println("AT+QGPSCFG=");
      DEBUG.println("sending");
    }
  }
  flag = false;
  while(!flag){
    sendIt("AT+QGPS=1");
    flag = checkRespForOk(30);
    if (!flag){
      sendIt("AT+QGPSEND");

      DEBUG.println("AT+QGPS=1");
      DEBUG.println("sending");
    }
  }
}


bool askForLocation(){
  String receivedMessage;
  sendIt("AT+QGPSLOC=0,36");
  String received = checkGPSLocation();
  if (received.length() > 0) {
    viDo = received.substring(0, 2) + "*" + received.substring(3,8) + "'" + received.substring(9, received.indexOf(","));
    kinhDo = received.substring(1 + received.indexOf(","), 4 + received.indexOf(",")) + "*" + received.substring(5 + received.indexOf(","), 10 + received.indexOf(",")) + "'" + received.substring(11 + received.indexOf(","));
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
  int timeOut = 60;					// From BG95 Quectel AT Commands Manual
	String response;
	String received = "";
	bool ok = false;
	unsigned long startTime = millis();	// Take time at start for time out

	while ( !ok && ((millis()-startTime) <= (unsigned long) timeOut) ) {
    response = receiveIt();
    if (response.indexOf("\r\nOK\r\n") >=0 ){
      if(response.indexOf("\r\n+QGPSLOC:")>=0){
        response = response.substring(response.indexOf(":"));
        received = response.substring(13,35);
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

bool checkRespForOk( int timeOut ) 
{
	String response;
	bool ok = false;
	bool error = false;
	unsigned long startTime = millis();	// Take time at start for time out

	// Check every char string received for "OK" response
	while ( !error && !ok && ((millis()-startTime) <= (unsigned long) timeOut) ) {
		response += receiveIt();
		ok = (response.indexOf("\r\nOK\r\n") >= 0);	// Check if "OK" is in the response
		error = (response.indexOf("\r\nERROR\r\n") >= 0);	// Check if "ERROR" is in response
	}

	return ok;
}