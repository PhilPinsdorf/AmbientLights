// Master

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoWebsockets.h>

#define SSID "AmbientLights"
#define PASSWORD "0fZkhJv=4!0m4buJ"
#define PORT 80

enum LedAnimation {
  LED_ERROR,
  LED_SUCCESSFULL
};

// There are a maximum of 8 connections to the ESP
const int maxClients = 8;
websockets::WebsocketsClient clients[maxClients];
websockets::WebsocketsServer server;


/*
  Description:
  Main Setup
*/
void setup() {
  // For Debugging
  Serial.begin(115200);

  // Setup Internal LED
  // Attention: LED is low active
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // Set ESP in Access Point mode
  WiFi.mode(WIFI_AP); 

  // Setup WiFi
  if(WiFi.softAP(SSID, PASSWORD, 1, true, maxClients)) { 
    Serial.println("WiFi Access-Point created.");
  } else {
    internalError("WiFi Access-Point could not be created.");
  }

  // Start Websocket Server
  server.listen(PORT);
  if(server.available()) {
    Serial.println("WebSocket Server is alive.");
  } else {
    internalError("WebSocket Server failed.");
  }

  playLedAnimation(LED_SUCCESSFULL);
}


/*
  Description:
  Main Loop
*/
void loop() {
  // Update all Clients and listen for new ones.
  pollClients();
  listenForClients();
}


/*
  Description: 
  Method gets called when Error occurs.
  Method stops Programm whit blinking. 
*/
void internalError(String errorMsg) {
  // Print Error Message
  Serial.println("Error: " + errorMsg);

  // Stop Programm and blink
  while(true) {
    playLedAnimation(LED_ERROR);
  }
}


/*
  Description:
  Method Plays Animation on onboard LED regarding the Enum Passed as Parameter.  
*/
void playLedAnimation(LedAnimation ledAnimation) {
  switch(ledAnimation) {
    case LED_ERROR:
      // Pulsate like a Heartbeat

      for(int i = 0; i < 2; i++) {
        digitalWrite(LED_BUILTIN, LOW);
        delay(100);  
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);  
      }

      delay(400);
      break;
    
    case LED_SUCCESSFULL:
      // Blink 3 times then stay active

      for(int i = 0; i < 3; i++) {
        digitalWrite(LED_BUILTIN, LOW);
        delay(100);  
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);  
      }

      digitalWrite(LED_BUILTIN, LOW);
      break;
  }
}


/*
  Description:
  Check if Server is alive and Clients want to connect.
  Check if there is Space for a Client to connect (ESP allows a maximum of 8 Clients at a time).
  Accept new incoming connection request, setup callbacks and save client in clients array for future use. 
*/
void listenForClients() {
  // Return if Server is no availible
  if(!server.available()) return;

  // Check if there are new clients, that want to connect 
  if (!server.poll()) return; 

  // Check for free space to connect
  short freeIndex = getFreeClientIndex();
  if (freeIndex == -1) return;

  // Accept new client, setup callbacks and save in clients array
  websockets::WebsocketsClient newClient = server.accept();
  Serial.println("New client connected! Saved in Index " + String(freeIndex) + ".");
  newClient.onMessage(handleMessage);
  newClient.onEvent(handleEvent);
  clients[freeIndex] = newClient;
}


/*
  Description:
  Update all clients, that are saved in clients.
*/
void pollClients() {
  for (byte i = 0; i < maxClients; i++) {
    clients[i].poll();
  }
}

/*
  Description:
  Find free Index in clients array
*/
short getFreeClientIndex() {
  // If a client in our list is not available, it's connection is closed and we can use it for a new client. 
  // .available() also returns false if there is no client saved at the specified array position.
  for (byte i = 0; i < maxClients; i++) {
    if (!clients[i].available()) return i;
  }

  return -1;
}

void handleMessage(websockets::WebsocketsClient &client, websockets::WebsocketsMessage message) {
  if(!message.isText()) return;

  // Log message
  Serial.print("Got Message: ");
  Serial.println(message.data());
}

void handleEvent(websockets::WebsocketsClient &client, websockets::WebsocketsEvent event, String data) {

}



