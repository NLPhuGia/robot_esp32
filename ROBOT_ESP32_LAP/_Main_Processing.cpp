// 
// 
// 

//#include "_data.h"
#include "_Main_Processing.h"
#include <EEPROM.h>

 //ROBOT_UART_PROCESSClass _Robot;
 _WIFI_ST_ROBOT_CONNECT_SERVERClass server;

 Robot_info Info;
 EEPROMClass  _PORT_SERVER("eeprom1");
 EEPROMClass  _IP_SERVER("eeprom2");
 EEPROMClass  _PASS_ST("eeprom0");
 EEPROMClass  _SSID_ST("eeprom1");
 EEPROMClass  _PASS_AP("eeprom2");
 EEPROMClass  _SSID_AP("eeprom0");
 EEPROMClass  _SERIAL_NO("eeprom1");
 EEPROMClass  _NAME("eeprom2");


 //EEPROMClass  _PORT_SERVER("eeprom1", 0x800);
 //EEPROMClass  _IP_SERVER("eeprom2", 0x700);
 //EEPROMClass  _PASS_ST("eeprom0", 0x600);
 //EEPROMClass  _SSID_ST("eeprom1", 0x500);
 //EEPROMClass  _PASS_AP("eeprom2", 0x400);
 //EEPROMClass  _SSID_AP("eeprom0", 0x300);
 //EEPROMClass  _SERIAL_NO("eeprom1", 0x200);
 //EEPROMClass  _NAME("eeprom2", 0x100);

  int timer_tam; // biến clock thời gian cua robot
  int reconect_server; // biến lập lại reconnect
  int _stage_motor_1, _stage_motor_2; // biến trạng thái hoạt động của motor 1 và 2
  int step = 0 ;
  String _Data_serial1, _Data_serial2, _Data_serial3;
