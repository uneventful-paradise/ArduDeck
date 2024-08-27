#include "SerialPort.h"

SerialPort::SerialPort(char* portName)
{
    this->status = { 0 };
    this->errors = 0;
    this->connected = false;
    //create and open the COM I/O device. Returns a gandle to the COM device
    this->handler = CreateFileA(
        static_cast<LPCSTR>(portName),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    //get the error code of last error
    this->errors = GetLastError();

    /*if (this->handler == INVALID_HANDLE_VALUE) {
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            printf("ERROR: Handle was not attached. Reason: %s not available\n", portName);
        }
        else
        {
            printf("ERROR: Invalid handle value on %s\n", portName);
        }
    }*/

    if (this->handler == INVALID_HANDLE_VALUE) {
        //error code for FILE_NOT_FOUND
        if (this->errors == 2) {
            printf("ERROR: Handle was not attached. Reason: %s not available\n", portName);
        }
        //error code for ERROR_ACCESS_DENIED
        else if(this->errors == 5)
        {
            printf("ERROR: Invalid handle value on %s. Another app is already using this device\n", portName);
        }
    }
    else {
        //proceed to configuring the serial port. DCB = device control block is a structure that holds COM PORT settings
        DCB dcbSerialParameters = { 0 };
        //attempt to get the COM state
        if (!GetCommState(this->handler, &dcbSerialParameters)) {
            printf("failed to get current serial parameters");
        }
        else {
            dcbSerialParameters.BaudRate = CBR_9600;
            dcbSerialParameters.ByteSize = 8;
            dcbSerialParameters.StopBits = ONESTOPBIT;
            dcbSerialParameters.Parity = NOPARITY;
            dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;

            if (!SetCommState(handler, &dcbSerialParameters))
            {
                printf("ALERT: could not set Serial port parameters\n");
            }
            else {
                this->connected = true;
                PurgeComm(this->handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
                Sleep(ARDUINO_WAIT_TIME);
            }
        }
    }
}

SerialPort::~SerialPort()
{
    if (this->connected) {
        this->connected = false;
        CloseHandle(this->handler);
    }
}

int SerialPort::readSerialPort(char* buffer, unsigned int buf_size)
{
    DWORD bytesRead;
    unsigned int toRead = 0;

    ClearCommError(this->handler, &this->errors, &this->status);
    printf("Current buffer size is %d\n", this->status.cbInQue);
    //cbInQue = number of bytes in the port buffer that havent been read yet. now we determine how many bytes we need to read
    if (this->status.cbInQue > 0) {//read the desired amount of bytes

        if (this->status.cbInQue > buf_size) {
            toRead = buf_size;
        }//else read however many bytes are in buffer
        else {
            toRead = this->status.cbInQue;
        }
        printf("toRead is %d\n", toRead);
    }
    else {
        printf("ALERT: Negative buffer size\n");
    }
    if (ReadFile(this->handler, buffer, toRead, &bytesRead, NULL)) {
        printf("Read %ld bytes. Target was %d\n--------------------\n", bytesRead, toRead);
        return bytesRead;
    }
    return 0;
}

bool SerialPort::writeSerialPort(char* buffer, unsigned int buf_size)
{
    DWORD bytesSend;

    if (!WriteFile(this->handler, (void*)buffer, buf_size, &bytesSend, 0)) {
        ClearCommError(this->handler, &this->errors, &this->status);
        printf("Error when writing message\n");
        return false;
    }
    else {
        printf("Wrote %d bytes\n----------------\n", bytesSend);
        return true;
    }
}

bool SerialPort::isConnected()
{
    return this->connected;
}