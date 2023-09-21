#pragma warning(disable:4996)

#include <iostream>
#include <thread>
#include <cstring>
#include <memory>
#include <thread>
#include <ctime>
#include <malloc.h>
#include <string>
#include <fstream>

#include "../../include/socket/define.h"
#include "../../include/socket/define.h"
#include "../../include/socket/udpsender.h"
#include "../../include/socket/util.h"

//#include "../include/data/data_struct.h"
//#include "../include/data/data_type.h"
#include "../include/msg/morai_msg.h"

using namespace morai::comm::udp;
using namespace morai::msg;
using namespace cirrus::msg;

// packet process
void sendVehicleinfo();
void sendNavSatFix();
void sendSensorImage(int id);
void sendLargeData(int datasize);
void ReadTextFile(const char* file_path);
void sendLidarData();
void sendControlEngine();


// for cirrus
void sendCirrus_Tx();
void sendControlPostion();
void sendControlBraking();
void sendControlEngine();

// for rohde & schwarz
//void sendGNSS();

// vordme
void sendVORDME();

void TestFunc();
void insertData();

// main function
void SingleMessage();
void Loop();

int imageSize = 640 * 480;
UdpSender sender;
uint8_t* bmpBuffer;
uint8_t* bwBuffer;
SafeQueue<char*> m_DataQueue;

int main(int argc, const char* argv[])
{
	ReadTextFile("33output.txt");
	//ReadTextFile("baducpan.txt");

	std::cout << "===== UDP Sender Example =====" << std::endl;
	std::tuple<char*, int> t = morai::comm::Util::arg_to_addr(argc, argv);
	char* ip = std::get<0>(t);
	int port = std::get<1>(t);

	morai::msg::ReturnValue value = morai::comm::Util::isValidAddress(ip, port);
	if (!value.result)
	{
		std::cout << "ip is not valid!" << std::endl;
		return 0;
	}

	std::cout << "try to socket open -> ip : " << ip << " port : " << port << std::endl;
	//sender.Start(ip, port);
	morai::msg::ReturnValue ret = sender.Start("127.0.0.1", 9090);
	if (!ret.result)
	{
		std::cout << "UDP Sender Start failed " << ret.GetErrorCodeToString() << std::endl;
		return 0;
	}

	SingleMessage();
	sender.Stop();
	return 0;
}

void Loop()
{
	int index = 0;
	while(1)
    {
		sendSensorImage(index++);
		#if _WIN32
		Sleep(1);
		#else
		sleep(1);
		#endif		
    }
}

void sendVORDME()
{
	morai::msg::VORDMECtrl ctrl;
	const int size = sizeof(morai::msg::VORDMECtrl);
	memset(&ctrl, 0x00, size);

	// for test dummy
	for (int i = 0; i < 100; ++i)
	{
		ctrl.frequency[i] = 144 + i;
	}
	
	// buffer
	char buffer[size] = { 0, };
	memcpy(buffer, &ctrl, size);	
	sender.Send(buffer, size);
}

void insertData()
{
	int size = 1 << 20;
	char* dummy = new char[size];
	memset(dummy, 0x00, size);
	for (int i = 0; i < size; i++)
	{
		dummy[i] = 'a';
	}

	char* total = new char[size + 4];
	memcpy(total, &size, 4);
	memcpy(total + 4, dummy, size);

	m_DataQueue.Push(total);	

	if (dummy != nullptr)
	{
		delete[] dummy;
		dummy = nullptr;
	}
}

void TestFunc()
{
	char* out = nullptr;
	if (m_DataQueue.TryPop(std::chrono::milliseconds(1), out))
	{
		int out_size = 0;
		memcpy(&out_size, out, 4);
		std::cout << "out_size : " << out_size << std::endl;

		char* data = new char[out_size];
		memcpy(data, out + 4, out_size);

		if (data != nullptr)
		{
			delete[] data;
			data = nullptr;
		}

		if (out != nullptr)
		{
			delete[] out;
			out = nullptr;
		}
	}	
}