/// <summary>
/// hàm khai báo chân cho robot
/// </summary>
void _int_pin_mode()
{
	analogReadResolution(9);
	// khai báo uart
	Serial.begin(115200); // khởi chạy uart 0 cho debug robot
	//Serial1.begin(115200);// khởi chạy uart 1 truyền nhận dữ liệu từ module wifi
	Serial2.begin(115200);// khởi chạy uart 2 truyền nhận dữ liệu từ module rfid
	//Serial3.begin(115200);// khởi chạy uart 2 truyền nhận dữ liệu từ module rfid
	//khai báo trạng thái
	Info.Stage = 0;
	//khai báo chân input va out put
	pinMode(_SW_MODE_WIFI_MODULE, INPUT);
	pinMode(_safety_max_motor_2, INPUT);
	pinMode(_safety_min_motor_2, INPUT);
	pinMode(_done_transfer, INPUT);
	
	pinMode(_safety_in1_motor_1, OUTPUT);
	pinMode(_safety_in1_motor_2, OUTPUT);
	pinMode(_safety_in2_motor_1, OUTPUT);
	pinMode(_safety_in2_motor_2, OUTPUT);
	// khai báo chân pwm
	ledcSetup(0, 5000, 8);
	ledcSetup(1, 5000, 8);
	ledcAttachPin(_pwm_motor_1, 0);
	ledcAttachPin(_pwm_motor_2, 1);
	// khởi động robot 
	_init_eerom();
	// get dữ liệu từ eerom
	

	//// chọn chế độ kết nối server hay chế độ kế nối webserver
	if (digitalRead(_SW_MODE_WIFI_MODULE) == false)	// chế độ server
	{
		Serial.println("CH DEBUG: bắt đầu kết nối vào mạng wifi ");
		// 1 : cập nhật tên ssid và pass để kết nối 
		server.Update_wifi_SSID(Info.SSID_ST.c_str(), Info.Pass_ST.c_str());
		

		// 2 : Khởi tạo kết nối .
		server.init();
		
		// 3 : Conect to wifi và server lần đầu
		if (server.Check_connect_server(Info.IP_serrver, Info.Porrt_Server, Info.Path) == 2){
			Info._Wifi_st_connected = true;
			Info.Stage = 1;
		}
		else Info._Wifi_st_connected = false;
	}
	else											// chế độ webserver
	{

	}
	//
	delay(1000);
	Serial.println("BAT DAU CHAY"); timer_tam = 0;
	

}
/// <summary>
/// hàm xu lý trạng thái của robot
/// </summary>
/// <param name="stage"> 
/// trạng thái 0: cho ket noi server hoặc trạng thái webserver local
/// trạng thái 1: đã kết nối server
/// trạng thái 2: đang thực thi chuỗi lệnh từ server gửi đến .
/// 
/// </param>
void _proccessing_stage(int stage,int _time_text)
{
	
	bool _input_tam = false;
	int i = 0;
	// kiểm tra sw wifi module mode
	while (i > 10)
	{
		_input_tam = digitalRead(_SW_MODE_WIFI_MODULE);
		i++;
		
	}
	
	switch (stage)
	{
	case 0:
		//Serial.println(timer_tam);
		// kiem tra cong tac gac mode
		// SW dang o che do web serrver 
		if (timer_tam > _time_text && _input_tam == true) {
			timer_tam = 0;
			Serial.println("CHUONG DEBUG STAGE 0 : mode webserver");

		}
		// SW dang o che do serrver 
		else if (timer_tam > _time_text && _input_tam == false) {
			Serial.println("CHUONG DEBUG STAGE 0:  Stage 0 mode server");
			timer_tam = 0;
			// Kiem tra ke noi cua module wifi
			if (server.Flag_connected_server == true) {
				// neu wifi da ket noi vao server thif sex chuyen trang thai
				Info.Stage = 1;
				Serial.println("CHUONG DEBUG STAGE 0: module wifi da connect to server");
			}
			else if (server.Flag_connected_server == false){
				delay(1);
				if (reconect_server >= 3) {
					reconect_server = 0;
					server.Check_connect_server(Info.IP_serrver, Info.Porrt_Server, Info.Path);
				}
				else
					reconect_server = reconect_server + 1;
			}
		}// SW dang o che do server st
		break;
	case 1:
		//Serial.println("CHUONG DEBUG: STAGE 1");
		if (timer_tam >= _time_text)
			timer_tam = 0;
		else if (timer_tam % 10 == 0){
			Serial.println("CHUONG DEBUG STAGE 1: update robot Status");
			_Load_info(Info);
			timer_tam++;
			// update gia trị của robot
		}
			// kiểm tra có lệnh từ server gửi đến không

		// if (Info.CMD_FROM_MODULE_SERVER == ""){		
	    if (Info.CMD_FROM_MODULE_SERVER.isNull()) {
			step = 0;
			/// nếu không có lệnh > kiểm tra vị trí robot khong nằm ở home location 



			if (Info._Rfid_location != Info.Home_location) {
				// Serial.println("CHUONG DEBUG STAGE 1: robot khong o home tiền hành chạy robot ve home");
				// tiến hành nạp dữ liệu vào CMD để robot di chuyển đến home location


				Info.CMD_FROM_MODULE_SERVER[0] = "GOTO:" + Info.Home_location + " ";
				Info.Stage = 2;
			}
		}
		// kiểm tra có lệnh từ server gửi đến 
		else{
			Serial.println("CHUONG DEBUG STAGE 1: tien hanh thực hiện lệnh từ server");
			Info.Stage = 2;
		}
		break;
	case 2:

		if (timer_tam >= _time_text)
			timer_tam = 0;
		else if (timer_tam % 10 == 0) {
			delay(1);
			Serial.println("CHUONG DEBUG STAGE 2: update robot Status");
			_Load_info(Info);
			timer_tam++;
			// update gia trị của robot
		}
		// update gia trị của robot
		else {
			// xữ lý lệnh từ 
			if (!Info.CMD_FROM_MODULE_SERVER.isNull())
			{
				// gọi hãmu lý lệnh
				_proccessing_CMD(Info.CMD_FROM_MODULE_SERVER);
			}
			else
			{
				step = 0;
				Info.Stage = 1;
			}
		}
		/// tien hanh xu ly lenh cua dong co
		
		break;
		default:
			Serial.println("CHUONG DEBUG default");
			delay(100);
	}
}
/// <summary>
/// hàm thực thi chủi lệnh từ server mỗi khi thực thi xong sẽ xóa bớt chủi lệnh lại.
/// </summary>
/// <param name="cmd">
/// Lệnh GOTO:
/// LỆNH GET:
/// LỆNH TAKE:
/// </param>
/// 
/// 
/// array cmd :sửa
/// 
/// 
void _proccessing_CMD(JsonArray cmd) {
	//cmd =  CMD_FROM_MODULE_SERVER
	if (step < server.length_cmd) {
		//String cmd = Info.CMD_FROM_MODULE_SERVER;
	/*	Serial.println("BUGGG" + cmd[step].as<String>());*/
		if (cmd[step].as<String>().indexOf("GOTO:") == 0)
		{
			Serial.println("BUGGG:" + cmd[step].as<String>());

			// Cho motor chạy thẳng
			if (Info.Motor1_mode == 0)
			{
				Info.Motor1_mode = 1;
			}
			// neu motor ddang chay lui thi giam toc sau do se chayj thang
			else if (Info.Motor1_mode != 1)
			{
				// giảm tốc độ động cơ
				while (true)
				{
					if (Info.PWM_MT_1 > 10)
					{
						Info.PWM_MT_1--; ledcWrite(0, Info.PWM_MT_1);
						delay(50);

					}
					else
					{
						Info.PWM_MT_1 = 0;
						Info.Motor1_mode = 1;
						break;
					}
				}
			}
			// kiem tra vi tri robot da dan diem yeu cau thi tien hanh dung dong co va tru lenh CMD
			Serial.println("GOTO:" + Info._Rfid_location);
			Serial.println(cmd);
			//Serial.println(cmd.indexOf("GOTO:" + Info._Rfid_location));


			if (cmd[step].as<String>().indexOf("GOTO:" + String(Info._Rfid_location)) == 0)
			{
				Serial.println("da den vi tri location:" + Info._Rfid_location);
				while (true)
				{
					if (Info.PWM_MT_1 > 10)
					{
						Info.PWM_MT_1--; ledcWrite(0, Info.PWM_MT_1);
						delay(5);

					}
					else
					{
						Info.PWM_MT_1 = 0;
						Info.Motor1_mode = 0;
						//cmd.substring(cmd.indexOf("GOTO:") + 4);
						String _tam = "GOTO:" + Info._Rfid_location;
						step++;
						break;
						//	return cmd;
					}
				}
				Serial.println("CHUONG DEBUG : " + Info.CMD_FROM_MODULE_SERVER);
			}
		}
		else if (cmd[step].as<String>().indexOf("GET:") == 0) {
			//Serial.println("CHUONG DEBUG : xu ly nag ha");
			//Serial.println("CHUONG DEBUG : " + String(_stage_motor_2));
			if (_stage_motor_2 == 0) {
				//  motor đang dừng
				if (Info.Motor2_mode == 0) {
					if (digitalRead(_safety_max_motor_2) == false)
						Info.Motor2_mode = 1;// motor 2 chạy chế độ dw
					else
					{
						Serial.println("CHUONG DEBUG : Loi mo tor nang ha");
					}
				}
				// neu motor dang chay leen thi giam toc sau do se chay xuong
				else if (Info.Motor2_mode != 1) {
					// giảm tốc độ động cơ
					while (true)
					{
						if (Info.PWM_MT_2 > 10) {
							Info.PWM_MT_2--; ledcWrite(1, Info.PWM_MT_1);
							delay(5);

						}
						else {
							Info.PWM_MT_2 = 0;
							Info.Motor2_mode = 1;
							break;
						}
					}
				}
				// kiem tra vi tri thang da chay xuong thi tien hanh dung lai chuyen trang thai 2
				_stage_motor_2 = 1;
			}
			else if (_stage_motor_2 == 1) {

				if (digitalRead(_safety_max_motor_2) == true)
				{

					while (true)
					{
						if (Info.PWM_MT_2 > 10)
						{
							Info.PWM_MT_2--; ledcWrite(1, Info.PWM_MT_2);
							delay(5);

						}
						else
						{
							Info.PWM_MT_2 = 0;
							Info.Motor2_mode = 0;
							ledcWrite(1, Info.PWM_MT_2);
							//cmd.substring(cmd.indexOf("GOTO:") + 4);
							_stage_motor_2 = 2;
							step++;
							break;
							//	return cmd;
						}
					}
					Serial.println("CHUONG DEBUG : da tha thang ");
				}

			}
			else if (_stage_motor_2 == 2) {
				if (digitalRead(_done_transfer) == true) {
					Info.Motor2_mode = 2;
					_stage_motor_2 = 3;
					Serial.println("CHUONG DEBUG : xac nhan done keo thang cho hang len  ");
				}
			}

			else if (_stage_motor_2 == 3) {

				// thang cho hang da keo len
				if (digitalRead(_safety_min_motor_2) == true) {
					//dung dong co
					Info.Motor2_mode = 0;
					_stage_motor_2 == 0;
					while (true) {
						if (Info.PWM_MT_2 > 10)
						{
							Info.PWM_MT_2 -= 8; ledcWrite(1, Info.PWM_MT_2);
							delay(5);

						}
						else
						{


							Info.PWM_MT_2 = 0;
							Info.Motor2_mode = 0;
							ledcWrite(1, Info.PWM_MT_2);
							if (cmd[step].as<String>().indexOf("GET:") == 0) {
								Serial.println("CHUONG DEBUG : da keo thang len  ");
								/*cmd = cmd.substring(cmd.indexOf("GET:") + 4, cmd.length());
								Info.CMD_FROM_MODULE_SERVER = cmd;*/
								Serial.println("CHUONG DEBUG : " + Info.CMD_FROM_MODULE_SERVER);
								step++;
							}
							_stage_motor_2 = 0;
							break;
							//	return cmd;
						}
					}

				}
			}
		}
		else if (cmd[step].as<String>().indexOf("TAKE:") == 0) {
			//Serial.println("CHUONG DEBUG : xu ly nag ha");
			//Serial.println("CHUONG DEBUG : " + String(_stage_motor_2));
			if (_stage_motor_2 == 0) {
				//  motor đang dừng
				if (Info.Motor2_mode == 0) {
					if (digitalRead(_safety_max_motor_2) == false)
						Info.Motor2_mode = 1;// motor 2 chạy chế độ dw
					else
					{
						Serial.println("CHUONG DEBUG : Loi mo tor nang ha");
					}
				}
				// neu motor dang chay leen thi giam toc sau do se chay xuong
				else if (Info.Motor2_mode != 1) {
					// giảm tốc độ động cơ
					while (true)
					{
						if (Info.PWM_MT_2 > 10) {
							Info.PWM_MT_2--; ledcWrite(1, Info.PWM_MT_1);
							delay(5);

						}
						else {
							Info.PWM_MT_2 = 0;
							Info.Motor2_mode = 1;
							break;
						}
					}
				}
				// kiem tra vi tri thang da chay xuong thi tien hanh dung lai chuyen trang thai 2
				_stage_motor_2 = 1;
			}
			else if (_stage_motor_2 == 1) {

				if (digitalRead(_safety_max_motor_2) == true)
				{

					while (true)
					{
						if (Info.PWM_MT_2 > 10)
						{
							Info.PWM_MT_2--; ledcWrite(1, Info.PWM_MT_2);
							delay(5);

						}
						else
						{
							Info.PWM_MT_2 = 0;
							Info.Motor2_mode = 0;
							ledcWrite(1, Info.PWM_MT_2);
							//cmd.substring(cmd.indexOf("GOTO:") + 4);

							_stage_motor_2 = 2;

							break;
							//	return cmd;
						}
					}
					Serial.println("CHUONG DEBUG : da tha thang ");
				}

			}
			else if (_stage_motor_2 == 2) {
				if (digitalRead(_done_transfer) == true) {
					Info.Motor2_mode = 2;
					_stage_motor_2 = 3;
					Serial.println("CHUONG DEBUG : xac nhan done keo thang cho hang len  ");
				}
			}

			else if (_stage_motor_2 == 3) {

				// thang cho hang da keo len
				if (digitalRead(_safety_min_motor_2) == true) {
					//dung dong co
					Info.Motor2_mode = 0;
					_stage_motor_2 == 0;
					while (true) {
						if (Info.PWM_MT_2 > 10)
						{
							Info.PWM_MT_2 -= 8; ledcWrite(1, Info.PWM_MT_2);
							delay(5);

						}
						else
						{


							Info.PWM_MT_2 = 0;
							Info.Motor2_mode = 0;
							ledcWrite(1, Info.PWM_MT_2);
							if (cmd[step].as<String>().indexOf("TAKE:") == 0) {
								Serial.println("CHUONG DEBUG : da keo thang len  ");
								/*cmd = cmd.substring(cmd.indexOf("TAKE:") + 5, cmd.length());
								Info.CMD_FROM_MODULE_SERVER = cmd;*/
								step++;
								Serial.println("CHUONG DEBUG : " + Info.CMD_FROM_MODULE_SERVER);
							}
							_stage_motor_2 = 0;
							break;
							//	return cmd;
						}
					}

				}
			}
		}
		else if (cmd[step].as<String>().indexOf("FW:") == 0) {
			if (Info.Motor1_mode == 0)
			{
				Info.Motor1_mode = 1;
			}
			// neu motor ddang chay lui thi giam toc sau do se chayj thang
			else if (Info.Motor1_mode != 1)
			{
				// giảm tốc độ động cơ
				while (true)
				{
					if (Info.PWM_MT_1 > 10)
					{
						Info.PWM_MT_1--; ledcWrite(0, Info.PWM_MT_1);
						delay(50);

					}
					else
					{
						Info.PWM_MT_1 = 0;
						Info.Motor1_mode = 1;
						break;
					}
				}
			}
		}
		else if (cmd[step].as<String>().indexOf("RW:") == 0) {
			if (Info.Motor1_mode == 0)
			{
				Info.Motor1_mode = 2;
			}
			// neu motor ddang chay lui thi giam toc sau do se chayj thang
			else if (Info.Motor1_mode != 2)
			{
				// giảm tốc độ động cơ
				while (true)
				{
					if (Info.PWM_MT_1 > 10)
					{
						Info.PWM_MT_1--; ledcWrite(0, Info.PWM_MT_1);
						delay(50);

					}
					else
					{
						Info.PWM_MT_1 = 0;
						Info.Motor1_mode = 2;
						break;
					}
				}
			}
		}
		else if (cmd[step].as<String>().indexOf("STOP:") == 0) {
			if (Info.Motor1_mode == 0) {
				Info.Motor1_mode = 0;
			}
			// neu motor ddang chay lui thi giam toc sau do se chayj thang
			else if (Info.Motor1_mode != 0) {
				// giảm tốc độ động cơ
				while (true)
				{
					if (Info.PWM_MT_1 > 10)
					{
						Info.PWM_MT_1 -= 8; ledcWrite(0, Info.PWM_MT_1);
						delay(50);

					}
					else
					{
						Info.PWM_MT_1 = 0;
						Info.Motor1_mode = 0;
						break;
					}
				}

			}
			_stage_motor_1 = 0;
			if (Info.Motor2_mode == 0) {
				Info.Motor2_mode = 0;
			}
			// neu motor ddang chay lui thi giam toc sau do se chayj thang
			else if (Info.Motor2_mode != 0) {
				// giảm tốc độ động cơ
				while (true)
				{
					if (Info.PWM_MT_2 > 10)
					{
						Info.PWM_MT_2 -= 8; ledcWrite(1, Info.PWM_MT_2);
						delay(50);

					}
					else
					{
						Info.PWM_MT_2 = 0;
						Info.Motor2_mode = 0;
						break;
					}
				}
			}
			_stage_motor_2 = 0;
		}
		else if (cmd[step].as<String>().indexOf("CLEAR:") == 0) {
			if (Info.Motor1_mode != 0 && Info.PWM_MT_1 != 0)
			{
				// giảm tốc độ động cơ
				while (true)
				{
					if (Info.PWM_MT_1 > 10)
					{
						Info.PWM_MT_1--; ledcWrite(0, Info.PWM_MT_1);
						delay(50);

					}
					else
					{
						Info.PWM_MT_1 = 0;
						Info.Motor1_mode = 0;
						break;
					}
				}

			}
			else
			{
				Info.PWM_MT_1 = 0;
				Info.Motor1_mode = 0;
				cmd.clear();
				step++;   ///// xem lai   !!!!!
				Info.CMD_FROM_MODULE_SERVER = cmd;
			}
			_stage_motor_1 = 0; _stage_motor_2 = 0;

		}
		else
		{
			step = 0;
			_stage_motor_1 = 0;
			_stage_motor_2 = 0;
			cmd.clear();
			Info.CMD_FROM_MODULE_SERVER = cmd;
		}
	}
	//return cmd;
}

