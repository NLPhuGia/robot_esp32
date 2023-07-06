// 
// 
// 

#include "_WIFI_ST_ROBOT_CONNECT_SERVER.h"

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClient.h>

WiFiClient client;
WiFiMulti wifiMulti;


void _WIFI_ST_ROBOT_CONNECT_SERVERClass::init()
{
    //WiFi.mode(WIFI_STA);
    //// delete old config
    //WiFi.disconnect(true);
    delay(1000);
    // Examples of different ways to register wifi events
  
    // Remove WiFi event
    wifiMulti.addAP(ssid, password);
 
    delay(2000);
    if (wifiMulti.run() == WL_CONNECTED) {
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
       Serial.println("CHdebug: wifi khong the connect");
    }
    ////WiFi.begin(ssid, password);
  

}

void _WIFI_ST_ROBOT_CONNECT_SERVERClass::WiFiEvent(WiFiEvent_t event)
{
    Serial.printf("[WiFi-event] event: %d\n", event);

   // switch (event) {
   // case ARDUINO_EVENT_WIFI_READY:
   //     Serial.println("WiFi interface ready");
   //     break;
   // case ARDUINO_EVENT_WIFI_SCAN_DONE:
   //     Serial.println("Completed scan for access points");
   //     break;
   // case ARDUINO_EVENT_WIFI_STA_START:
   //     Serial.println("WiFi client started");
   //     break;
   // case ARDUINO_EVENT_WIFI_STA_STOP:
   //     Serial.println("WiFi clients stopped");
   //     break;
   // case ARDUINO_EVENT_WIFI_STA_CONNECTED:
   //     Serial.println("Connected to access point");
   //     break;
   // case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
   //     Serial.println("Disconnected from WiFi access point");
   //     break;
   // case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
   //     Serial.println("Authentication mode of access point has changed");
   //     break;
   // case ARDUINO_EVENT_WIFI_STA_GOT_IP:
   //     Serial.print("Obtained IP address: ");
   //     Serial.println(WiFi.localIP());
   //     break;
   // case ARDUINO_EVENT_WIFI_STA_LOST_IP:
   //     Serial.println("Lost IP address and IP address is reset to 0");
   //     break;
   // case ARDUINO_EVENT_WPS_ER_SUCCESS:
   //     Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
   //     break;
   // case ARDUINO_EVENT_WPS_ER_FAILED:
   //     Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
   //     break;
   // case ARDUINO_EVENT_WPS_ER_TIMEOUT:
   //     Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
   //     break;
   // case ARDUINO_EVENT_WPS_ER_PIN:
   //     Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
   //     break;
   // case ARDUINO_EVENT_WIFI_AP_START:
   //     Serial.println("WiFi access point started");
   //     break;
   // case ARDUINO_EVENT_WIFI_AP_STOP:
   //     Serial.println("WiFi access point  stopped");
   //     break;
   // case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
   //     Serial.println("Client connected");
   //     break;
   // case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
   //     Serial.println("Client disconnected");
   //     break;
   // case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
   //     Serial.println("Assigned IP address to client");
   //     break;
   // case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:
   //     Serial.println("Received probe request");
   //     break;
   // case ARDUINO_EVENT_WIFI_AP_GOT_IP6:
   //     Serial.println("AP IPv6 is preferred");
   //     break;
   // case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
   //     Serial.println("STA IPv6 is preferred");
   //     break;
   // case ARDUINO_EVENT_ETH_GOT_IP6:
   //     Serial.println("Ethernet IPv6 is preferred");
   //     break;
   // case ARDUINO_EVENT_ETH_START:
   //     Serial.println("Ethernet started");
   //     break;
   // case ARDUINO_EVENT_ETH_STOP:
   //     Serial.println("Ethernet stopped");
   //     break;
   // case ARDUINO_EVENT_ETH_CONNECTED:
   //     Serial.println("Ethernet connected");
   //     break;
   // case ARDUINO_EVENT_ETH_DISCONNECTED:
   //     Serial.println("Ethernet disconnected");
   //     break;
   // case ARDUINO_EVENT_ETH_GOT_IP:
   //     Serial.println("Obtained IP address");
   //     break;
   // default: break;
   //} 
}

void _WIFI_ST_ROBOT_CONNECT_SERVERClass::WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
}

void _WIFI_ST_ROBOT_CONNECT_SERVERClass::Update_wifi_SSID(const char* _ssid, const char* _pass)
{
    ssid = _ssid;
    password = _pass;
    Serial.println("CHDEBUG : CAP NHAT THANH CONG SSID VA PASS");
    Serial.println(ssid);
    Serial.println(password);
}

int _WIFI_ST_ROBOT_CONNECT_SERVERClass::Check_connect_server(String _IP, int _Port, String path)
{
    if (wifiMulti.run() == WL_CONNECTED) {
        // thể hiện thông tin đã kết nối wifi
      /*  WiFiEventId_t eventID = WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
            Serial.print("WiFi lost connection. Reason: ");
            Serial.println(info.wifi_sta_disconnected.reason);
        }, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);*/
        // kiểm tra thông tin kết nối server
        int tam = 0;
        host = _IP.c_str();
        port = _Port;
        path = path.c_str();
        Serial.println("CHdebug : ĐỊA CHI IP HOST va port là");
        Serial.println(String(host));
        Serial.println(String(port));
        while (!websocketsClient.connect(host, port, path)) {
            if (tam >= 1) {
                Serial.println("Error-101:connection server failed reset module");
                Flag_connected_server = false;
                return 1;
                
            }
            else {
                Serial.println("connection again");
                Serial.println("wait 1 sec...");
            }
            tam++;
        }
        Serial.println("connected to server"); 
        Flag_connected_server = true;
        return 2;
    }
    Serial.println("WRONG SSID AND PASS plesae config again");
    Flag_connected_server = false;
    return 0;
}