void ReadTextFile(const char* file_path)
{
	int imageSize = 640*480*4;
	bmpBuffer = new uint8_t[imageSize];
	memset(bmpBuffer, 0x00, imageSize);

	std::string line;
	std::ifstream myfile(file_path); // Open the file

	int index = 0;
	if (myfile.is_open()) { // Check if file is open
		std::cout << "file open ok aaa" << std::endl;
		while (getline(myfile, line, ' ')) { // Read the file line by line
			//std::cout << line << '\n'; // Print each line to the console
			//if (line.compare("255") == 0)
				//continue;		
			int value = atoi(line.c_str());
			bmpBuffer[index] = (uint8_t)value;
			//std::cout << (uint8_t)value;
			index++;
		}
		myfile.close(); // Close the file

		std::cout << "file read all" << std::endl;
	}
	else {
		std::cout << "Unable to open file";
	}		
	//return buffer;
}

void SingleMessage() {
	char msg[256];
    while(1)
    {
		std::cout << "=======================================" << std::endl;		
		std::cout << "1. CIRRUS_ENGINE" << std::endl;
		std::cout << "2. CIRRUS_BRAKING" << std::endl;	
		std::cout << "3. CIRRUS_CONTROL_POSITION" << std::endl;	
		std::cout << "4. CIRRUS_TX" << std::endl;
		std::cout << "5. VOR/DME Ctrl List" << std::endl;
		std::cout << "6. Rohde & Schwarz GNSS message" << std::endl;
		std::cout << ">> ";
		std::cin >> msg;

		if (msg[0] == '1')
		{
			sendControlEngine();			
		}
		else if (msg[0] == '2')
		{
			sendControlBraking();
		}		
		else if (msg[0] == '3')
		{
			sendControlPostion();
		}
		else if (msg[0] == '4')
		{
			sendCirrus_Tx();
		}
		else if (msg[0] == '5')
		{
			sendVORDME();
		}
		else if (msg[0] == '6')
		{
			//sendGNSS();
		}
		std::cout << "=> go to main" << std::endl;
    }
}

// void sendGNSS()
// {
// 	RohdeGNSS gnss;
// 	gnss.reserve0 = 0;
// 	gnss.reserve1 = 0;
// 	gnss.reserve2 = 0;
// 	gnss.reserve3 = 0;
// 	gnss.elapsedTime = 41;
// 	gnss.xx = 6378147;
// 	gnss.yy = 0;
// 	gnss.zz = 0;
// 	gnss.xDot = 0;
// 	gnss.yDot = 0;
// 	gnss.zDot = 0;
// 	gnss.xDotDot = 0;
// 	gnss.yDotDot = 0;
// 	gnss.zDotDot = 0;
// 	gnss.yaw = 0;
// 	gnss.pitch = 0;
// 	gnss.roll = 0;
// 	gnss.yawDot = 0;
// 	gnss.pitchDot = 0;
// 	gnss.rollDot = 0;        
// 	gnss.yawDotDot = 0;
// 	gnss.pitchDotDot = 0;
// 	gnss.rollDotDot = 0;
// 	gnss.yawDotDotDot = 0;
// 	gnss.pitchDotDotDot = 0;
// 	gnss.rollDotDotDot = 0;

// 	char buffer[1024];
// 	std::cout << "size : " << sizeof(RohdeGNSS) << std::endl;
// 	memcpy(buffer, &gnss, sizeof(RohdeGNSS));
// 	sender.Send(buffer, sizeof(RohdeGNSS));
// }