/// <summary>///




/// hàm xữ lý dọc dữ liệu các input và output .
/// </summary>/// <param name="trong_so"></param>
void _Processing_device(int trong_so){
	
	// read input 
	float _distance_FW = 0, _distance_RW=0, _distance_DW, _distance_UP;
	//float _distance_FW = 0, _distance_RW = 0;

	float _xval, _yval;
	_distance_FW = analogRead(_distance_sensor_1);
	_distance_RW = analogRead(_distance_sensor_2);
	//	case GP2Y0A02YK0F : mã cảm biến 

	//distance = 9462 / (analogRead(pin) - 16.92);
	//Serial.println("gia tri do duoc = : " + String(_distance_FW));
	_distance_FW = 9462 / (_distance_FW - 16.92);
	_distance_FW = 250;     // Gán để test
	if(_distance_FW > 150)	_distance_FW = 151;	else if(_distance_FW < 20)	_distance_FW = 19;

	_distance_RW = 9462 / (_distance_RW - 16.92);
	_distance_RW = 200;   // // Gán để test
	if (_distance_RW > 150)	_distance_RW = 151;	else if (_distance_RW < 20)	_distance_RW = 19;
	Serial.println("Khoang cach truoc : " + String(_distance_FW));
	Serial.println("Khoang cach sau : " + String(_distance_RW));
	Serial.println("CHE DO DONG CO 1 : " + String(Info.Motor1_mode));


	//xu ly dong co di chuyen
	if (Info.Motor1_mode == 1)
	{
		digitalWrite(_safety_in1_motor_1, HIGH);
		digitalWrite(_safety_in2_motor_1, LOW);
		_xval = _distance_FW;

		if (_xval < 40)// neu khoan cách <=> 40 cm thì giam tốc
		{

			_yval = 0;
			if (Info.PWM_MT_1 < 30)//neu tốc độ còn 30 thì tốc độ bằng 0
			{
				Info.PWM_MT_1 = 0;
			}
			else//neu tốc độ > 30 thì tốc độ giam di 3xtrong số
			{
				Info.PWM_MT_1 -= (trong_so * 3);
			}
		}
		else // neu khoan cách > 40 cm thì tăng tốc
		{
			_yval = (_xval / 150) * 255;// Tính giá trị tốc độ cực đại từ khoản cách
			//Serial.println("GIA TRI Y  : " + String(_yval));
			//Serial.println("GIA TRI PWM  : " + String(Info.PWM_MT_1));
			if (_yval > Info.PWM_MT_1)//neu tốc độ cực đại > tăng tốc độ hiện tại thi tăng tốc thêm bằng trọng số
			{
				if (Info.PWM_MT_1 < 240)
					Info.PWM_MT_1 += trong_so;
				else
					Info.PWM_MT_1 = 250;
			}
			else
			{
				if (Info.PWM_MT_1 < 50)
					Info.PWM_MT_1 = 0;
				else
					Info.PWM_MT_1 -= (trong_so * 3);

			}

		}
		//_result_PWM 
		ledcWrite(0, Info.PWM_MT_1);
		//Serial.println("toc do dong co fw  : " + String(_yval));
	//	Serial.println("toc do dong co fw  : " + String(Info.PWM_MT_1));

	}
	else if (Info.Motor1_mode == 2)
	{
		digitalWrite(_safety_in1_motor_1, LOW);
		digitalWrite(_safety_in2_motor_1, HIGH);
		_xval = _distance_FW;

		if (_xval < 40)// neu khoan cách <=> 40 cm thì giam tốc
		{

			_yval = 0;
			if (Info.PWM_MT_1 < 30)//neu tốc độ còn 30 thì tốc độ bằng 0
			{
				Info.PWM_MT_1 = 0;
			}
			else//neu tốc độ > 30 thì tốc độ giam di 3xtrong số
			{
				Info.PWM_MT_1 -= (trong_so * 3);
			}
		}
		else // neu khoan cách > 40 cm thì tăng tốc
		{
			_yval = (_xval / 225) * 255;// Tính giá trị tốc độ cực đại từ khoản cách
			//Serial.println("GIA TRI Y  : " + String(_yval));
			//Serial.println("GIA TRI PWM  : " + String(Info.PWM_MT_1));
			if (_yval > Info.PWM_MT_1)//neu tốc độ cực đại > tăng tốc độ hiện tại thi tăng tốc thêm bằng trọng số
			{
				if (Info.PWM_MT_1 < 240)
					Info.PWM_MT_1 += trong_so;
				else
					Info.PWM_MT_1 = 250;
			}
			else
			{
				if (Info.PWM_MT_1 < 50)
					Info.PWM_MT_1 = 0;
				else
					Info.PWM_MT_1 -= (trong_so * 3);

			}

		}
		//_result_PWM 
		ledcWrite(0, Info.PWM_MT_1);
		//Serial.println("toc do dong co fw  : " + String(_yval));
	//	Serial.println("toc do dong co rw  : " + String(Info.PWM_MT_1));
	}
	else
	{
		digitalWrite(_safety_in1_motor_1, LOW);
		digitalWrite(_safety_in2_motor_1, LOW);
		Info.PWM_MT_1 = 0;
		ledcWrite(0, Info.PWM_MT_1);
	}
	
	// xu ly motor 2 nâng hạ ở đây
	if (Info.Motor2_mode != 0)
	{
		if (Info.Motor2_mode == 1) {
			digitalWrite(_safety_in1_motor_2, HIGH);
			digitalWrite(_safety_in2_motor_2, LOW);
		}
		else if (Info.Motor2_mode == 2)
		{
			digitalWrite(_safety_in1_motor_2, LOW);
			digitalWrite(_safety_in2_motor_2, HIGH);
		}
		else
		{
			digitalWrite(_safety_in1_motor_2, LOW);
			digitalWrite(_safety_in2_motor_2, LOW);
		}
		if (Info.PWM_MT_2 < 250)//neu tốc độ be hon 150
		{
			Info.PWM_MT_2 = Info.PWM_MT_2 + trong_so;
		}
		else
		{
			Info.PWM_MT_2 = Info.PWM_MT_2 - trong_so;
		}
		//_result_PWM 
		ledcWrite(1, Info.PWM_MT_2);
		//Serial.println("toc do dong co nag ha  : " + String(Info.PWM_MT_2));
	//	Serial.println("toc do dong co nang ha  : " + String(Info.PWM_MT_2));

	}
	else {
		digitalWrite(_safety_in1_motor_2, LOW);
		digitalWrite(_safety_in2_motor_2, LOW);
		Info.PWM_MT_2 = 0;
		//	Serial.println("toc do dong co nag ha  : " + String(Info.PWM_MT_2));
	}

}

