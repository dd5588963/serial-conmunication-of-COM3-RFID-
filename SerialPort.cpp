/*

* Author: Manash Kumar Mandal

* Modified Library introduced in Arduino Playground which does not work

* This works perfectly

* LICENSE: MIT

*/



#include "SerialPort.hpp"



SerialPort::SerialPort(const char* portName)

{

    this->connected = false;



    this->handler = CreateFileA(static_cast<LPCSTR>(portName),

        GENERIC_READ | GENERIC_WRITE,

        0,

        NULL,

        OPEN_EXISTING,

        FILE_ATTRIBUTE_NORMAL,

        NULL);
    //假如this->handler是无效句柄
    if (this->handler == INVALID_HANDLE_VALUE)

    {

        if (GetLastError() == ERROR_FILE_NOT_FOUND)

        {

            std::cerr << "ERROR: Handle was not attached.Reason : " << portName << " not available\n";

        }

        else

        {

            std::cerr << "ERROR!!!\n";

        }

    }

    else

    {
        //DCB--记录串口信息的结构体
        DCB dcbSerialParameters = { 0 };


        //GetCommState 读取串口设置
        if (!GetCommState(this->handler, &dcbSerialParameters))

        {

            std::cerr << "Failed to get current serial parameters\n";

        }

        else

        {
            //配置串口信息参数
            dcbSerialParameters.BaudRate = CBR_57600;

            dcbSerialParameters.ByteSize = 8;

            dcbSerialParameters.StopBits = ONESTOPBIT;

            dcbSerialParameters.Parity = NOPARITY;

            dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;


            //假如配置失败
            if (!SetCommState(handler, &dcbSerialParameters))

            {
                //parameters--参数
                std::cout << "ALERT: could not set serial port parameters\n";

            }

            else
            //配置成功
            {

                this->connected = true;
                //PurgeComm初始化一个指定的通信设备的通信参数。 
                PurgeComm(this->handler, PURGE_RXCLEAR | PURGE_TXCLEAR);

                Sleep(ARDUINO_WAIT_TIME);

            }

        }

    }

}



SerialPort::~SerialPort()

{

    if (this->connected)

    {

        this->connected = false;

        CloseHandle(this->handler);

    }

}



// Reading bytes from serial port to buffer;

// returns read bytes count, or if error occurs, returns 0

int SerialPort::readSerialPort(const char* buffer, unsigned int buf_size)

{
    //DWORD双字节
    DWORD bytesRead{};
    
    //实际读取过程中的读取字节数
    unsigned int toRead = 0;



    ClearCommError(this->handler,  //通信设备的句柄
                   &this->errors, //接收错误代码变量的指针
                   &this->status); //通信状态缓冲区的指针
    //cbInQue--输入缓冲区中的字节数
    if (this->status.cbInQue > 0)

    {
        //toRead=min(buf_size,status.cbInQue)
        if (this->status.cbInQue > buf_size)

        {

            toRead = buf_size;

        }

        else

        {

            toRead = this->status.cbInQue;

        }

    }



    memset((void*)buffer, 0, buf_size);



    if (ReadFile(this->handler, (void*)buffer, toRead, &bytesRead, NULL))

    {

        return bytesRead;

    }



    return 0;

}



// Sending provided buffer to serial port;

// returns true if succeed, false if not

bool SerialPort::writeSerialPort(const char* buffer, unsigned int buf_size)

{

    DWORD bytesSend;



    if (!WriteFile(this->handler, (void*)buffer, buf_size, &bytesSend, 0))

    {

        ClearCommError(this->handler, &this->errors, &this->status);

        return false;

    }



    return true;

}



// Checking if serial port is connected

bool SerialPort::isConnected()

{

    if (!ClearCommError(this->handler, &this->errors, &this->status))

    {

        this->connected = false;

    }



    return this->connected;

}



void SerialPort::closeSerial()

{

    CloseHandle(this->handler);

}