void sendLargeData(int msgsize)
{
	// make header
	int headersize = sizeof(Header);	
	// header + msg = buffer
	Header header;
	header.msg_type = MsgType::LARGE_PACKET;
	header.msg_size = msgsize;
	//strcpy(header.header_name, "MoraiHeader");

	// make data
	LargePacket packet;
	//packet->data = new char[datasize];
	std::cout << "create dummy data : " << msgsize << std::endl;
	memset(packet.data, 0x00, msgsize);

	int dummy = 0;
	for (int i = 0; i < msgsize ; i++)
	{
		if (msgsize / 2 > i)
		{
			packet.data[i] = 'a';
		}
		else
		{
			packet.data[i] = 'b';
		}	
	}

	//packet->data[datasize - 1] = 'b';
	//std::cout << "last byte " << datasize - 1 << " " << packet->data[datasize - 1] << std::endl;

	int total = headersize + header.msg_size;
	char* buffer = new char[total];
	memset(buffer, 0x00, total);
	memcpy(buffer, &header, headersize);
	memcpy(buffer + headersize, packet.data, msgsize);


	sender.Send(buffer, total);
	
	delete [] buffer;
}

void sendCirrus_Tx()
{
	int headersize = sizeof(Header);
	int msg_size = sizeof(CirrusTx);
	int total = headersize +  msg_size;
	std::cout << "msg total size : " << total << std::endl;

	char* buffer = new char[MAX_SEND_BUFFER_SIZE];
	Header header;
	header.msg_type = MsgType::CIRRUS_TX;
	//strcpy(header.header_name, "MoraiHeader");
	header.msg_size = msg_size;

	CirrusTx tx;
	
	tx.lat = 46.94f;			// left, right
	tx.lon = -92.17f;			// front, rear
	tx.alt = 0;					// up, down

	tx.heading = 0;
	tx.pitch = 0;
	tx.roll = 0;

	tx.sur_aileron = -2.21f;
	tx.sur_elevator = 4.61f;
	tx.sur_flap = 0;
	tx.sur_rudder = 1.82f;

	memcpy(buffer, &header, headersize);
	memcpy(buffer + headersize, &tx, msg_size);
	sender.Send(buffer, total);
	
	delete [] buffer;
}

void sendNavSatFix() {
	// int header_size = sizeof(Header);
	// int NavSatFix_size = sizeof(SensorNavSatFix);
	// int total = header_size + NavSatFix_size;
	// std::cout << "header_size size : " << header_size << std::endl;
	// std::cout << "NavSatFix_size size : " << NavSatFix_size << std::endl;
	// std::cout << "msg total size : " << total << std::endl;

	// char* buffer = new char[160];

	// Header header;
	// header.msg_type = MsgType::GNSS_SENSOR;
	// //strcpy(header.header_name, "MoraiHeader");
	// header.msg_size = NavSatFix_size;
	// memcpy(buffer, &header, header_size);

	// SensorNavSatFix navSatFix;
	// navSatFix.id = 123;
	// navSatFix.header.stamp.sec = 12345;
	// navSatFix.header.stamp.nanosec = 678910;
	// navSatFix.status.status = 1;
	// navSatFix.status.service = 2;
	// navSatFix.latitude = 123.456;
	// navSatFix.longitude = 7891011.121314;
	// navSatFix.altitude = 15161718.19202122;
	// for (int i = 0; i < 9; i++) {
	// 	navSatFix.position_covariance[i] = 10+i;
	// }
	// navSatFix.position_covariance_type = 20;

	// memcpy(buffer + header_size, &navSatFix, sizeof(SensorNavSatFix));
	// sender.Send(buffer, total);

	// morai::comm::DeletePtr(buffer);
}

