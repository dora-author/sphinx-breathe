#include <iostream>
#include <thread>
#include <string>
#include <memory>
#include <tuple>
#include <fstream>
#include <iomanip>
#include <time.h>
#include <chrono>
#include <sstream>

#include "../include/socket/util.h"
#include "../include/socket/define.h"
#include "../include/socket/udpreceiver.h"

#include "../include/msg/morai_msg.h"

using namespace morai::comm::udp;
using namespace morai::msg;
using namespace cirrus::msg;

UdpReceiver receiver;
int recv_size = 0;

// main loop
void Loop();

// sensro for ros2 bridge
void gnss_sensor_parse(char* msg, int total_size);
void imu_sensor_parse(char* msg, int total_size);
void radar_sensor_parse(char* msg, int total_size);
void lidar_sensor_parse(char* msg, int total_size);
void image_sensor_parse(char* msg, int total_size);

void SureSoft();
void VORDME();
void Samsung();
void MoraiCollision();
void Read_CirrusTx(char* data);
void Read_Sensor_Image(Header header, char* data);
void Read_LargePacket(Header header, char* data);
void Read_packetImage(Header header, char* data);
void Read_Lidar(Header header, char* data);
void Lidar3D_V1();
std::stringstream Timestamp();

int main(int argc, const char* argv[])
{
    std::tuple<char*, int> t = morai::comm::Util::arg_to_addr(argc, argv);
    char* ip = std::get<0>(t);
    int port = std::get<1>(t);
    std::cout << "ip : " << ip << " port : "  << port << std::endl;
    
    //bool res = receiver.Connect("127.0.0.1", 9100);
    //bool res = receiver.Connect("10.52.11.2", 9092);
    //bool res = receiver.Connect("127.0.0.1", 9090);
    morai::msg::ReturnValue ret = receiver.Connect(ip, port);
    if (!ret.result)
    {        
        std::cout << "UDP Receiver connection failed " << ret.GetErrorCodeToString() << std::endl;
        return 0;
    }

    // receive Loop
    Loop();
    
    receiver.Disconnect();
    return 0;   
}

void VORDME()
{
    while(1)
    {
        char* data = receiver.ReceiveMsg(&recv_size);
        // check
        if (recv_size <= 0) {
            std::cout << "error??" << recv_size << std::endl;
            continue;
        }

        if (recv_size == 24)
        {
            int hertz = 0;
            float lat = 0;
            float lon = 0;
            float angle = 0;
            double dist = 0;

            int index = 0;
            memcpy(&hertz, data, 4);
            index = 4;
            memcpy(&lat, data + index, 4);
            index += 4;
            memcpy(&lon, data + index, 4);
            index += 4;
            memcpy(&angle, data + index, 4);
            index += 4;
            memcpy(&dist, data + index, 8);            
            std::cout << "hertz:" << hertz << "lat:" << lat<< "lon:" << lon<< "angle:" << angle<< "dist:" << dist << std::endl;
        }
        else if (recv_size == 400)
        {
            int hertz[100];
            memcpy(hertz, data, 400);
            std::cout << "hertz:" << hertz[0] << std::endl;
        }
        morai::comm::DeletePtr(data);        
    }
}

std::stringstream Timestamp()
{
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

    std::stringstream ss;
    ss << std::put_time(std::localtime(&t), "%F %T.") << std::setfill('0') << std::setw(3) << ms % 1000;
    return ss;
}

void Lidar3D_V1()
{
    std::ofstream ofs("test.txt");
    ofs.clear();

    if (ofs.fail())
    {
        std::cout << "error" << std::endl;
    }

    while (1)
    {
        char* data = receiver.ReceiveMsg(&recv_size);
        // check
        if (recv_size <= 0) {
            std::cout << "error??" << recv_size << std::endl;
            continue;        
        }

        int index = 1206 - 6;
        int timestamp;
        memcpy(&timestamp, data + index, sizeof(int));
        std::cout << "[" << Timestamp().str() << "] : " << timestamp << std::endl;
        ofs << "[" << Timestamp().str() << "] : " << timestamp << std::endl;

        //std::cout << "data : " << data << std::endl;
        morai::comm::DeletePtr(data);
    }
    ofs.close();
}

void SureSoft()
{
    while(1)
    {
        char* data = receiver.ReceiveMsg(&recv_size);
        // check
        if (recv_size <= 0) {
            std::cout << "error??" << recv_size << std::endl;
            continue;
        }
        std::cout << "data : " << data << std::endl;
        morai::comm::DeletePtr(data);
    }
}

