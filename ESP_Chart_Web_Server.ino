/**
 * @file ESP_Chart_Web_Server.ino
  * @brief This is an example of a web server that serves a chart with real-time
data from a DHT22 sensor.
  * It uses the ESPAsyncWebServer library to create a web server and the
  * WebSocketsServer library to create a WebSocket server for real-time
  * communication between the server and the client.
  * The web server serves an HTML page that contains a chart created with the
  * Chart.js library. The chart displays the temperature and humidity data from
the
  * DHT22 sensor in real-time. The data is sent to the client using WebSockets,
  * and the chart is updated every second with the latest readings from the
sensor.
  * The web server also allows the client to control the state of three LEDs
  * (red, green, and yellow) using WebSockets. The client can send a message to
  * the server to turn on or off the LEDs, and the server will respond by
  * updating the state of the LEDs accordingly.
  * This code is designed to run on an ESP32 or ESP8266 microcontroller and
  * requires the ESPAsyncWebServer, WebSocketsServer, DHT, and ArduinoJson

  * libraries to be installed in the Arduino IDE.
  * The code is structured to handle WebSocket events, read sensor data,
  * update the chart, and control the LEDs. It also includes functions to send
  * JSON data to the client and handle WebSocket events.
  * The code is organized into functions for better readability and
maintainability.
  *
    * This code is a good example of how to create a web server with real-time
    * data visualization and control using WebSockets and the ESPAsyncWebServer
    * library.
    * This code is a good starting point for creating IoT applications that
    * require real-time data visualization and control. It can be extended to
    * include more sensors, more complex data processing, and more advanced
    * features such as user authentication, data logging, and remote control.
  *
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-08-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

// Include necessary libraries
// These libraries are required for the ESP32/ESP8266 to connect to Wi-Fi,
// serve web pages, handle WebSocket communication, and read sensor data.

#ifdef ESP32
  #include <WiFi.h>
  #include <ESPAsyncWebServer.h>
  #include <LittleFS.h>
#else
  #include <Arduino.h>
  #include <ESP8266WiFi.h>
  #include <Hash.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h> 
  #include <LittleFS.h>
  #include <FS.h>
#endif
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <WebSocketsServer.h>                         // needed for instant communication between client and server through Websockets
#include <ArduinoJson.h>

//------------- Sensor definition ---------------
// Include the necessary libraries for the DHT sensor
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// Define the DHT sensor type and pin
// The DHT sensor type can be DHT11, DHT22, or AM2302.
#define DHTPIN     3      // Digital pin connected to the DHT sensor 
#define DHTTYPE    DHT22 // DHT 22 (AM2302), change to DHT11, if you using DHT11.

// LED staue controlling pins
// These pins are used to control the state of the LEDs.
// The red LED is connected to pin 17, the green LED is connected to pin 18,
// and the yellow LED is connected to pin 8. These pins are defined as constants  
#define RED_LED       17    // To control red led
#define GREEN_LED     18     // To control green led
#define YELLOW_LED    8    // To control yellow led

// define dht senser reading variable.
// DHT sensor object
// DHT22 is also known as AM2302, AM2321, DHT22
// Create an instance of the DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// Replace with your network credentials
const char* ssid     = "VM1080293";
const char* password = "Omidmuhsin2015";

// number of the connected clients
// This variable is used to keep track of the number of connected clients.
// It is initialized to 0 and will be incremented or decremented when a client
// connects or disconnects. This is used to manage the number of clients
// connected to the WebSocket server.

static int clients  = 0; 


// REPLACE with your Domain name and URL path or IP address with path
const char* serverName = "https://example.com/post-esp-data.php";
// Keep this API Key value to be compatible with the PHP code provided in the project page. 
// If you change the apiKeyValue value, the PHP file /post-esp-data.php also needs to have the same key 
String apiKeyValue = "tPmAT5Ab3j7F9";
String sensorName = "DHT22";
String sensorLocation = "Office";

// web server listening on port 80 and websocket listening on port 81.
// The web server will serve the HTML page and the websocket will handle real-time communication.
// create an instance of the web server on port 80    
AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);    // the websocket uses port 81 (standard port for websockets

// array to store sensor readings
// We will store the last 10 readings of temperature and humidity in arrays.
// This allows us to keep track of the last 10 readings and send them to the
// client
// when requested. The arrays are defined with a fixed length of 10.
// This means that we can store up to 10 readings in each array.    
const int ARRAY_LENGTH = 10;
float temperature_vals[ARRAY_LENGTH];
float temperature_vals2[ARRAY_LENGTH];
float humidity_vals[ARRAY_LENGTH];

// We want to periodically send values to the clients, so we need to define an "interval" and remember the last time we sent data to the client (with "previousMillis")
int interval = 1000;                                  // send data to the client every 1000ms -> 1s
unsigned long previousMillis = 0;                /**
 * @brief This function is used to send a JSON string to the web clients.
 * It creates a JSON object with the type and value parameters,
 * serializes it to a string, and then broadcasts it to all connected clients.    
 * @param l_type  a type of the data to be sent, e.g. "temperature", "humidity", etc. 
 * @param l_value   a value of the data to be sent, e.g. "25.5", "60", etc.
 */