void _EE_put_info_robot()
{

}

void _EE_get_info_robot()
{

}

void _init_eerom()
{
	bool retart_tam = false;
	Serial.println("Testing EEPROMClass\n");
	if (!_NAME.begin(0x100)) {
		Serial.println("Failed to initialise NAMES");
		retart_tam = true;
	}
	if (!_SERIAL_NO.begin(0x200)) {
		Serial.println("Failed to initialise SERIAL_NO");
		retart_tam = true;
	}
	if (!_SSID_AP.begin(0x300)) {
		Serial.println("Failed to initialise SSID_AP");
		retart_tam = true;
	}
	if (!_PASS_AP.begin(0x300)) {
		Serial.println("Failed to initialise PASS_AP");
		retart_tam = true;
	}
	if (!_SSID_ST.begin(0x400)) {
		Serial.println("Failed to initialise SSID_ST");
		retart_tam = true;
	}
	if (!_PASS_ST.begin(0x500)) {
		Serial.println("Failed to initialise PASS_ST");
		retart_tam = true;
	}
	if (!_PASS_ST.begin(0x600)) {
		Serial.println("Failed to initialise PASS_ST");
		retart_tam = true;
	}
	if (!_IP_SERVER.begin(0x700)) {
		Serial.println("Failed to initialise IP_SERVER");
		retart_tam = true;
	}
	if (!_PORT_SERVER.begin(0x800)) {
		Serial.println("Failed to initialise PORT_SERVER");
		retart_tam = true;
	}
	if (retart_tam == true)
	{
		delay(1000);
		ESP.restart();
	}
	else
	{
		Serial.println("CHdebug : success init eerom");
	}
}

