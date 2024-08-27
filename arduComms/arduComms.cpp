#define _CRT_SECURE_NO_WARNINGS 1
#include<iostream>
using namespace std;
#include<string>
#include<stdlib.h>
#include"SerialPort.h"

char output[MAX_DATA_LENGTH];
char incomingData[MAX_DATA_LENGTH];

char port[] = "\\\\.\\COM4";
char* comport = port;

int main() {
	SerialPort arduino(port);
	if (arduino.isConnected()) {
		cout << "Connection made" << endl << endl;
	}
	else {
		cout << "Error in port name" << endl << endl;
	}
	while (arduino.isConnected()) {
		//cout << "output is " << output << endl;

		cout << "Enter your command: " << endl;
		string data;
		getline(cin, data);

		//append terminator
		char* charArray = new char[data.length() + 2];
		copy(data.begin(), data.end(), charArray);
		charArray[data.size()] = '\n';
		charArray[data.size()+1] = '\0';
		//strcpy(charArray, data.c_str());

		arduino.writeSerialPort(charArray, MAX_DATA_LENGTH);
		//read response
		arduino.readSerialPort(output, MAX_DATA_LENGTH);

		cout << ">> " << output;

		delete[] charArray;
		memset(output, 0, sizeof(output));
	}
	return 0;
}