void sendJson(String l_type, String l_value) {
    String jsonString = "";                           // create a JSON string for sending data to the client
    StaticJsonDocument<200> doc;                      // create JSON container
    JsonObject object = doc.to<JsonObject>();         // create a JSON Object
    object["type"] = l_type;                          // write data into the JSON object
    object["value"] = l_value;
    serializeJson(doc, jsonString);                   // convert JSON object to string
    webSocket.broadcastTXT(jsonString);               // send JSON string to all clients
}

// Simple function to send information to the web clients (arrays)
/**
 * @brief It creates a JSON object with the type and value parameters,serializes it to
  a string, and then broadcasts it to all connected clients.
 *
 * @param l_type  a type of the data to be sent, e.g. "temperature", "humidity",
etc.
 * @param l_array_values an array of float values to be sent, e.g. temperature
or humidity values.

 * @param updatedValue    a value of the data to be sent, e.g. "25.5", "60", etc.
 * This value is used to update the chart on the client side.
 **/
void sendJsonArray(String l_type, float l_array_values[], float updatedValue)
 {
    String jsonString = "";                           // create a JSON string for sending data to the client
    const size_t CAPACITY = JSON_ARRAY_SIZE(ARRAY_LENGTH) + 100;
    StaticJsonDocument<CAPACITY> doc;                      // create JSON container    
    JsonObject object = doc.to<JsonObject>();         // create a JSON Object

    object["type"] = l_type;  
    object["updatedVal"] = updatedValue;                          // write data into the JSON object
    JsonArray value = object.createNestedArray("value");
    for(int i=0; i<ARRAY_LENGTH; i++) {
      value.add(l_array_values[i]);
    }
    serializeJson(doc, jsonString);                   // convert JSON object to string
    webSocket.broadcastTXT(jsonString);               // send JSON string to all clients
}

/**
 * @brief   This function is called once at the beginning of the program.
  * It initializes the serial port, sets up the LEDs, mounts the LittleFS file system,
  * initializes the DHT sensor, connects to Wi-Fi, and sets up the web server and WebSocket server.
  * It also serves the HTML page and starts listening for WebSocket events.
 */