//void _Load_info(Robot_info _info)
//{
//
//	server._data_sender = "CLI:";
//	server._data_sender += "1:"; server._data_sender += _info.Name; server._data_sender += " ";
//	server._data_sender += "2:"; server._data_sender += _info.serial; server._data_sender += " ";
//	server._data_sender += "3:"; server._data_sender += String(_info.Stage); server._data_sender += " ";
//	if (_info.CMD_FROM_MODULE_SERVER == "")
//	{
//		server._data_sender += "4:"; server._data_sender += "NULL"; server._data_sender += " ";
//	}
//	else
//	{
//		server._data_sender += "4:"; server._data_sender += _info.CMD_FROM_MODULE_SERVER; server._data_sender += " ";
//	}
//	server._data_sender += "5:"; server._data_sender += _info.Home_location; server._data_sender += " ";
//	server._data_sender += "6:"; server._data_sender += _info._Rfid_location; server._data_sender += " ";
//	server._data_sender += "7:"; server._data_sender += String(_info.Motor1_mode); server._data_sender += " ";
//	server._data_sender += "8:"; server._data_sender += String(_info.Motor2_mode); server._data_sender += " ";
//	server._data_sender += ":CLI";
//	Serial.println(server._data_sender);
//	server.Flag_data_sender = true;
//	// server._data_sender = _Data_send_module_wifi; server.Flag_data_sender = true;
//	//Serial3.println(_Data_send_module_wifi);
//	// gửi dữ liệu về server ở đây
//
//}



