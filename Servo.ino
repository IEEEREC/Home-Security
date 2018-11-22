//////////////////////////////////////////////////////////////////////////////////
//                               Setup for Servo                                //
//////////////////////////////////////////////////////////////////////////////////

/*
 * Connection Map
 * 
 * Orange - Pin 14
 * RED - USB
 * Brown - Gnd
 */
 
#include <Servo.h>

Servo myservo;  // create servo object to control a servo
int pos = 0;    // variable to store the servo position

//////////////////////////////////////////////////////////////////////////////////
//                             Setup for CAYENNE IOT                            //
//////////////////////////////////////////////////////////////////////////////////

//#define CAYENNE_DEBUG
#define CAYENNE_PRINT Serial
#include <CayenneMQTTESP8266.h>

// WiFi network info.
char ssid[] = "ssid";
char wifiPassword[] = "pass";

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "username";
char password[] = "pass";
char clientID[] = "client ID";


//////////////////////////////////////////////////////////////////////////////////
//                                      Setup                                   //
//////////////////////////////////////////////////////////////////////////////////


void setup() { 
  Serial.begin(115200);
  
  // Initilize the Servo on pin 14.
  myservo.attach(14);
  
  
  Cayenne.begin(username, password, clientID, ssid, wifiPassword);
  pinMode(0,OUTPUT);
}



//////////////////////////////////////////////////////////////////////////////////
//                                      Loop                                    //
//////////////////////////////////////////////////////////////////////////////////
void loop() {
  Cayenne.loop();
}


//////////////////////////////////////////////////////////////////////////////////
//                              Extra Cayenne Code                              //
//////////////////////////////////////////////////////////////////////////////////


// Default function for sending sensor data at intervals to Cayenne.
// You can also use functions for specific channels, e.g CAYENNE_OUT(1) for sending channel 1 data.
CAYENNE_OUT_DEFAULT()
{
	// Write data to Cayenne here. This example just sends the current uptime in milliseconds on virtual channel 0.
	// Cayenne.virtualWrite(0, millis());
	// Some examples of other functions you can use to send data.
	//Cayenne.celsiusWrite(1, 22.0);
	//Cayenne.luxWrite(2, 700);
	//Cayenne.virtualWrite(3, 50, TYPE_PROXIMITY, UNIT_CENTIMETER);
}

// Default function for processing actuator commands from the Cayenne Dashboard.
// You can also use functions for specific channels, e.g CAYENNE_IN(1) for channel 1 commands.
CAYENNE_IN_DEFAULT()
{
	CAYENNE_LOG("Channel %u, value %s", request.channel, getValue.asString());
	//Process message here. If there is an error set an error message using getValue.setError(), e.g getValue.setError("Error message");
}



//////////////////////////////////////////////////////////////////////////////////
//                              Servo Comparison Code                             //
//////////////////////////////////////////////////////////////////////////////////



CAYENNE_IN(V3){
  int servoState = getValue.asInt();
  
  Serial.println(servoState);
  digitalWrite (0,servoState);
  
  if(servoState)
  {
	  
      // Unlocking	  
      for (pos = 0; pos <= 180; pos += 5) 
      { // goes from 0 degrees to 180 degree in steps of 1 degree
          myservo.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position
      }
	  
      
      delay(3000);	// Keep the door open for 3 sec.  
	  
      // Locking	  
      for (pos = 180; pos >= 0; pos -= 5) // goes from 180 degrees to 0 degrees
      { 
        myservo.write(pos);              // tell servo to go to position in variable 'pos'
        delay(15);                       // waits 15ms for the servo to reach the position
      }
  } 
}
