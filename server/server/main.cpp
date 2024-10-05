#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <thread>
#include <vector>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

//Function to initialize the winsock
bool Initialize() {
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0; //0 indicates successful init
}

//Function to keep the server always up and running
void InteractWithClient(SOCKET clientSocket, vector<SOCKET> &clients){
	
	cout << "Client connected..." << endl;
	char buffer[4096];

	while (1) {
		int bytesrecvd = recv(clientSocket, buffer, sizeof(buffer), 0); //recv is a blocking call here
		if (bytesrecvd <= 0) {
			cout << "Client disconnected..." << endl;
			break;
		}
		string message(buffer, bytesrecvd);
		cout << "Message from client : " << message << endl;

		for (auto client : clients) {
			if (client != clientSocket) {
				send(client, message.c_str(), message.length(), 0);
			}
		}

		auto it = find(clients.begin(), clients.end(), clientSocket);
		if (it != clients.end()) {
			clients.erase(it);
		}

	}
	
	closesocket(clientSocket);
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
	vector<SOCKET> clients;

	while (1) {
		//Accept
		SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);

		if (clientSocket == INVALID_SOCKET) {
			cout << "Invalid client socket..." << endl;
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}

		clients.push_back(clientSocket);      //Store the clientsockets into the vector
		thread t1(InteractWithClient, clientSocket, std::ref(clients));
	}
	
	closesocket(listenSocket);

	WSACleanup(); //finalization
	return 0;
}