void _Load_info(Robot_info _info)
{	

	//if (_event == true){
		//_event = false;
   ////---------------------------

		StaticJsonDocument<1024> json;
		json["type"] = "robot_message";
		json["data"]["name"] = _info.Name;
		json["data"]["serial"] = _info.serial;
		json["data"]["stage"]  = _info.Stage;
			if (!_info.CMD_FROM_MODULE_SERVER.isNull())
		{
			json["data"]["cmd_from_module_server"] = _info.CMD_FROM_MODULE_SERVER;
		}

		json["data"]["home_location"] = _info.Home_location;
		json["data"]["rfid_location"] = _info._Rfid_location;
		json["data"]["motor1_mode"] = String(_info.Motor1_mode);
		json["data"]["motor2_mode"] = String(_info.Motor2_mode);

		serializeJson(json, server._data_sender);

		Serial.println(server._data_sender);
		server.Flag_data_sender = true;





/////------------------------------------------
		// server._data_sender = _Data_send_module_wifi; server.Flag_data_sender = true;
		//Serial3.println(_Data_send_module_wifi);
		// gửi dữ liệu về server ở đây
	//}
}

// void _Update_info(JsonDocument data, Robot_info _info) {
	
// 	// Kiểm tra và cập nhật các thông tin từ JSON vào class Info
// 	if (data.containsKey("Name")) {
// 		const char* Name = data["Name"];
// 		_info.Name = Name;
// 		//Serial.println(Info.Name.c_str());
// 	}
// 	if (data.containsKey("serial")) {
// 		const char* serial = data["serial"];
// 		_info.serial = serial;
// 	}
// 	if (data.containsKey("Home_location")) {
// 		const char* Home_location = data["Home_location"];
// 		_info.Home_location = Home_location;
// 	}
// 	if (data.containsKey("IP_serrver")) {
// 		const char* IP_serrver = data["IP_serrver"];
// 		_info.IP_serrver = IP_serrver;
// 	}
// 	if (data.containsKey("Porrt_Server")) {
// 		const char* Porrt_Server = data["Porrt_Server"];
// 		_info.Porrt_Server = Porrt_Server;
// 	}
// 	if (data.containsKey("SSID_ST")) {
// 		const char* SSID_ST = data["SSID_ST"];
// 		_info.SSID_ST = SSID_ST;
// 	}
// 	if (data.containsKey("Pass_ST")) {
// 		const char* Pass_ST = data["Pass_ST"];
// 		_info.Pass_ST = Pass_ST;
// 	}
// 	if (data.containsKey("SSID_AP")) {
// 		const char* SSID_AP = data["SSID_AP"];
// 		_info.SSID_AP = SSID_AP;
// 	}
// 	if (data.containsKey("Pass_AP")) {
// 		const char* Pass_AP = data["Pass_AP"];
// 		_info.Pass_AP = Pass_AP;
// 	}
// }

