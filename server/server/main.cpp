#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

//Function to initialize the winsock
bool Initialize() {
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0; //0 indicates successful init
}

int main() {
	if (!Initialize()) {
		cout << "Winsock init failed..." << endl;
		return 1;
	}

	//socket creation
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET) {
		cout << "Socket creation failed..." << endl;
		return 1;
	}

	//create address structure
	int port = 12345;
	SOCKADDR_IN serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);

	//setting the address structure
	//Convert the ip address into the binary and put it into the server address
	if (InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1) {
		cout << "Setting address structure failed..." << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//bind
	if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
		cout << "Bind failed..." << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//listen
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		cout << "Listen failed..." << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	cout << "The server has started to listen on the port " << port << endl;

	SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);

	if (clientSocket == INVALID_SOCKET) {
		cout << "Invalid client socket..." << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	char buffer[4096]; 
	int bytesrecvd = recv(clientSocket, buffer, sizeof(buffer), 0);
	string message(buffer, bytesrecvd);
	cout << "Message from client : " << message << endl;

	closesocket(clientSocket);
	closesocket(listenSocket);

	WSACleanup(); //finalization
	return 0;
}
