// _WIFI_ST_ROBOT_CONNECT_SERVER.h


#ifndef __WIFI_ST_ROBOT_CONNECT_SERVER_h
#define __WIFI_ST_ROBOT_CONNECT_SERVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
	#include <WiFi.h>
	#include <WiFiMulti.h>
	#include <WiFiClient.h>
	#include <ArduinoWebsockets.h>
	#include<ArduinoJson.h>
#else
	#include "WProgram.h"
#endif

extern WiFiClient client;
extern WiFiMulti wifiMulti;
using namespace websockets;
extern WebsocketsClient websocketsClient;


class _WIFI_ST_ROBOT_CONNECT_SERVERClass
{
 protected:
 private:
	 
	


 public:
	/// <summary>
	/// Khai báo chế độ wifi station kết dùng kết nối SOFT WARE SERVER
	/// </summary>
	void init();
	/// <summary>
	/// hàm kiểm tra sự kiện của module wifi station 
	/// </summary>
	/// <param name="event"> đây là biến sự kiện </param>
	void WiFiEvent(WiFiEvent_t event); 
	/// <summary>
	/// 
	/// Lấy thông tin info ip của robot khi kết nối vào mạng lan
	/*
		* WiFi Events

		0  ARDUINO_EVENT_WIFI_READY               < ESP32 WiFi ready
		1  ARDUINO_EVENT_WIFI_SCAN_DONE                < ESP32 finish scanning AP
		2  ARDUINO_EVENT_WIFI_STA_START                < ESP32 station start
		3  ARDUINO_EVENT_WIFI_STA_STOP                 < ESP32 station stop
		4  ARDUINO_EVENT_WIFI_STA_CONNECTED            < ESP32 station connected to AP
		5  ARDUINO_EVENT_WIFI_STA_DISCONNECTED         < ESP32 station disconnected from AP
		6  ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE      < the auth mode of AP connected by ESP32 station changed
		7  ARDUINO_EVENT_WIFI_STA_GOT_IP               < ESP32 station got IP from connected AP
		8  ARDUINO_EVENT_WIFI_STA_LOST_IP              < ESP32 station lost IP and the IP is reset to 0
		9  ARDUINO_EVENT_WPS_ER_SUCCESS       < ESP32 station wps succeeds in enrollee mode
		10 ARDUINO_EVENT_WPS_ER_FAILED        < ESP32 station wps fails in enrollee mode
		11 ARDUINO_EVENT_WPS_ER_TIMEOUT       < ESP32 station wps timeout in enrollee mode
		12 ARDUINO_EVENT_WPS_ER_PIN           < ESP32 station wps pin code in enrollee mode
		13 ARDUINO_EVENT_WIFI_AP_START                 < ESP32 soft-AP start
		14 ARDUINO_EVENT_WIFI_AP_STOP                  < ESP32 soft-AP stop
		15 ARDUINO_EVENT_WIFI_AP_STACONNECTED          < a station connected to ESP32 soft-AP
		16 ARDUINO_EVENT_WIFI_AP_STADISCONNECTED       < a station disconnected from ESP32 soft-AP
		17 ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED         < ESP32 soft-AP assign an IP to a connected station
		18 ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED        < Receive probe request packet in soft-AP interface
		19 ARDUINO_EVENT_WIFI_AP_GOT_IP6               < ESP32 ap interface v6IP addr is preferred
		19 ARDUINO_EVENT_WIFI_STA_GOT_IP6              < ESP32 station interface v6IP addr is preferred
		20 ARDUINO_EVENT_ETH_START                < ESP32 ethernet start
		21 ARDUINO_EVENT_ETH_STOP                 < ESP32 ethernet stop
		22 ARDUINO_EVENT_ETH_CONNECTED            < ESP32 ethernet phy link up
		23 ARDUINO_EVENT_ETH_DISCONNECTED         < ESP32 ethernet phy link down
		24 ARDUINO_EVENT_ETH_GOT_IP               < ESP32 ethernet got IP from connected AP
		19 ARDUINO_EVENT_ETH_GOT_IP6              < ESP32 ethernet interface v6IP addr is preferred
		25 ARDUINO_EVENT_MAX
		*/
	/// </summary>
	/// <param name="event"> đưa sự kiện vào </param>
	/// <param name="info"> lấy thông tin ra</param>
	void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
	/// <summary>
	/// Hàm cập nhật ssid vào class
	/// </summary>
	/// <param name="_ssid">tên mạng wifi</param>
	/// <param name="_pass">mật khẩu của mạng wifi</param>
	void Update_wifi_SSID(const char *_ssid,const char *_pass);
	int  Check_connect_server(String _IP, int _Port, String path);

	//int Ack_connect();
		
	//void Check_reciver_data_server();

	int Process_data_reciver_server(int stage, String data);

	void check_sender_data_server();

	const char* ssid = "VanPhong2.4g";
	const char* password = "Vp@1denchin";
	const char* host;
	int port;


	bool Flag_data_reciver;
	bool Flag_data_sender;
	bool Flag_connected_server;
	bool Flag_Cmd_server;

	//JsonObject _data_json;
	StaticJsonDocument<1024> jsonDoc;
	StaticJsonDocument<1024> json;

	//array
	//String _cmd_server;
	//char* _cmd_server;
	JsonArray _cmd_server;
	String _data_reciver;
	String _data_sender;
	int length_cmd;
};

extern _WIFI_ST_ROBOT_CONNECT_SERVERClass _WIFI_ST_ROBOT_CONNECT_SERVER;

#endif
