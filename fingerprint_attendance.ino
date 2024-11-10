#include <time.h>
#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include "WiFiClientSecureBearSSL.h"
#include <ESP8266Ping.h>

String x[] =
{"0"
,
//1
"عصام احمد"
,
//2
"مازن"
,
//3
"فتحي علي"
,
//4
"فتحي علي"
,
//5
"فتحي علي"
,
//6
"فتحي علي"
,
//7
"فتحي علي"
,
//8
"فتحي علي"
,
//9
"فتحي علي"
,
//10
"فتحي علي"
,
//11
"فتحي علي"
,
//12
"فتحي علي"
,
//13
"خالد علي"
};

bool greenState =0;
bool redState =0;
int currentMillis;
int startMillis;


// On Leonardo/Micro or others with hardware serial, use those! #0 is green wire, #1 is white
// uncomment this line:
// #define mySerial Serial1

// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// comment these two lines if using hardware serial
//const char* NAME;
String NAME;
//const char* ID;
String ID;

String Event_Name = "fingerprint";

String Key = "//IFTTT key of your account//";

// Replace with your unique IFTTT URL resource
String resource = "/trigger/" + Event_Name + "/with/key/" + Key;

// Maker Webhooks IFTTT
const char* server = "maker.ifttt.com";

// Replace with your SSID and Password
const char* ssid     = "//WIFI//"; 
const char* password = "//Password//"; 
SoftwareSerial mySerial(D6, D5);




Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup()
{
 pinMode(D3,OUTPUT);
 pinMode(D7,OUTPUT);
 Serial.begin(9600);
  finger.begin(57600);
 // while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) {
      delay(1);
    }
  }

  finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  Serial.println("Waiting for valid finger...");

  Serial.print("Connecting to: ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
 
 
  int timeout = 10 * 4; // 10 seconds
  while (WiFi.status() != WL_CONNECTED  && (timeout-- > 0)) {
    digitalWrite(D7,0);
    greenState = !greenState;
    digitalWrite(D3,greenState);
    delay(250);
    Serial.print(".");
  }
  Serial.println("");

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect, going back to sleep");
    digitalWrite(D3,0);
    digitalWrite(D7,1);
  }

  Serial.print("WiFi connected in: ");
  Serial.print(millis());
  Serial.print(", IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(D3,0);
  startMillis = millis();
}
 
void loop()                     // run over and over again
{

currentMillis = millis();
  if (currentMillis - startMillis >= 45000)
  {

if(Ping.ping("www.google.com", 3) == 1)
{
  digitalWrite(D7,LOW);  
}
else
{
  digitalWrite(D7,HIGH);    
}
startMillis = currentMillis;
  }
  
  getFingerprintIDez();

  
  if (finger.fingerID >0 ) 
{

   Attend(finger.fingerID);
}

  
  finger.fingerID = 0;
}


void Attend( int id)
{

 Serial.print("!!--");
    Serial.println(id);
    NAME = x[id];
    ID = String(id);
    if (finger.confidence >= 60) {
      Serial.print("Attendace Marked for "); Serial.println(NAME);
      makeIFTTTRequest();
    }
  
}


uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) 
  {
   
    return -1;
  }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) 
  
  {
     digitalWrite(D7,HIGH);
    delay(150);
    digitalWrite(D7,LOW);
    return -1;
  }

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  
  {
    digitalWrite(D7,HIGH);
    delay(150);
    digitalWrite(D7,LOW);
  return -1;
  }
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}

// Make an HTTP request to the IFTTT web service
void makeIFTTTRequest() {
  Serial.print("Connecting to ");
  Serial.print(server);

//  WiFiClient client;
BearSSL::WiFiClientSecure client;
client.setInsecure();

  int retries = 5;
  while (!!!client.connect(server, 
443) && (retries-- > 0)) {
    Serial.print(".");
  }
  Serial.println();
  if (!!!client.connected()) {
    Serial.println("Failed to connect...");
  }

  Serial.print("Request resource: ");
  Serial.println(resource);

  // Temperature in Celsius
  String jsonObject = String("{\"value1\":\"") + NAME + "\",\"value2\":\"" + ID
                      + "\"}";

  client.println(String("POST ") + resource + " HTTP/1.1");
  client.println(String("Host: ") + server);
  client.println("Connection: close\r\nContent-Type: application/json");
  client.print("Content-Length: ");
  client.println(jsonObject.length());
  client.println();
  client.println(jsonObject);

  int timeout = 5 * 10; // 5 seconds
  while (!!!client.available() && (timeout-- > 0)) {
    digitalWrite(D7,HIGH);
    delay(50);
    digitalWrite(D7,LOW);
    delay(50);
  }
  if (!!!client.available()) {
    Serial.println("No response...");
    digitalWrite(D7,HIGH);
    delay(150);
  }
  while (client.available()) {
    Serial.write(client.read());
    digitalWrite(D3,HIGH);
    delay(5);
   
  }
  digitalWrite(D3,LOW);
  digitalWrite(D7,LOW);
  Serial.println("\nclosing connection");
  client.stop();
}
