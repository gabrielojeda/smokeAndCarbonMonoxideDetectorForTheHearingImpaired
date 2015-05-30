/*
*  Smoke and CO Detector for the Deaf and Hard of Hearing
*  Electrical Engineering Senior Design
*  2014-2015
*
*  Written by Gabriel Ojeda
*
*  Referenced Code
*  Arduino, Temp, Humidity, WiFi, MySQL and Highcharts
*  http://www.instructables.com/id/Arduino-Temp-Humidity-WiFi-MySQL-and-Highcharts/
*
*  Open Home Automation
*  https://learn.adafruit.com/wifi-weather-station-arduino-cc3000/introduction
*  
*  Adafruit
*  https://learn.adafruit.com/adafruit-cc3000-wifi
*/

/* Required Libraries */
#include <Adafruit_CC3000.h>
#include <SPI.h>
#include <stdlib.h>

/* CC3000 Wi-Fi Module Pins */
#define ADAFRUIT_CC3000_IRQ   3
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10

/* Wi-Fi Network Name and Password */
#define WLAN_SSID       "NETGEAR53"        
#define WLAN_PASS       "helpfulsky459"
#define WLAN_SECURITY   WLAN_SEC_WPA2      

/* CC3000 Instances */
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT, SPI_CLOCK_DIV2);

/* Local Server IP address, Webserver Port, and Server Repository */
uint32_t ip = cc3000.IP2U32(192,168,1,5);      //Computer IP address
int port = 80;                                 //Webserver port
String repository = "/sht15_cc3000_mysql/";    //Folder on your webserver where the sensor.php file is located

/* Threshold Values for the Smoke and CO Sensors */
int smokeThreshold = 200;
int coThreshold = 150;

/* Global Notification Messages */
String initialValue = "0.00";
String bothDetectedMessage = "Both_Detected!";
String smokeDetectedMessage = "Smoke_Detected!";
String coDetectedMessage = "CO_Detected!";

/* Flag variables */
int initialFlag = 1;
int finalFlag = 1;

void setup(void)
{
  Serial.begin(115200);
    
  /* Initialize the CC3000 Wi-Fi module */
  if (!cc3000.begin())
  {
    while(1);
  }

  /* Connect to the Wi-Fi Network */
  Serial.println("Connecting to Wi-Fi network!") + Serial.println("");
  cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY);
  Serial.println("Connected to Wi-Fi network!") + Serial.println("");
    
  /* Check DHCP */
  Serial.println(F("Request DHCP")) + Serial.println("");
  while (!cc3000.checkDHCP())
  {
    delay(100);
  }
  
  /* Send request */
  String request = "GET "+ repository + "sensor.php?temp=" + initialValue + " HTTP/1.0";
  send_request(request);
  Serial.println("");
  Serial.print("request: ");
  Serial.println(request);
  Serial.println(""); 
}

void loop(void)
{
  int smokeValue = analogRead(A0);
  int coValue = analogRead(A2);

  /* Print Data */
  Serial.print("Smoke Value (Analog): ") + Serial.print(smokeValue) + Serial.print("CO Value (Analog): ") + Serial.print(coValue) + Serial.println();
  delay(500);
  
  if(initialFlag)
  {
    if((smokeValue > smokeThreshold) && (coValue > coThreshold))
    {
      /* Send request */
      String request = "GET "+ repository + "sensor.php?both=" + bothDetectedMessage + " HTTP/1.0";
      send_request(request);
      Serial.println("");
      Serial.print("request: ");
      Serial.println(request);
      Serial.println("");
      
      delay(3000);    //Update every 3 seconds
      
      initialFlag = 0; 
    }
    
    else if(smokeValue > smokeThreshold)
    {                                                     
      /* Send request */
      String request = "GET "+ repository + "sensor.php?temp=" + smokeDetectedMessage + " HTTP/1.0";
      send_request(request);
      Serial.println("");
      Serial.print("request: ");
      Serial.println(request);
      Serial.println("");
       
      delay(3000);    //Update every 3 seconds
    
      initialFlag = 0;
    }
    
    else if(coValue > coThreshold)
    {
      /* Send request */
      String request = "GET "+ repository + "sensor.php?hum=" + coDetectedMessage + " HTTP/1.0";
      send_request(request);
      Serial.println("");
      Serial.print("request: ");
      Serial.println(request);
      Serial.println("");
       
      delay(3000);    // Update every 3 seconds
       
      initialFlag = 0;
    }
     
    if (finalFlag && !initialFlag)
    {
      /* Send request */
      String request = "GET "+ repository + "sensor.php?temp=" + initialValue + " HTTP/1.0";
      send_request(request);
      Serial.println("");
      Serial.print("request: ");
      Serial.println(request);
      Serial.println("");
  
      finalFlag = 0;
    }
  }
}

/* Function to Send a TCP Request and Get the Result as a String */
void send_request (String request) 
{
  /* Connect to Server */
  Serial.println("Starting connection to server...");
  Adafruit_CC3000_Client client = cc3000.connectTCP(ip, port);
  
  /* Send request */
  if (client.connected())
  {
    client.println(request);
    client.println(F(""));
    Serial.println("Connected & Data sent");
  }
  
  else
  {
    Serial.println(F("Connection failed"));
  }
  
  while (client.connected())
  {
    while (client.available())
    {
      char c = client.read();    //Read answer
    }
  }
  
  Serial.println("Closing connection");
  Serial.println("");
  client.close();
}
