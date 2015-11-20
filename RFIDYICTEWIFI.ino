
/*
* Code to read RFID tags and then log an entry to a website
 */

 /*
 * MFRC522 - Library to use ARDUINO RFID MODULE KIT 13.56 MHZ WITH TAGS SPI W AND R BY COOQROBOT.
 * The library file MFRC522.h has a wealth of useful info. Please read it.
 * The functions are documented in MFRC522.cpp.
 *
 * Based on code Dr.Leong   ( WWW.B2CQSHOP.COM )
 * Created by Miguel Balboa (circuitito.com), Jan, 2012.
 * Rewritten by Søren Thing Andersen (access.thing.dk), fall of 2013 (Translation to English, refactored, comments, anti collision, cascade levels.)
 * Released into the public domain.
 *
 * Sample program showing how to read data from a PICC using a MFRC522 reader on the Arduino SPI interface.
 *----------------------------------------------------------------------------- empty_skull 
 * Aggiunti pin per arduino Mega
 * add pin configuration for arduino mega
 * http://mac86project.altervista.org/
 ----------------------------------------------------------------------------- Nicola Coppola
 * Pin layout should be as follows:
 * Signal     Pin              Pin               Pin
 *            Arduino Uno      Arduino Mega      MFRC522 board
 * ------------------------------------------------------------
 * Reset      9                5                 RST
 * SPI SS     10               53                SDA
 * SPI MOSI   11               51                MOSI
 * SPI MISO   12               50                MISO
 * SPI SCK    13               52                SCK
 *
 * The reader can be found on eBay for around 5 dollars. Search for "mf-rc522" on ebay.com. 
 */

#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <string.h>
#include <HomeWiFi.h> //  credentials for wireless network
#include <RFIDSettings.h> // WEBSERVER and TAGINURL settings

#define RST_PIN		9
#define SS_PIN		10

// Although with a Galileo board already on the wifi, I am not sure that
// WiFi.begin is actually required...?

char ssid[] = WIFI_SSID; //  your network SSID (name) 
char pass[] = WIFI_PASSWORD;    // your network password (use for WPA, or use as key for WEP)

char webServer[] = WEBSERVER; // e.g. myserver.com
char webURL[] = TAGINURL; // e.g. cgi-bin/tagin.cgi

int cardID[8];
//int ethansCard[4] = {9, 110, 122, 158};
//byte ethansCard[4] = {0x9, 0x6E, 0x7A, 0x9E };
byte ethansCard[4] = {0xC4, 0x5D, 0xD2, 0xB7 };
byte ronansCard[4] = {0x6A, 0x32, 0x20, 0x2B };
byte ivansCard[4] = { 0x74, 0x1D, 0xDB, 0xB7 };
byte swaglordsTag[4] = { 0x01, 0x1A, 0x35, 0x40 };

int matchStatus;

WiFiClient wirelessClient;
int status = WL_IDLE_STATUS;

MFRC522 mfrc522(SS_PIN, RST_PIN);	// Create MFRC522 instance

void setup() {
	Serial.begin(9600);		// Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  
 // No need to connect to Wifi network: on Galileo the board is already connected
 //status = 0;
  /*while (status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
    status = WiFi.begin(ssid, pass);
  
    // wait for connection:
    delay(2000);
  } 
  Serial.println("Connected to wifi");  */
  //printWifiStatus();
  
	
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	ShowReaderDetails();	// Show details of PCD - MFRC522 Card Reader details
	Serial.println(F("Scan tag to check in to class..."));
}

void loop() {
	// Look for new cards
	if ( ! mfrc522.PICC_IsNewCardPresent()) {
		delay(100);
	}

	// Select one of the cards
	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}
  Serial.print("This card has UID: ");
  printCardUID(mfrc522.uid.uidByte,mfrc522.uid.size);

  if (memcmp(mfrc522.uid.uidByte, ethansCard, 4) == 0) {
    Serial.println("Welcome Ethan :)");
    TagIn("ethan");
  } else if (memcmp(mfrc522.uid.uidByte, ronansCard, 4)==0) {
    Serial.println("Welcome Ronan :)");
    TagIn("ronan");
  } else if (memcmp(mfrc522.uid.uidByte, ivansCard, 4)==0) {
    Serial.println("Welcome Ronan :)");
    TagIn("ivan");
  } else if (memcmp(mfrc522.uid.uidByte, swaglordsTag, 4)==0) {
    Serial.println("Welcome swaglord");
    TagIn("swaglord");
  } else {
    Serial.println("This tag is unknown!");
  }
  
  delay(300);

  
	// Dump debug info about the card; PICC_HaltA() is automatically called
	//mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
}

void printCardUID(byte IDarray[10], int IDsize) {
  
  for (byte i = 0; i < IDsize; i++) {
    if(IDarray[i] < 0x10)
      Serial.print(F(" 0"));
    else
      Serial.print(F(" "));
      Serial.print(IDarray[i], HEX);
      cardID[i] = IDarray[i];
  } 
  Serial.println();
}

void TagIn(char *tagname) {
  char URLString[100];
	char HOSTLine[100];
	
  WiFiClient client;
  //Serial.print("TagIn function: You have asked me to tag in the user: ");
  //Serial.println(tagname);
  
   Serial.print("\nStarting connection to ");
   Serial.println(webServer);
  
  if (client.connect(webServer, 80)) {
    Serial.println("connected to server successfully. ");
    
    // Construct the HTTP request:
    sprintf(URLString,"GET %s?name=%s HTTP/1.0",TAGINURL,tagname);
	  sprintf(HOSTLine,"Host: %s",WEBSERVER);

    client.println(URLString);

    client.println(HOSTLine);
    
    client.println("Connection: close");
    client.println();
    
    Serial.print("Sent tag-in request for ");
    Serial.println(tagname);
    // Print out any response from the server...
    //while (client.available()) {
      //char c = client.read();
      //Serial.write(c);
    //}
  } else {
    Serial.print("Connection to webserver ");
    Serial.print(webServer);
    Serial.println(" failed!");
  }
  
  //client.stop();  This SEGFAULTs
  
  
}



void printCardID(int cardID[4]) {
  for (byte i = 0; i < 8; i++) {
    if(cardID[i] < 0x10)
      Serial.print(F(" 0"));
    else
      Serial.print(F(" "));
      Serial.print(cardID[i], HEX);
  } 
  Serial.println();
}

void ShowReaderDetails() {
	// Get the MFRC522 software version
	byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
	Serial.print(F("MFRC522 Software Version: 0x"));
	Serial.print(v, HEX);
	if (v == 0x91)
		Serial.print(F(" = v1.0"));
	else if (v == 0x92)
		Serial.print(F(" = v2.0"));
	else
		Serial.print(F(" (unknown)"));
	Serial.println("");
	// When 0x00 or 0xFF is returned, communication probably failed
	if ((v == 0x00) || (v == 0xFF)) {
		Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
	}
}