int dummy = 0;
void sendSensorImage(int id) {
	
	// int header_size = sizeof(Header);

	// const int IMAGE_BUFF_SIZE = (1 << 24);
	// char* buffer = new char[IMAGE_BUFF_SIZE];

	// Header header;
	// header.msg_type = MsgType::IMAGE_SENSOR;
	// //strcpy_s(header.header_name, sizeof(header.header_name), "MoraiHeader");
	// //strcpy(header.header_name, "MoraiHeader");

	// SensorImage sensorImage;
	// sensorImage.id = 1;
	// sensorImage.header.stamp.sec = (int32_t)12345;
	// sensorImage.header.stamp.nanosec = (uint32_t)678910;

	// sensorImage.header.str_len = (uint8_t)6;
	// sensorImage.header.frame_id = new char[sensorImage.header.str_len + 1];
	// memset(sensorImage.header.frame_id, 0, sizeof(char) * (sensorImage.header.str_len + 1));
	// //strcpy_s(sensorImage.header.frame_id, sizeof(char) * (sensorImage.header.str_len + 1),"Camera");
	// strcpy(sensorImage.header.frame_id, "Camera");
	
	// sensorImage.height = (uint32_t)480;
	// sensorImage.width = (uint32_t)640;

	// sensorImage.str_len = (uint16_t)5;	
	// sensorImage.encoding = new char[sensorImage.str_len + 1];
	// memset(sensorImage.encoding, 0, sizeof(char) * (sensorImage.str_len + 1));
	// //strcpy_s(sensorImage.encoding, sensorImage.str_len + 1, "rgba8");
	// strcpy(sensorImage.encoding, "rgba8");

	// sensorImage.is_bigendian = (uint8_t)1;
	// sensorImage.step = sensorImage.width * 4;
	
	// int totalImageSize = 640 * 480 * 4;
	// sensorImage.data = new uint8_t[totalImageSize];

	// dummy += 1;
	// dummy %= 255;
	// memset(sensorImage.data, (uint8_t)dummy, sizeof(uint8_t) * totalImageSize);
	// memcpy(sensorImage.data, bmpBuffer, sizeof(uint8_t) * totalImageSize);

	// int offset = 0;
	// memcpy(buffer, &header, header_size);
	// offset += header_size;

	// memcpy(buffer + offset, &sensorImage.id, sizeof(int));
	// offset += sizeof(int);

	// memcpy(buffer + offset, &sensorImage.header.stamp.sec, sizeof(int32_t));
	// offset += sizeof(int32_t);
	// memcpy(buffer + offset, &sensorImage.header.stamp.nanosec, sizeof(uint32_t));
	// offset += sizeof(uint32_t);

	// memcpy(buffer + offset, &sensorImage.header.str_len, sizeof(uint8_t));
	// offset += sizeof(uint8_t);

	// //strcpy_s(buffer + offset, sizeof(buffer) + offset, sensorImage.header.frame_id);
	// strcpy(buffer + offset, sensorImage.header.frame_id);
	// //memcpy(buffer + offset, sensorImage.header.frame_id, sizeof(char*) * sensorImage.header.str_len);
	// offset += sensorImage.header.str_len + 1;

	// memcpy(buffer + offset, &sensorImage.height, sizeof(uint32_t));
	// offset += sizeof(uint32_t);
	// memcpy(buffer + offset, &sensorImage.width, sizeof(uint32_t));
	// offset += sizeof(uint32_t);

	// memcpy(buffer + offset, &sensorImage.str_len, sizeof(uint16_t));
	// offset += sizeof(uint16_t);

	// //memcpy(buffer + offset, sensorImage.encoding, sizeof(char*) * (sensorImage.str_len + 1));
	// //strcpy_s(buffer + offset, sizeof(buffer) + offset, sensorImage.encoding);
	// strcpy(buffer + offset, sensorImage.encoding);
	// offset += sensorImage.str_len + 1;

	// memcpy(buffer + offset, &sensorImage.is_bigendian, sizeof(uint8_t));
	// offset += sizeof(uint8_t);

	// memcpy(buffer + offset, &sensorImage.step, sizeof(uint32_t));
	// offset += sizeof(uint32_t);

	// memcpy(buffer + offset, bmpBuffer, sizeof(uint8_t) * totalImageSize);
	// offset += sizeof(uint8_t) * totalImageSize;	

	// header.msg_size = offset - header_size;

	// std::cout << "msg size : " << header.msg_size << std::endl;
	// memcpy(buffer, &header, sizeof(header_size));

	// sender.Send(buffer, header_size + header.msg_size);
	// std::cout << "offset : " << offset << std::endl;

	// morai::comm::DeletePtr(buffer);	
}