void _Update_info(String data1)
{

	// SER: UP_ROBOT:1, Name:, serial,Home_location,IP_server,Porrt_Server,SSID_ST,Pass_ST,SSID_AP,Pass_AP
	 // Định nghĩa bộ đệm để lưu trữ dữ liệu từ JSON
	DynamicJsonDocument data(256);

	// Parse chuỗi JSON
	// Phân tích cú pháp JSON và đọc dữ liệu vào bộ đệm
	DeserializationError error = deserializeJson(data, data1);

	// Kiểm tra lỗi trong quá trình parse
	if (error) {
		Serial.print("Parsing failed: ");
		Serial.println(error.c_str());
		return;
	}

	if (data.containsKey("Name")) {
		const char* Name = data["Name"];
		Info.Name = Name;
		Serial.println(Info.Name.c_str());
	}
	if (data.containsKey("serial")) {
		const char* serial = data["serial"];
		Info.serial = serial;
	}
	if (data.containsKey("Home_location")) {
		const char* Home_location = data["Home_location"];
		Info.Home_location = Home_location;
	}
	if (data.containsKey("IP_serrver")) {
		const char* IP_serrver = data["IP_serrver"];
		Info.IP_serrver = IP_serrver;
	}
	/*if (data.containsKey("Porrt_Server")) {
		const char* Porrt_Server = data["Porrt_Server"];
		Info.Porrt_Server = Porrt_Server.;
	}*/
	if (data.containsKey("SSID_ST")) {
		const char* SSID_ST = data["SSID_ST"];
		Info.SSID_ST = SSID_ST;
	}
	if (data.containsKey("Pass_ST")) {
		const char* Pass_ST = data["Pass_ST"];
		Info.Pass_ST = Pass_ST;
	}
	if (data.containsKey("SSID_AP")) {
		const char* SSID_AP = data["SSID_AP"];
		Info.SSID_AP = SSID_AP;
	}
	if (data.containsKey("Pass_AP")) {
		const char* Pass_AP = data["Pass_AP"];
		Info.Pass_AP = Pass_AP;
	}
}






String Read_location()
{
	char inChar;
	_Data_serial2 = "";
	while (Serial2.available()) {
		// get the new byte:
		inChar = (char)Serial2.read();
		// add it to the inputString:
		
		// if the incoming character is a newline, set a flag so the main loop can
		// do something about it:
		if (inChar == '\n') {
			// Hiển thị LCD RFID number dòng thứ 3
			return _Data_serial2;
		}
		else
		{
			_Data_serial2 += inChar;
		}
	}
	return "";
}
