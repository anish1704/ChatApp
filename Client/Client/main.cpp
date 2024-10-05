#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;


//Initialize
bool Initialize() {
	WSADATA data;
	return (WSAStartup(MAKEWORD(2, 2), &data) == 0);
}

int main() {
	
	if (!Initialize()) {
		cout << "Winsock init failed..." << endl;
		return 1;
	}

	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET) {
		cout << "Socket created is invalid..." << endl;
		return 1;
	}

	int port = 12345;
	string serveraddress = "127.0.0.1";
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	inet_pton(AF_INET, serveraddress.c_str(), &(serveraddr.sin_addr));

	if(connect(s, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
		cout << "Unable to connect to the server..." << endl;
		closesocket(s);
		WSACleanup();
		return 1;
	}

	cout << "Successfully connected to the server..." << endl;

	string message = "Hello there!";
	int bytesent;
	bytesent = send(s, message.c_str(), message.length(), 0);

	if (bytesent == SOCKET_ERROR) {
		cout << "Send failed..." << endl;
		closesocket(s);
		WSACleanup();
		return 1;
	}

	closesocket(s);
	WSACleanup();
	return 0;
}