void sendVehicleinfo() {
	int headersize = sizeof(Header);
	// int msg_size = sizeof(Vehicle_info);
	// int total = headersize +  msg_size;
	// std::cout << "msg total size : " << total << std::endl;

	 char* buffer = new char[MAX_SEND_BUFFER_SIZE];
	// MsgHeader header;
	// header.msg_type = VEHICLE_INFO;
	// strcpy(header.header_name, "MoraiHeader");
	// header.msg_size = sizeof(Vehicle_info);        

	// Vehicle_info info;
	// info.id = 55;
	// info.position.x = 1.1f;
	// info.position.y = 1.2f;
	// info.position.z = 1.3f;
	// info.rotation.x = 1.4f;
	// info.rotation.y = 1.5f;
	// info.rotation.z = 1.6f;
	// info.scale.x = 1.7f;
	// info.scale.y = 1.8f;
	// info.scale.z = 1.9f;
	// info.reserved = 56;

	//  memcpy(buffer, &header, sizeof(MsgHeader));
	//  memcpy(buffer + headersize, &info, sizeof(Vehicle_info));
	//  sender.Send(buffer, total);

	delete[] buffer;
}

void sendControlPostion() 
{
	int header_size = sizeof(Header);
	char* buffer = new char[MAX_SEND_BUFFER_SIZE];

	Header header;
	header.msg_type = MsgType::CIRRUS_CONTROL_POSITION;
	//strcpy(header.header_name, "MoraiHeader");

	CirrusControlPosition controlPosition;
	controlPosition.SetDaCmd = -1;				// -1 ~ 1
	controlPosition.SetRollTrimCmd = 0.1f;		// -0.1 ~ 0.1
	controlPosition.SetDeCmd = 1;				// -1 ~ 1
	controlPosition.SetPitchTrimCmd = -0.1f;		// -0.1 ~ 0.1
	controlPosition.SetDrCmd = 0;				// -1 ~ 1
	controlPosition.SetDsCmd = 0;				// -1 ~ 1
	controlPosition.SetYawTrimCmd = 0;			// -0.1 ~ 0.1
	controlPosition.SetDfCmd = 0;				// 0 ~ 1

	int offset = 0;
	offset += header_size;

	memcpy(buffer + offset, &controlPosition.SetDaCmd, sizeof(float));
	offset += sizeof(float);
	memcpy(buffer + offset, &controlPosition.SetRollTrimCmd, sizeof(float));
	offset += sizeof(float);
	memcpy(buffer + offset, &controlPosition.SetDeCmd, sizeof(float));
	offset += sizeof(float);
	memcpy(buffer + offset, &controlPosition.SetPitchTrimCmd, sizeof(float));
	offset += sizeof(float);
	memcpy(buffer + offset, &controlPosition.SetDrCmd, sizeof(float));
	offset += sizeof(float);
	memcpy(buffer + offset, &controlPosition.SetDsCmd, sizeof(float));
	offset += sizeof(float);
	memcpy(buffer + offset, &controlPosition.SetYawTrimCmd, sizeof(float));
	offset += sizeof(float);
	memcpy(buffer + offset, &controlPosition.SetDfCmd, sizeof(float));
	offset += sizeof(float);

	header.msg_size = offset - header_size;
	memcpy(buffer, &header, header_size);

	sender.Send(buffer, offset);
};

