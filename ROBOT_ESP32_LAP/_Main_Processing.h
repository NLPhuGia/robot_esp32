// _Main_Processing.h
#include <EEPROM.h>
#include <ArduinoJson.h>


#ifndef __MAIN_PROCESSING_h
#define __MAIN_PROCESSING_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


#endif


#include "_WIFI_ST_ROBOT_CONNECT_SERVER.h"


struct Robot_info {
	String Name = "robot_tranporter";
	String serial = "0123456789";
	int Stage = 0;
	// bien dung cho module wifi
	bool _wifi_module_mode = 0;// 0 chế độ AP , 1 chế độ phát wifi
	String IP_serrver = "192.168.1.196";
	int Porrt_Server = 80;
	String Path = "/ws/robot/" + serial;
	// server
	String SSID_ST = "VanPhong2.4g";
	String Pass_ST = "Vp@1denchin";
	// webserver
	String SSID_AP = "ROBOT_1";
	String Pass_AP = "12345678";



	// arary :sửa

	JsonArray CMD_FROM_MODULE_SERVER;
	//String CMD_FROM_MODULE_SERVER;


	bool _Wifi_st_connected = false;
	// bien  dung cho module RFID reader
	String Home_location = "X1y3Z5z7Y9";
	String _Rfid_location = "0123456789";
	// Bien danh cho motor
	int Motor1_mode = 0;
	int Motor2_mode = 0;
	int PWM_MT_1;
	int PWM_MT_2;
	// Bien danh cho cam bien
	int DISTAN_M1_FW;
	int DISTAN_M1_RW;
	int DISTAN_M2_UP;
	int DISTAN_M2_DW;
};


extern	Robot_info Info;


// remain các giá trị theo tên
// Khai báo chân điều khiển cho adruino
// Khai báo chân của motor 1: motor di chuyển
//#define enable_pwm_motor_1  50 // chân ENA  module CầU H
#define _safety_in1_motor_1 15   // chân IN1  module CầU H
#define _safety_in2_motor_1 14  // chân IN2  module CầU H
#define _pwm_motor_1  32 // chân PWM motor 1

// Công tắc hành trình đâm đụng của robot
// Khai báo chân của motor 2: motor nâng hạ 
//#define enable_pwm_motor_2 51 // chân ENA  module CầU H 
#define _safety_in1_motor_2 13  // chân IN3  module CầU H
#define _safety_in2_motor_2 12  // chân IN4  module CầU H
#define _safety_max_motor_2 5	// Công tắc hành trình nâng hạ của robot
#define _safety_min_motor_2 23  // Công tắc hành trình nâng hạ của robot
#define _done_transfer  34		// Công tắc xác nhận đã nhận hàng từ người dùng
#define _pwm_motor_2  33 // chân PWM motor 2

// Khai báo chân đọc cảm biến khác  _analog
#define _distance_sensor_1 18	   // Cảm biến phía trước to A1
#define _distance_sensor_2 19	  // cảm biến phía sau to A2

// sữ dụng uart 1 để nhận dữ liệu từ RFID MODULE
// Chân GPIO TX = 17      RX =  16
// Sữ dụng I2C SDA = 21 SCL = 22 cho LCD

// Khai báo chân chế độ của robot WIFI AP HAY ST
#define _SW_MODE_WIFI_MODULE 35   

//Khai báo biến toàn cục 

extern String _Data_serial1, _Data_serial2, _Data_serial3;
extern bool _data_complete1, _data_complete2, _data_complete3;

extern _WIFI_ST_ROBOT_CONNECT_SERVERClass server;
//extern ROBOT_UART_PROCESSClass _Robot;
extern int timer_tam;
extern int reconect_server;
extern int _stage_motor_1, _stage_motor_2;
extern int step;

//extern bool _event;

/// <summary>
/// KHAI BÁO CHÂN
/// </summary>
void _int_pin_mode();
/// <summary>
/// HÀM XỬ LÝ TRẠNG THÁI
/// </summary>
void _proccessing_stage(int stage,int _time_text);
/// <summary>
/// HÀM XƯ LÝ LỆNH TỪ SERVER
/// </summary>
/// 
/// 
/// 
/// array cmd :sửa
void _proccessing_CMD(JsonArray cmd);
/// <summary>
/// HÀM XU LÝ THIẾT BỊ
// /// </summary>
void _Processing_device(int trong_so);
// Hàm xữ lý lưu dữ liệu thông tin của robot vào eerom
void _EE_put_info_robot();
// Hàm xữ lý dọc dữ liệu thông tin của robot từ eerom
void _EE_get_info_robot();

void _init_eerom();

void _Load_info(Robot_info _info);

void _Update_info(String data1);

String Read_location();