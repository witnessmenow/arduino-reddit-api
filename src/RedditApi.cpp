/*

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Written By Brian Lough
https://www.youtube.com/user/witnessmenow
https://github.com/witnessmenow
https://twitter.com/witnessmenow

*/

#include "RedditApi.h"

String currentKey = "";
String currentParent = "";
SubRedditInfo subRedditInfoResponse;

//**************************************************************************//
// This code is the JSON Parser code written by squix78 as part of his example,
// modified for this application //
// https://github.com/squix78/json-streaming-parser //

class SubRedditInfoListener : public JsonListener {
 public:
  virtual void whitespace(char c);

  virtual void startDocument();

  virtual void key(String key);

  virtual void value(String value);

  virtual void endArray();

  virtual void endObject();

  virtual void endDocument();

  virtual void startArray();

  virtual void startObject();
};

void SubRedditInfoListener::whitespace(char c) {
  // Serial.println("whitespace");
}

void SubRedditInfoListener::startDocument() {
  // Serial.println("start document");
}

void SubRedditInfoListener::key(String key) {
  currentKey = key;
}

void SubRedditInfoListener::value(String value) {
  if (currentKey == "subscribers") {
      subRedditInfoResponse.subscriberCount = value.toInt();
  } else if (currentKey == "title") {
      subRedditInfoResponse.title = value;
  }
}

void SubRedditInfoListener::endArray() {
  // Serial.println("end array. ");
}

void SubRedditInfoListener::endObject() {
  currentParent = "";
  // Serial.println("end object. ");
}

void SubRedditInfoListener::endDocument() {
  // Serial.println("end document. ");
}

void SubRedditInfoListener::startArray() {
  // Serial.println("start array. ");
}

void SubRedditInfoListener::startObject() {
  currentParent = currentKey;
  // Serial.println("start object. ");
}
//*********** END of Json Parser code **************//


RedditApi::RedditApi(Client& client) {
  this->client = &client;
}

SubRedditInfo RedditApi::getSubRedditInfo(String subReddit) {
  if (_debug) Serial.println("starting getUserStats function");
  JsonStreamingParser parser;
  SubRedditInfoListener listener;
  currentKey = "";
  currentParent = "";
  subRedditInfoResponse = SubRedditInfo();
  parser.setListener(&listener);

  long now;
  // Connect to Instagram over ssl

  // It's failing to connect ever second time on version 2.3 of ESP8266, so lets try this knonsense
  if (client->connect(REDDIT_HOST, portNumber) || client->connect(REDDIT_HOST, portNumber)) {
    if (_debug) Serial.println(".... connected to server");

    client->println("GET /r/" + subReddit + "/about HTTP/1.1");
    client->print("Host:"); client->println(REDDIT_HOST);
    client->println("User-Agent: arduino/1.0");
    client->println();

    now = millis();
    while (millis() - now < 3000) {
      while (client->available()) {
        char c = client->read();

        if (_debug) Serial.print(c);

        // parsing code:
        // most of the work happens in the header code
        // at the top of this file
        parser.parse(c);

        // This is the check to see if you have everything you connected
        // as the library is expanded to include more info this will connected
        // to change.
        if (subRedditInfoResponse.subscriberCount > 0 && subRedditInfoResponse.title != "") {
          if (_debug) Serial.println("finished");
          closeClient();
          return subRedditInfoResponse;
        }
      }
    }
  }
  closeClient();
  return subRedditInfoResponse;
}

void RedditApi::closeClient() {
  if(client->connected()){
    client->stop();
  }
}