void sendControlBraking() 
{
	char msg[256];
	while (1)
	{
		std::cout << ">> brake : 0 ~ 1 or exit : 2" << std::endl;
		std::cout << ">> ";
		std::cin >> msg;

		if (msg[0] == '2')
		{
			return;
		}
		else if (msg[0] != '2')
		{
			float input = (float)atof(msg);

			int header_size = sizeof(Header);
			char* buffer = new char[MAX_SEND_BUFFER_SIZE];

			Header header;
			header.msg_type = MsgType::CIRRUS_BRAKING;
			//strcpy(header.header_name, "MoraiHeader");

			CirrusBraking cirrusBraking;
			cirrusBraking.SetBrake = input;

			int offset = 0;
			offset += header_size;

			memcpy(buffer + offset, &cirrusBraking.SetBrake, sizeof(float));
			offset += sizeof(float);

			header.msg_size = offset - header_size;
			memcpy(buffer, &header, header_size);

			sender.Send(buffer, offset);

			std::cout << "send brake cmd : " << input << std::endl;
		}
	}
};

void sendControlEngine() 
{
	char msg[256];
	while (1)
	{
		std::cout << ">> throttle : 0 ~ 1 or exit : 2" << std::endl;
		std::cout << ">> ";
		std::cin >> msg;

		if (msg[0] == '2')
		{
			return;
		}
		else if (msg[0] != '2')
		{
			float input = (float)atof(msg);

			int header_size = sizeof(Header);
			char* buffer = new char[MAX_SEND_BUFFER_SIZE];

			Header header;
			header.msg_type = MsgType::CIRRUS_ENGINE;
			//strcpy(header.header_name, "MoraiHeader");

			CirrusEngine cirrusEngine;
			cirrusEngine.SetThrottleCmd = input;

			int offset = 0;
			offset += header_size;

			memcpy(buffer + offset, &cirrusEngine.SetThrottleCmd, sizeof(float));
			offset += sizeof(float);

			header.msg_size = offset - header_size;
			memcpy(buffer, &header, header_size);

			sender.Send(buffer, offset);

			std::cout << "send throttle cmd : " << input << std::endl;
		}		
	}	
};