void MoraiCollision()
{
    while (1)
    {
        char* data = receiver.ReceiveMsg(&recv_size);
        // check
        if (recv_size <= 0) {
            std::cout << "error??" << recv_size << std::endl;
            continue;
        }

        int headersize = 31;
        char* header = new char[headersize];
        memcpy(header, data, headersize);
        //std::cout << "data : " << header << std::endl;
        
        int offset = headersize;
        
        uint16_t obj_type;
        memcpy(&obj_type, data + offset, 2);
        std::cout << "obj_type : " << obj_type << ", ";
        offset += 2;

        uint16_t obj_id;
        memcpy(&obj_id, data + offset, 2);
        std::cout << "obj_id : " << obj_id << ", ";
        offset += 2;

        float pos_x;
        memcpy(&pos_x, data + offset, 4);
        std::cout << "pos_x : " << pos_x << ", ";

        morai::comm::DeletePtr(header);
    }
}

void Samsung()
{
    while(1)
    {
        char* data = receiver.ReceiveMsg(&recv_size);
        // check
        if (recv_size <= 0) {
            std::cout << "error??" << recv_size << std::endl;
            continue;
        }

        //int type;
        //int size;

        //memcpy(&type, data, 4);
        //memcpy(&size, data + 4, 4);

        //std::cout << "type : " << type << std::endl;
        //std::cout << "size : " << size << std::endl;
        
        //char* msg = new char[recv_size];
        //memset(msg, 0x00, size);
        //memcpy(msg, data, recv_size);
        
        std::cout << "data : " << data << std::endl;

        morai::comm::DeletePtr(data);
        //delete[] msg;
    }
}

void Loop() {
    while(1)
    {   
        std::cout << "*** recv wait ***" << std::endl;
        char* packet = receiver.ReceiveMsg(&recv_size);

        // check
        if (recv_size <= 0)
        {
            std::cout << "error??" << recv_size << std::endl;
            continue;
        }

        int header_size = sizeof(morai::msg::Header);
        Header header;
        memcpy(&header, packet, header_size);

        std::cout << "msg_type : 0x" << (int)header.msg_type << std::endl;
        std::cout << "msg_size : " << header.msg_size << std::endl;
        std::cout << "protocol_type : " << (int)header.protocol_type << std::endl;
        std::cout << "send count : " << header.send_count << std::endl;
        
        int msg_size = header.msg_size;
        int total_size = header_size + header.msg_size;
        MsgType type = static_cast<MsgType>(header.msg_type);

        if (total_size > recv_size)
        {            
            int next_size = 0;
            int count = 1;            
            // while(header.msg_size >= offset)
            // {
            //     char* nData = receiver.GetData(&next_size);
            //     if (morai::comm::Util::isHeader(nData))
            //         break;

            //     memcpy(result + offset, nData, next_size);
            //     std::cout << count++ << " next_size : " << next_size;
            //     offset += next_size;                
            //     std::cout << " offset : " << offset << std::endl;
            //     morai::comm::DeletePtr(nData);
            // }
        }
        else
        {
            if (type == MsgType::GNSS_SENSOR)
            {
                std::cout << "recv : SENSOR_GNSS" << std::endl;
                gnss_sensor_parse(packet, total_size);
            }
            else if (type == MsgType::RADAR_SENSOR)
            {
                std::cout << "recv : SENSOR_RADAR" << std::endl;
                radar_sensor_parse(packet, total_size);
            }
            else if (type == MsgType::IMU_SENSOR)
            {
                std::cout << "recv : SENSOR_IMU" << std::endl;
                imu_sensor_parse(packet, total_size);
            }
            else if (type == MsgType::LIDAR_SENSOR)
            {
                std::cout << "recv : SENSOR_LIDAR" << std::endl;
                lidar_sensor_parse(packet, total_size);
            }
            else if (type == MsgType::CIRRUS_TX)
            {
                std::cout << "recv : CIRRUS_TX" << std::endl;
                //Read_CirrusTx(data);
            }
            else if (type == MsgType::CIRRUS_CONTROL_POSITION)
            {
                std::cout << "recv : CIRRUS_CONTROL_POSITION" << std::endl;
            }
            else if (type == MsgType::CIRRUS_BRAKING)
            {
                std::cout << "recv : CIRRUS_BRAKING" << std::endl;
            }
            else if (type == MsgType::CIRRUS_ENGINE)
            {
                std::cout << "recv : CIRRUS_ENGINE" << std::endl;
            }
            else if(type == MsgType::LARGE_PACKET)
            {
                std::cout << "recv : LARGE_DATA" << std::endl;
                //Read_LargePacket(header, data);
            }
            //            
        }
        recv_size = 0;

        morai::comm::DeletePtr(packet);
    }
}

