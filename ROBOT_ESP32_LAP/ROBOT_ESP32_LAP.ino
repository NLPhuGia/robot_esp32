/*
 Name:		ROBOT_ESP32_LAP.ino
 Created:	6/23/2023 1:44:35 PM
 Author:	Dell M4800
*/
#include "_WIFI_ST_ROBOT_CONNECT_SERVER.h"
//#include "ROBOT_UART_PROCESS.h"
#include "_Main_Processing.h"
// the setup function runs once when you press reset or power the board
#include <EEPROM.h>


// the setup function runs once when you press reset or power the board

WebsocketsClient websocketsClient;


// interrupt timer 
hw_timer_t* timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;



//void onEventsCallback(WebsocketsEvent event, String data) {
//	if (event == WebsocketsEvent::ConnectionOpened) {
//		Serial.println("Connnection Opened");
//	}
//	else if (event == WebsocketsEvent::ConnectionClosed) {
//		Serial.println("Connnection Closed");
//	}
//	else if (event == WebsocketsEvent::GotPing) {
//		Serial.println("Got a Ping!");
//	}
//	else if (event == WebsocketsEvent::GotPong) {
//		Serial.println("Got a Pong!");
//	}
//}
//
//void onMessageCallback(WebsocketsMessage message) {
//	Serial.print("Got Message: ");
//	Serial.println(message.data());
//}



void onMessageCallback(WebsocketsMessage message)
{
	Serial.println("======================================");
	Serial.print("Got Message: ");
	Serial.println(message.data());
	server._data_reciver = message.data();
	DeserializationError error = deserializeJson(server.jsonDoc, message.data());

	if (error) {
		Serial.print("Failed to parse JSON: ");
		Serial.println(error.c_str());
	}
	/*Serial.println("===================\n" + _data_reciver + "\n" + "===================");*/
	if (server.jsonDoc.containsKey("type")) {
		//nếu dữ liệu đúng tách lấy data gửi về robot
		server.Flag_data_reciver = true;
		Serial.println("CHdebug ser send data : " + server._data_reciver);
	}
	else {
		// nếu dữ liệu ko đúng xóa dữ liệu
		Serial.println("CHdebug dữ liêu sai : " + server._data_reciver);
		server._data_reciver = ""; server.Flag_data_reciver = false;
	}

}



void onEventsCallback(WebsocketsEvent event, String data)
{
	
	if (event == WebsocketsEvent::ConnectionOpened) {
		Serial.println("Connnection Opened");
	}
	else if (event == WebsocketsEvent::ConnectionClosed) {
		if (wifiMulti.run() == WL_CONNECTED)
		{
			Serial.println("WIFI VẪN CÒN KÊT NỐI");
			Serial.println("Connnection SERVER Closed");
			Info.Stage = 0; server.Flag_connected_server = false;
		}
		else
		{
			Serial.println("WIFI MẤT KÊT NỐI");
			delay(2000);
			ESP.restart();
		}
	}
	else if (event == WebsocketsEvent::GotPing) {
		Serial.println("Got a Ping!");
	}
	else if (event == WebsocketsEvent::GotPong) {
		Serial.println("Got a Pong!");
	}
	
}


// Mỗi 10m2 sẽ tự động tạo 1 ngắt timer để tạo clock timer tam .
void IRAM_ATTR onTime() {
	portENTER_CRITICAL_ISR(&timerMux);
	timer_tam = timer_tam + 1;
	//Serial.println("CHDEBUG: timer tam = " + String(timer_tam));
	portEXIT_CRITICAL_ISR(&timerMux);
}




// the setup function runs once when you press reset or power the board
void setup() {
	// cấu hình chế độ của robot
	_int_pin_mode();
	// Configure Prescaler to 80, as our timer runs @ 80Mhz
	// Giving an output of 80,000,000 / 80 = 1,000,000 ticks / second

	timer = timerBegin(0, 80, true);
	timerAttachInterrupt(timer, &onTime, true);
	// Fire Interrupt every 1m ticks, so 1s
	timerAlarmWrite(timer, 50000, true);
	timerAlarmEnable(timer);

	websocketsClient.connect(Info.IP_serrver, Info.Porrt_Server, Info.Path);

	// run callback when messages are received
	websocketsClient.onMessage(onMessageCallback);

	// run callback when events are occuring
	websocketsClient.onEvent(onEventsCallback);

	// Send a message
	/*websocketsClient.send("Client connect to Server");*/
}

// the loop function runs over and over again until power down or reset
void loop() {
  websocketsClient.poll();
	// hàm xữ lý trạng thái của robot

    _proccessing_stage(Info.Stage, 100);

	// Goi ham xu ly UART tu module wifi 
	/*if (_Robot._Data_reciver_complete == true) {
		_Robot._Data_reciver_complete = false;
		_Robot._processing_wifi_module(Info.Stage, _Robot._Data_reciver_module_wifi);
		_Robot._Data_reciver_module_wifi = "";
	}*/
	// Goi ham xu ly data từ server 
	if (server.Flag_data_reciver == true) {
		server.Flag_data_reciver = false;
		Serial.println("CHDEBUG: Du Lieu Nhan Dc = " + server._data_reciver);
		// Ham xu ly lenh nhan tu server
		switch (server.Process_data_reciver_server(Info.Stage, server._data_reciver))
		{
		// có lệnh gửi từ server
		case 1:Info.CMD_FROM_MODULE_SERVER = server._cmd_server;
			break;
		// lệnh force connected 
		case 2: _Update_info(server._data_reciver); break;
		default:
			break;
		}
		server._data_reciver = ""; // thực hiện xong xóa dữ liệu đã xử lý 
	}
	 //Xữ lý yêu cầu từ server
	_proccessing_CMD(Info.CMD_FROM_MODULE_SERVER);     

	// xữ lý input và out put
	if (timer_tam % 10 == 0) {		
		//Serial.println("CHDEBUG: processing motor ");
		_Processing_device(8);
		timer_tam++;		
	}

	/* kiểm tra server co gửi dữ liệu về không*/
	//server.Check_reciver_data_server();

	// kiểm tra robot co gửi dữ liệu về server không
	server.check_sender_data_server();

	//if (Info.Stage != 0) {
	//	//Serial.println("tam tam tam");
	//	if (timer_tam % 10 == 0) {
	//		Serial.println("CHDEBUG: bat dau kiem tra lai ket noi 1");
	//		if (reconect_server >= 3) {
	//			Serial.println("CHDEBUG: bat dau kiem tra lai ket noi 2");
	//			reconect_server = 0;
	//			int check = server.Ack_connect();
	//			if (check == 1)// mat ket noi server
	//			{
	//				Info.Stage = 0; server.Flag_connected_server = false;
	//				_proccessing_CMD("STOP:"); // dung dong co
	//			}
	//			else if (check == 2) // mat ket noi wifi khoi dong lai robot
	//			{
	//				ESP.restart();  /*ESP restart function*/

	//			}
	//			timer_tam++;
	//		}
	//		else
	//			reconect_server = reconect_server + 1;
	//	}
	//	
	//}
	

}


void serialEvent()
{

//	_Robot._serialEvent();
}
void serialEvent1()
{
//	_Robot._serialEvent1();
}
void serialEvent2() //  ĐỌC UART LOCATION
{
	Serial.println("CHDEBUG: COS NGAT LOCATION");
	//Info._Rfid_location = Read_location();
	//Serial.println(Info._Rfid_location);
//	_Robot._serialEvent2();
}

//void serialEvent3()
//{
//
//	_Robot._serialEvent3();
//}
//
