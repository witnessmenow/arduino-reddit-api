/*******************************************************************
 *  An example of usisng the Reddit API library to get
 *  info on a given sub reddit
 *
 *  Written by Brian Lough
 *  https://www.youtube.com/channel/UCezJOfu7OtqGzd5xrP3q6WA
 *******************************************************************/

#include "RedditApi.h"

 // ----------------------------
 // Standard Libraries - Already Installed if you have ESP32 set up
 // ----------------------------

#include <WiFi.h>
#include <WiFiClientSecure.h>

// ----------------------------
// Additional Libraries - each one of these will need to be installed.
// ----------------------------

#include "JsonStreamingParser.h"
// Used to parse the Json code within the library
// Available on the library manager (Search for "Json Streamer Parser")
// https://github.com/squix78/json-streaming-parser

//------- Replace the following! ------
char ssid[] = "SSID";       // your network SSID (name)
char password[] = "PASSWORD";  // your network key

WiFiClientSecure client;
RedditApi redditApi(client);

unsigned long delayBetweenChecks = 60000; //mean time between api requests
unsigned long whenDueToCheck = 0;

//Inputs
String subRedditName = "arduino";

void setup() {

  Serial.begin(115200);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
}

void getInfoForSubReddit() {
  Serial.println("Getting info for /r/" + subRedditName );
  SubRedditInfo response = redditApi.getSubRedditInfo(subRedditName);
  Serial.println("Response:");
  Serial.print("Number of subscribers: ");
  Serial.println(response.subscriberCount);
  Serial.print("Title: ");
  Serial.println(response.title);
}

void loop() {
  unsigned long timeNow = millis();
  if ((timeNow > whenDueToCheck))  {
    getInfoForSubReddit();
    whenDueToCheck = timeNow + delayBetweenChecks;
  }
}