void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);  
   
   // leds initilaization 
  pinMode(GREEN_LED,OUTPUT);
  pinMode(RED_LED,OUTPUT);
  pinMode(YELLOW_LED,OUTPUT);
  // turning off leds 
  digitalWrite(GREEN_LED,LOW);
  digitalWrite(RED_LED,LOW);
  digitalWrite(YELLOW_LED,LOW);

  // Initialize LittleFS
  // LittleFS is a file system for storing files on the ESP32/ESP8266

  if(!LittleFS.begin())
  {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  // --------- setup sensor ------------
  // Initialize DHT sensor

  dht.begin();
  // Connect to Wi-Fi
  // The ESP32/ESP8266 will connect to the Wi-Fi network specified by the ssid and password variables.
  WiFi.begin(ssid, password);
  // Wait for the connection to be established
  // The ESP32/ESP8266 will wait until it is connected to the Wi-Fi
  // network before proceeding. This is done in a loop that checks the Wi-Fi
  // status every second. If the connection is not established, it will print

  
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  // After connecting to the Wi-Fi network, the ESP32/ESP8266 will print its local IP address to the serial monitor.
  Serial.println(WiFi.localIP());

  // Route for root / web page
  // The web server will serve the HTML page when the root URL is accessed.
  // The HTML page is stored in the LittleFS file system and will be served when
  // the root URL is accessed. The HTML page is named "webpage2.html" and is
  // stored in the root
  // directory of the LittleFS file system.
  // The web server will listen for HTTP GET requests on the root URL and will
  // respond by sending the "webpage2.html" file from the LittleFS file system.
  Serial.println("HTTP server started");
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/webpage2.html");
  });
  
  server.serveStatic("/",LittleFS, "/");  
  webSocket.begin();                                 
  // start websocket
  webSocket.onEvent(webSocketEvent);
  // Start server
  // The web server will start listening for incoming HTTP requests on port 80.
  server.begin();
}

/**
 * @brief  This function is called repeatedly in the main loop.

  * It updates the WebSocket server, checks if the interval has passed,
  * reads the DHT sensor data, and sends the updated temperature and humidity
values
  * to the connected clients. It also shifts the values in the arrays to make
  * room for the new readings.
  * 
  */
void loop() 
{ 
  webSocket.loop();   // Update function for the webSockets  
  unsigned long now = millis();   // read out the current "time" ("millis()" gives the time in ms since the Arduino started)
  if ((unsigned long)(now - previousMillis) > interval) 
    { // check if "interval" ms has passed since last time the clients were updated
        Serial.println("Updating clients..."); // print to the serial monitor that the clients are being updated
        // if the interval has passed, then we can update the clients
        // we can read the DHT sensor data and send it to the clients
        // we can also shift the values in the arrays to make room for the new
        // readings
        
        previousMillis = now;                             // reset previousMillis
        for(int i=0; i < ARRAY_LENGTH - 1; i++) 
        {
          temperature_vals[i] = temperature_vals[i+1];
          temperature_vals2[i] = temperature_vals2[i+1];
          humidity_vals[i]    = humidity_vals[i+1];
        }

      // Print temperature sensor details.
      
      // Read temperature as Fahrenheit (isFahrenheit = true)
      float h = dht.readHumidity();
      // Read temperature as Celsius (the default)
      float t = dht.readTemperature();
      // Read temperature as Fahrenheit (isFahrenheit = true)
      float f = dht.readTemperature(true);

      // Check if any reading failed and exit early (to try again).
      // If the reading failed, we will print an error message to the serial
      // monitor and return from the function. This is to avoid sending invalid
      // data to the clients.
      
      if (isnan(h) || isnan(t) || isnan(f)) 
      {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
      }

      Serial.print(F("Humidity: "));
      Serial.print(h);      
      Serial.print("% ");

      Serial.print("Temperature: ");
      Serial.print(t);
      Serial.print(F("C"));

      Serial.print("Temperature: ");
      Serial.print(f);
      Serial.println(F("F "));
      
      temperature_vals[ARRAY_LENGTH - 1]  = t;
      temperature_vals2[ARRAY_LENGTH - 1] = f;
      humidity_vals[ARRAY_LENGTH - 1]     = h;
      // send data to the connected clients.
      // We will send the updated temperature and humidity values to the
      // clients. The sendJsonArray function will create a JSON object with the
      // type and
      // value parameters, serialize it to a string, and then broadcast it to
      // all connected clients.

      sendJsonArray("temperature_update_c", temperature_vals,t);
      sendJsonArray("temperature_update_f", temperature_vals2,f);      
      sendJsonArray("humidity_update", humidity_vals,h);
    } 
     //  startTrafficLight(); // start the traffic light sequence
    // Uncomment the line below to start the traffic light sequence.
    //startTrafficLight();
    // delay(2000);
}


