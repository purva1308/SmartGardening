#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <I2CSoilMoistureSensor.h>
#include <PubSubClient.h>


#define WIFI_AP "****"
#define WIFI_PASSWORD "******"

#define TOKEN "xxxxxxxxxxxx"
char thingsboardServer[] = "xxxxxxxxxx";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

char msg[50];
int value = 0;
int interval = 5;
//String devID = "5b3b6f1cb5a7bd1644890d1f";
//String devID = "7f2f2b70-8f55-11e8-9d40-991883b61f12"; //thingsboard

int status = WL_IDLE_STATUS;

unsigned long lastSend;
long lastMsg = 0;

const int sensor_pin = A0;  /* Connect Soil moisture analog sensor pin to A0 of NodeMCU */
int pump_pin=16;

void setup()
{
  Serial.begin(9600);
  pinMode(pump_pin,OUTPUT);
  delay(10);
  InitWiFi();
  client.setServer( thingsboardServer, 1883 );
  lastSend = 0;
}

void loop()
{
  if ( !client.connected() ) {
    reconnect();
  }

  if ( millis() - lastSend > 1000 ) { // Update and send only after 1 seconds
    getSoilMoisture();
    lastSend = millis();
  }

  client.loop();
}

void getSoilMoisture()
{
  Serial.println("Collecting moisture data.");

  // Reading soil moisture

float soilMoisture;
 soilMoisture = ( 100.00 - ( (analogRead(sensor_pin)/1023.00) * 100.00 ) );
 Serial.print("Soil Moisture(in Percentage) = ");
  

  // Check if any reads failed and exit early (to try again).
  if (isnan(soilMoisture)) {
    Serial.println("Failed to read from soil moisture sensor!");
    return;
  }

Serial.print(soilMoisture);
Serial.println("%");
snprintf(msg,5,"%f",soilMoisture);

//DynamicJsonBuffer jsonBuffer;
//JsonObject& sndData = jsonBuffer.createObject();
//
////sndData["_id"] = devID;      
//sndData["reading"] = soilMoisture;
//sndData["interval"] = interval;

//String sendJData = "";
//sndData.printTo(sendJData);
//Serial.println(sendJData);

String moisture= String(soilMoisture);
  
  
// Just debug messages
  Serial.println( "Sending moisture : [" );
  Serial.println( moisture ); 
  Serial.println( "]   -> " );

  // Prepare a JSON payload string
  String payload = "{";
  payload += "\"moisture\":"; payload += moisture; 
  payload += "}";

  // Send payload
  char attributes[100];
  payload.toCharArray( attributes, 100 );
  client.publish("v1/devices/me/telemetry", attributes);
  Serial.println("Data Published");
  Serial.println(attributes);

//-----------------------------------------------------------------------------------------------------
// String response;

//  Serial.println();
// if(response=="on"){
//    Serial.println(response);
//  digitalWrite(pump_pin,HIGH);
// }
// else if(response=="off"){
//  digitalWrite(pump_pin,LOW);
//     Serial.println(response);
// }
// else{
//  interval = response.toInt();
}
//client.publish("soil", sendJData.c_str());
//delay(1000*interval);

//}

void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("ESP8266 Device", TOKEN, NULL) ) {
      Serial.println( "[DONE]" );

//------*----*------*-------*--------*-------*------*--------*--------*------*------*-----*------*-----*-

    //For client Attributes

      float firmware_version,serial_number;

      String Firmware_version = String(firmware_version);
      String Serial_number = String(serial_number);

       // Prepare a JSON payload string
  String payload = "{";
  payload += "\"Firmware_version\":"; payload += "1.0.1"; payload += ",";
  payload += "\"Serial_number\":"; payload += "SmartGardening";
  payload += "}";

  // Send payload
  char telemetry[100];
  payload.toCharArray( telemetry, 100 );
  client.publish("v1/devices/me/attributes", telemetry);
  Serial.println("Data Published");
  Serial.println(telemetry);

//----*----*------*-------*------*--------*--------*------*------*---------*-------*-------*------*------



      
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}


