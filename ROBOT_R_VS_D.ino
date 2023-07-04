#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>

#include <WiFi.h>

const char* ssid = "RCpyahhh"; //Enter SSID
const char* password = "Z50J1TMO"; //Enter Password
const char* websockets_server_host = "192.168.1.196"; //Enter server adress
const uint16_t websockets_server_port = 80; // Enter server portc
using namespace websockets;







void onMessageCallback(WebsocketsMessage message) {
    Serial.print("Got Message: ");
    Serial.println(message.data());
    String json_re = message.data();
    Process_data(json_re);
}




void onEventsCallback(WebsocketsEvent event, String data) {
    if(event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connnection Opened");
    } else if(event == WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connnection Closed");
    } else if(event == WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    } else if(event == WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    }
}

// {"type":"console_update","data":{"id":8765,"host":"192.168.1.192","port":61836,"status":"connected","message":"heloooooo"}}
//  

void Process_data(String json) {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, json);
    const char* type_re = doc["type"];
    String   id  = doc["data"]["id"].as<String>();
    String   host  = doc["data"]["host"];
    String   port  = doc["data"]["port"].as<String>();
    String   status_re  = doc["data"]["status"];
    String   message  = doc["data"]["message"];

    Serial.println(id);
    Serial.println(host);
    Serial.println(port);
    Serial.println(status_re);
    Serial.println(message);

    Serial.println(type_re);

    
}
WebsocketsClient client;




String Json_send() {
  
  StaticJsonDocument<200> doc;
    // Add values in the document
    //
    doc["type"] = "robot_message";
    doc["data"]["message"] = "heloooooo";
    doc["data"]["id"] = "78748738";


    
          
    String requestBody;
       serializeJson(doc, requestBody);
    return requestBody;
}






void setup() {
    Serial.begin(115200);
    // Connect to wifi
    WiFi.begin(ssid, password);

    // Wait some time to connect to wifi
    for(int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
        Serial.print(".");
        delay(1000);
    }

    // run callback when messages are received
    client.onMessage(onMessageCallback);
    
    // run callback when events are occuring
    client.onEvent(onEventsCallback);

    // Connect to server
    client.connect(websockets_server_host, websockets_server_port, "/ws/robot/8765");


    // Send a message
    String msg = Json_send();
    client.send(msg);

    // Send a ping
    client.ping();
}



void loop() {
    client.poll();
}