/**
 * @brief This function is called when a WebSocket event occurs.
 * It handles the connection, disconnection, and text messages from the clients.
 * It checks the type of event and performs the appropriate action.
 *
 * @param num  a number representing the client who sent the event
 * @param type  the type of the event, e.g. WStype_DISCONNECTED,
                WStype_CONNECTED, WStype_TEXT,...
 * @param payload   a pointer to the payload data sent by the client
 * @param length    the length of the payload data.
 */

void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length) 
{      // the parameters of this callback function are always the same -> num: id of the client who send the event, type: type of message, payload: actual data sent and length: length of payload
  switch (type) {
    // This switch statement checks the type of the event and performs the
    // appropriate action. The type can be WStype_DISCONNECTED,
    // WStype_CONNECTED, or WStype_TEXT. WStype_DISCONNECTED: the client has
    // disconnected from the server.
    // WStype_CONNECTED: the client has connected to the server.
    // WStype_TEXT: the client has sent a text message to the server.
    
    case WStype_DISCONNECTED:   
    {                      // if a client is disconnected, then type == WStype_DISCONNECTED
      Serial.println("Client " + String(num) + " disconnected");
      clients--;
    }
    break;

    case WStype_CONNECTED:     
    {                       // if a client is connected, then type == WStype_CONNECTED
      Serial.println("Client " + String(num) + " connected");   
      clients++;  
    }
    break;

    case WStype_TEXT:  
    {                               // if a client has sent data, then type == WStype_TEXT
      // try to decipher the JSON string received
      StaticJsonDocument<200> doc;                    // create JSON container 
      DeserializationError error = deserializeJson(doc, payload);
      if (error) {
         Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      else {
        // JSON string was received correctly, so information can be retrieved
        // do here what needs to be done when server receive data from clients.
        // the required actions are recieved from the clients, then do as
        // needed.
        // The JSON object contains an "action" field that indicates what action
        // to perform. The action can be 1, 2, 3, 4, 5, or 6.
        // 1: turn on green led
        // 2: turn off green led
        // 3: turn on red led
        // 4: turn off red led
        // 5: turn on yellow led
        // 6: turn off yellow led
        // The action is sent as a JSON object with the key "action" and the
        // corresponding value. The server will read the action and perform the
        // corresponding action on the LEDs.
        int led_action = doc["action"];
        Serial.println("Request received from ESP32 " + String(led_action));         
        switch(led_action) 
        {
          case 1:
            digitalWrite(GREEN_LED,HIGH); // turn the green led on.
            break;
          case 2: 
            digitalWrite(GREEN_LED,LOW); // turn the green led off.
            break;
          case 3:
            digitalWrite(RED_LED,HIGH); // turn the red led on.
            break;
          case 4:
            digitalWrite(RED_LED,LOW); // turn the red led off.
            break;
          case 5:
            digitalWrite(YELLOW_LED,HIGH); // turn the yellow led off.
            break;
          case 6:
            digitalWrite(YELLOW_LED,LOW); // turn the yellow led off.
            break;
          default: 
            Serial.println("Unknown Action, try again .... "); 
            break;
        }
      } 
    } break;
  }
}
/**
 * @brief This function is used to start the traffic light sequence.
  * It will turn on the green led for 5 seconds, then turn it off,
  * turn on the yellow led for 2 seconds, then turn it off,
  * and finally turn on the red led for 5 seconds, then turn it off.
  * 
 */
void startTrafficLight()
{

    digitalWrite(GREEN_LED,HIGH); // turn the green led on.
    delay(5000); //wait 5 seconds 
    digitalWrite(GREEN_LED,LOW);    // turn the green led off.
    digitalWrite(YELLOW_LED,HIGH); // turn the yellow led on.
    delay(2000);  // wait 2 seconds.
    digitalWrite(YELLOW_LED,LOW); // turn the yellow led off.
    digitalWrite(RED_LED,HIGH); // turn the red led on.
    delay(5000); // wait 5 seconds. 
    digitalWrite(RED_LED,LOW); // turn the red led off.
}