void lidar_sensor_parse(char* msg, int total_size)
{
    morai::msg::Identifier id;
    memcpy(&id.id, msg + sizeof(morai::msg::Header), sizeof(morai::msg::Identifier));
    std::cout << "id : " << (int)id.id << std::endl;

    morai::msg::RosHeader ros_header;
    int index = sizeof(morai::msg::Header) + sizeof(morai::msg::Identifier);
    ros_header.Deserialize(msg, total_size, index);
    std::cout << "ros_header.stamp.sec : " << ros_header.stamp.sec << std::endl;
    std::cout << "ros_header.stamp.nanosec : " << ros_header.stamp.nanosec << std::endl;
    std::cout << "ros_header.frame id : " << ros_header.get_frame_id() << std::endl;
}

void image_sensor_parse(char* msg, int total_size)
{
    morai::msg::Identifier id;
    memcpy(&id.id, msg + sizeof(morai::msg::Header), sizeof(morai::msg::Identifier));
    std::cout << "id : " << (int)id.id << std::endl;

    morai::msg::RosHeader ros_header;
    int index = sizeof(morai::msg::Header) + sizeof(morai::msg::Identifier);
    ros_header.Deserialize(msg, total_size, index);
    std::cout << "ros_header.stamp.sec : " << ros_header.stamp.sec << std::endl;
    std::cout << "ros_header.stamp.nanosec : " << ros_header.stamp.nanosec << std::endl;
    std::cout << "ros_header.frame id : " << ros_header.get_frame_id() << std::endl;
}

void gnss_sensor_parse(char* msg, int total_size)
{
    morai::msg::Identifier id;
    memcpy(&id.id, msg + sizeof(morai::msg::Header), sizeof(morai::msg::Identifier));
    std::cout << "id : " << (int)id.id << std::endl;

    morai::msg::RosHeader ros_header;
    int index = sizeof(morai::msg::Header) + sizeof(morai::msg::Identifier);
    ros_header.Deserialize(msg, total_size, index);
    index += ros_header.length();
    std::cout << "ros_header.stamp.sec : " << ros_header.stamp.sec << std::endl;
    std::cout << "ros_header.stamp.nanosec : " << ros_header.stamp.nanosec << std::endl;
    std::cout << "ros_header.frame id : " << ros_header.get_frame_id() << std::endl;
    // logic

    morai::msg::NavSatStatus status;
    memcpy(&status, msg + index, sizeof(morai::msg::NavSatStatus));
    index += sizeof(NavSatStatus);

    morai::msg::NavSatFix navsatfix;
    memcpy(&navsatfix, msg + index, sizeof(morai::msg::NavSatFix));
    std::cout << "lat : " << navsatfix.latitude << " lon : " << navsatfix.longitude << " alt : " << navsatfix.altitude << std::endl;    
}

void imu_sensor_parse(char* msg, int total_size)
{
    morai::msg::Identifier id;
    memcpy(&id.id, msg + sizeof(morai::msg::Header), sizeof(morai::msg::Identifier));
    std::cout << "id : " << (int)id.id << std::endl;

    morai::msg::RosHeader ros_header;
    int index = sizeof(morai::msg::Header) + sizeof(morai::msg::Identifier);
    ros_header.Deserialize(msg, total_size, index);
    index += ros_header.length();
    std::cout << "ros_header.stamp.sec : " << ros_header.stamp.sec << std::endl;
    std::cout << "ros_header.stamp.nanosec : " << ros_header.stamp.nanosec << std::endl;
    std::cout << "ros_header.frame id : " << ros_header.get_frame_id() << std::endl;
    // logic

    morai::msg::Imu imu;
    memcpy(&imu, msg + index, sizeof(morai::msg::Imu));
    std::cout << "[angular_velocity] x : " << imu.angular_velocity.x << " y : " << imu.angular_velocity.y << " z : " << imu.angular_velocity.z << std::endl;
    std::cout << "[linear_acceleration] x : " << imu.linear_acceleration.x << " y : " << imu.linear_acceleration.y << " z : " << imu.linear_acceleration.z << std::endl;
    std::cout << "[orientation] x : " << imu.orientation.x << " y : " << imu.orientation.y << " z : " << imu.orientation.z << std::endl;
}

void radar_sensor_parse(char* msg, int total_size)
{
    morai::msg::Identifier id;
    memcpy(&id.id, msg + sizeof(morai::msg::Header), sizeof(morai::msg::Identifier));
    std::cout << "id : " << (int)id.id << std::endl;

    morai::msg::RosHeader ros_header;
    int index = sizeof(morai::msg::Header) + sizeof(morai::msg::Identifier);
    ros_header.Deserialize(msg, total_size, index);
    std::cout << "ros_header.stamp.sec : " << ros_header.stamp.sec << std::endl;
    std::cout << "ros_header.stamp.nanosec : " << ros_header.stamp.nanosec << std::endl;
    std::cout << "ros_header.frame id : " << ros_header.get_frame_id() << std::endl;
    // logic
}