void sendLidarData()
{
	// std::cout << "1" << std::endl;

	// int header_size = sizeof(Header);
	// const int large_buff = (1 << 24);
	// std::cout << "buffer size " << large_buff << std::endl;
	// char* buffer = new char[large_buff];

	// std::cout << "2" << std::endl;

	// // create morai header
	// Header header;
	// header.msg_type = MsgType::LIDAR_SENSOR;	

	// // create lidar message
	// SensorLidar msg;
	// msg.id = (uint32_t)123;

	// std::cout << "3" << std::endl;

	// // ros header
	// msg.header.stamp.sec = (int32_t)12345;
	// msg.header.stamp.nanosec = (uint32_t)678910;
	// msg.header.str_len = (uint8_t)5;
	// int size = msg.header.str_len + 1;
	// msg.header.frame_id = new char[size];
	// memset(msg.header.frame_id, 0, sizeof(char) * (size));
	// strcpy(msg.header.frame_id, "lidar");

	// std::cout << "4" << std::endl;

	// // continue
	// msg.height = (uint32_t)1;
	// msg.width = (uint32_t)(1800 * 16);
	// // lidar point width 1800 
	// // channel 16
	// // 4 byte 

	// uint8_t point_fields_size = 4;
	// msg.fields_len = point_fields_size;
	// msg.fields = new PointField[point_fields_size];		// velodyne
		
	// msg.fields[0].name = "x";
	// msg.fields[0].name_length = (uint8_t)msg.fields[0].name.length();
	// msg.fields[0].offset = (uint32_t)0;
	// msg.fields[0].datatype = (uint8_t)7;
	// msg.fields[0].count = (uint32_t)1;
	
	// msg.fields[1].name = "y";
	// msg.fields[1].name_length = (uint8_t)msg.fields[1].name.length() + 1;
	// msg.fields[1].offset = (uint32_t)4;
	// msg.fields[1].datatype = (uint8_t)7;
	// msg.fields[1].count = (uint32_t)1;
	
	// msg.fields[2].name = "z";
	// msg.fields[2].name_length = (uint8_t)msg.fields[2].name.length() + 1;
	// msg.fields[2].offset = (uint32_t)8;
	// msg.fields[2].datatype = (uint8_t)7;
	// msg.fields[2].count = (uint32_t)1;

	// msg.fields[3].name = "intensity";
	// msg.fields[3].name_length = (uint8_t)msg.fields[3].name.length() + 1;
	// msg.fields[3].offset = (uint32_t)12;
	// msg.fields[3].datatype = (uint8_t)7;
	// msg.fields[3].count = (uint32_t)1;

	// std::cout << "5" << std::endl;

	// msg.is_bigendian = true;
	// msg.point_step = (uint32_t)16;
	// msg.row_step = msg.width * sizeof(uint32_t);		// byte * width

	// int msg_size = msg.row_step * msg.height;
	// msg.data = new uint8_t[msg_size];	
	// for (int i = 0 ; i < msg_size; i++)
	// {
	// 	msg.data[i] = (uint8_t)1;
	// }
	// msg.is_dense = true;

	// std::cout << "6" << std::endl;

	// // insert buffer
	// int offset = 0;	
	// // morai header
	// memcpy(buffer, &header, header_size);
	// offset += header_size;	
	// // id
	// memcpy(buffer + offset, &msg.id, sizeof(uint32_t));
	// offset += sizeof(uint32_t);
	// // ros header
	// memcpy(buffer + offset, &msg.header.stamp.sec, sizeof(int32_t));
	// offset += sizeof(int32_t);
	// memcpy(buffer + offset, &msg.header.stamp.nanosec, sizeof(uint32_t));
	// offset += sizeof(uint32_t);
	// memcpy(buffer + offset, &msg.header.str_len, sizeof(uint8_t));
	// offset += sizeof(uint8_t);
	// strcpy(buffer + offset, msg.header.frame_id);
	// offset += msg.header.str_len + 1;
	// // continue
	// memcpy(buffer + offset, &msg.height, sizeof(uint32_t));
	// offset += sizeof(uint32_t);
	// memcpy(buffer + offset, &msg.width, sizeof(uint32_t));
	// offset += sizeof(uint32_t);	

	// memcpy(buffer + offset, &msg.fields_len, sizeof(uint8_t));
	// offset += sizeof(uint8_t);

	// for(int i = 0; i< point_fields_size; i++)
	// {
	// 	memcpy(buffer + offset, &msg.fields[i].name_length, sizeof(uint8_t));
	// 	offset += sizeof(uint8_t);
	// 	memcpy(buffer + offset, msg.fields[i].name.c_str(), msg.fields[i].name_length);
	// 	offset += msg.fields[i].name_length;
	// 	memcpy(buffer + offset, &msg.fields[i].offset, sizeof(uint32_t));
	// 	offset += sizeof(uint32_t);
	// 	memcpy(buffer + offset, &msg.fields[i].datatype, sizeof(uint8_t));
	// 	offset += sizeof(uint8_t);
	// 	memcpy(buffer + offset, &msg.fields[i].count, sizeof(uint32_t));
	// 	offset += sizeof(uint32_t);
	// }

	// std::cout << "8" << std::endl;

	// memcpy(buffer + offset, &msg.is_bigendian, sizeof(uint8_t));
	// offset += sizeof(uint8_t);
	// memcpy(buffer + offset, &msg.point_step, sizeof(uint32_t));
	// offset += sizeof(uint32_t);
	// memcpy(buffer + offset, &msg.row_step, sizeof(uint32_t));
	// offset += sizeof(uint32_t);

	// std::cout << "9 : data size " << msg_size << std::endl;	
	// memcpy(buffer + offset, msg.data, msg_size);
	// offset += msg_size;

	// std::cout << "10" << std::endl;

	// memcpy(buffer + offset, &msg.is_dense, sizeof(uint8_t));
	// offset += sizeof(uint8_t);

	// std::cout << "11" << std::endl;

	// // update total size
	// header.msg_size = offset - header_size;
	// memcpy(buffer, &header, header_size);	
	// std::cout << "offset size " << offset << std::endl;
	// sender.Send(buffer, offset + header_size);

	// delete[] buffer;
};
