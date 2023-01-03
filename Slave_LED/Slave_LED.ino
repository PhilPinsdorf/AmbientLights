#include <ESP8266WiFi.h>
#include <ArduinoWebsockets.h>

#define SSID "AmbientLights"
#define PASSWORD "0fZkhJv=4!0m4buJ"
#define PORT 80

websockets::WebsocketsClient client;


void setup() {
  // For Debugging
  Serial.begin(115200);

  // Setup Internal LED
  // Attention: LED is low active
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  WiFi.begin(SSID, PASSWORD);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
  Serial.println();

  // Setup Callbacks
  client.onMessage(onMessage);
  client.onEvent(onEvent);
  
  // Connect to server
  client.connect("ws://192.168.4.1:80/");

  // Send a message
  client.send("Connection Successfull");
}

void loop() {
  // put your main code here, to run repeatedly:
  client.poll();
}

void onMessage(websockets::WebsocketsMessage message) {

}

void onEvent(websockets::WebsocketsEvent event, String data) {

}