//int _WIFI_ST_ROBOT_CONNECT_SERVERClass::Ack_connect()
//{
//    if (Flag_connected_server == true)
//    {
//        if (wifiMulti.run() == WL_CONNECTED)
//        {
//            Serial.println("Wifi van ket noi");
//           
//            if (websocketsClient.available() == 0)
//            {
//                Serial.println("mat ket noi server " + String(websocketsClient.available()));
//                return 1;
//            }
//            else
//            {
//                Serial.println("van ket noi server " + String(websocketsClient.available()));
//                return 0;
//            }
//        }
//        else
//        {
//            Serial.println("mat ket noi wifi " );
//            return 2;
//        }
//    }
//    return 0;
//}

//void _WIFI_ST_ROBOT_CONNECT_SERVERClass::Check_reciver_data_server()
//{
//    if (Flag_connected_server == true) {
//        if (websocketsClient.available()) {
//           
//                //// Đọc dữ liệu JSON từ server
//                //DeserializationError error = deserializeJson(jsonDoc, _data_reciver);
//                //_data_reciver = jsonDoc.as<String>();
//                //Serial.println("==================\n" + _data_reciver + "\n" + "====================");
//                //if (error) {
//                //    Serial.print("Failed to parse JSON: ");
//                //    Serial.println(error.c_str());
//                //}
//                //else {
//                    /*_data_json = jsonDoc.as<JsonObject>();*/
//            _data_reciver = _data_server;
//            
//                    if (_data_reciver.indexOf("SER:") == 0 && _data_reciver.indexOf(":SER") >= _data_reciver.lastIndexOf("SER:")) {
//                        //nếu dữ liệu đúng tách lấy data gửi về robot
//                        Flag_data_reciver = true;
//                        Serial.println("CHdebug ser send data : " + _data_reciver);
//
//                    }
//                    else if (_data_reciver.indexOf("RFI:") == 0 && _data_reciver.indexOf(":RFI") >= _data_reciver.lastIndexOf("RFI:")) {
//                        //nếu dữ liệu đúng tách lấy data gửi về robot
//                        Flag_data_reciver = true;
//                        Serial.println("CHdebug ser send data : " + _data_reciver);
//
//                    }
//                    else {
//                        // nếu dữ liệu ko đúng xóa dữ liệu
//                        Serial.println("CHdebug dữ liêu sai : " + _data_reciver);
//                        _data_reciver = ""; Flag_data_reciver = false;
//                    }
//                //}
//            
//        }
//    }
//}





    int _WIFI_ST_ROBOT_CONNECT_SERVERClass::Process_data_reciver_server(int stage, String data)
{
    // check data from module wifi if data start == "SER:" lenh nay do server gui den
        DynamicJsonDocument json(1024);

        // Parse chuỗi JSON
        DeserializationError error = deserializeJson(json, data);
        if (error) {
            Serial.print("Failed to parse JSON_reciver: ");
            Serial.println(error.c_str());
            return 0;
        }

    if (json["type"]=="command") {
        //SER:GOTO:1111111111,GET:,GOTO:2222222222,TAKE::SER\n
        //SER:GET::SER\n "GET:" + json["GET"].as<String>() + + "TAKE:" + json["TAKE"].as<String>()

        JsonArray commandArray = json["data"]["command"];
        String a = json["data"]["command"].as<String>();
        //commandArray = _cmd_server;
        Serial.println("BUGGGGGG:" + a);
        //commandArray = _cmd_server;
        Serial.println(commandArray);
        _cmd_server = commandArray;
        length_cmd = sizeof(commandArray);

        Serial.println("CO GOI PROCESS");
        Serial.println("CHDebug:xu ly du lieu " + _cmd_server);
        return 1;


        // String tam = "GOTO:"+ json["data"]["GOTO"].as<String>() + "GOTO:" + json["GOTO2"].as<String>();
        //        Serial.println("CO GOI PROCESS");
        //_cmd_server = tam;
        //Serial.println("CHDebug:xu ly du lieu " + _cmd_server);
        //return 1;












        ////----------- chuyen du lieu nhan duoc  sang array -----------------------
        //// Convert String to array
        //char* tamArray = strdup(tam.c_str()); // Create a duplicate of the tam string

        //// Tokenize the tamArray using strtok
        //char* token = strtok(tamArray, "GOTO:");
        //while (token != NULL) {
        //    // Process each token here (add to array, print, etc.)
        //    printf("Token: %s\n", token);

        //    token = strtok(NULL, "GOTO:");
        //}

        //// Free the memory allocated for tamArray
        //free(tamArray);

        ////-----------------------------------------------------------------


     }

    else if (json["type"] == "update") {
        _data_reciver = json["data"].as<String>();
      //  Serial.println("-----Den ham cap nhat du lieu-----");
        return 2;
    }
    else {
        return 0;
    }
}

void _WIFI_ST_ROBOT_CONNECT_SERVERClass::check_sender_data_server()
{
    if (Flag_data_sender == true) {
        Flag_data_sender = false;
        if (_data_sender != "") {
            bool check;
           check = websocketsClient.send(_data_sender);
            _data_sender = "";
            if (check == true)
            {
                Serial.println("CHdebug :server");
            }
            else
            {
                Serial.println("CHdebug :server khong nhan");
            }
        }
        else
        {
            Serial.println("CHdebug : Lỗi dữ liệu sender null");
        }
    }
}



_WIFI_ST_ROBOT_CONNECT_SERVERClass _WIFI_ST_ROBOT_CONNECT_SERVER;