void Read_packetImage(Header header, char* data)
{
    //LargePacket lp;
    //lp.data = new char[header.msg_size];
    //memcpy(lp.data, data + MORAI_HEADER_SIZE, (int)header.data_size);    
    //std::cout << "check start" << std::endl;
    //for (int i = 0; i < (int)header.data_size - 1; i++)
    //{
    //    if ((int)(header.data_size / 2) > i)
    //    {
    //        if (lp.data[i] != 'a')
    //        {
    //            std::cout << "====  error  === " << i  << " " << lp.data[i]<< std::endl;
    //        }
    //    }
    //    else
    //    {
    //        if (lp.data[i] != 'b')
    //        {
    //            std::cout << "====  error  === " << i  << " " << lp.data[i]<< std::endl;
    //        }
    //    }
    //}
    //std::cout << "check end" << std::endl;
}

void Read_LargePacket(Header header, char* data)
{
    //LargePacket lp;
    ////data.data = new char[header.msg_size];
    //memcpy(lp.data, data + MORAI_HEADER_SIZE, header.data_size);    
    //std::cout << "check start" << std::endl;
    //for (int i = 0; i < (int)header.data_size - 1; i++)
    //{
    //    if (lp.data[i] != 'a')
    //    {
    //        std::cout << "====  error  === " << i  << " " << lp.data[i]<< std::endl;
    //    }
    //}

    //// last byte check
    //if (lp.data[header.data_size - 1] != 'b')
    //    std::cout << "====  error last byte === " << std::endl;

    //std::cout << "check end" << std::endl;
}

void Read_Lidar(Header header, char* data)
{
    /*int data_size = recv_size - 20;
    std::cout << "data_size : " << data_size << "recv size : " << recv_size << std::endl;

    int id = 0;
    memcpy(&id, data + MORAI_HEADER_SIZE, 4);
    std::cout << "id : " << id << std::endl;*/
}

void Read_Sensor_Image(Header header, char* data)
{    
    ////Sensor_Image data;                      
    //int data_size = recv_size - 20;
    //std::cout << "data_size : " << data_size << "recv size : " << recv_size << std::endl;
    ////memset(&data, 0x00, msg_size);
    //int id = 0;
    //memcpy(&id, data + MORAI_HEADER_SIZE, 4);
    //std::cout << "id : " << id << std::endl;

    //int offset = MORAI_HEADER_SIZE + 4;
    //offset += sizeof(int32_t);  // sec
    //offset += sizeof(uint32_t);  // nano
    //offset += sizeof(uint8_t);  // strLen
    //offset += 7;                // frame_id
    //offset += sizeof(uint32_t);  // height
    //offset += sizeof(uint32_t);  // width
    //offset += sizeof(uint16_t);  // strlen
    //offset += 6;                // encoding
    //offset += sizeof(uint8_t);  // bigendian
    //offset += sizeof(uint32_t); // step    

    //int imageSize = 640*480*4;
    //uint8_t* imageBuffer = new uint8_t[imageSize];
    //memset(imageBuffer, 0x00, imageSize);
    //memcpy(imageBuffer, data + offset, imageSize);

    //std::ofstream fout;
    //
    //std::string fileName = std::to_string(id) + "output.txt";       	
    //fout.open(fileName.c_str());
    //if (fout.is_open())
    //{
    //    std::cout << "file open ok" << std::endl;
    //    for (int i = 0 ; i < imageSize; i++)
    //    {
    //        fout << (int)imageBuffer[i] << " ";
    //    }
    //}
    //else
    //{
    //    std::cout << "file open failed" << std::endl;
    //}
    //
    //fout.close();
    //delete [] imageBuffer;
}

void Read_CirrusTx(char* data)
{
    /*CirrusTx tx;
    int data_size = sizeof(CirrusTx);
    memset(&tx, 0x00, data_size);
    memcpy(&tx, data + MORAI_HEADER_SIZE, 40);    

    std::cout << "lat : " << tx.lat << std::endl;
    std::cout << "lon : " << tx.lon << std::endl;
    std::cout << "alt : " << tx.alt << std::endl;
    std::cout << "heading : " << tx.heading << std::endl;
    std::cout << "pitch : " << tx.pitch << std::endl;
    std::cout << "roll : " << tx.roll << std::endl;
    std::cout << "sur_aileron : " << tx.sur_aileron << std::endl;
    std::cout << "sur_elevator : " << tx.sur_elevator << std::endl;
    std::cout << "sur_flap : " << tx.sur_flap << std::endl;
    std::cout << "sur_rudder : " << tx.sur_rudder << std::endl;
    char* temp = new char[1024];
    std::cout << "111" << std::endl;
    memcpy(temp, data + 60, 1024);
    std::cout << "222" << std::endl;
    delete[] temp;
    std::cout << "333" << std::endl;*/
}
