#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "Order.h"
#include "OrderBook.h"

#pragma comment (lib, "Ws2_32.lib")

#define PORT "50000"
#define BUFFER_SIZE 1024

Order parseOrder(std::string message, int& orderIdCounter) {
	std::stringstream ss(message);
	std::string typeStr, ticker;
	double amount, price;

	ss >> typeStr >> ticker >> amount >> price;

	OrderType type = ( typeStr == "BUY" ) ? OrderType::BUY : OrderType::SELL;

	return Order(orderIdCounter++, type, price, amount);
}

int main() {
	WSADATA wsadata;
	int iResult;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (iResult != 0) {
		std::cout << "WSAStartup failed: " << iResult << std::endl;
		return 1;
	}

	struct addrinfo *result = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, PORT, &hints, &result);

	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	freeaddrinfo(result);

	listen(ListenSocket, SOMAXCONN);
	std::cout << "======================================" << std::endl;
	std::cout << "   EXCHANGE SERVER LSTENING ON PORT " << PORT << std::endl;
	std::cout << "======================================" << std::endl;
	SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
	std::cout << ">> Client connected!" << std::endl;

	OrderBook orderBook;
	char recvbuf[BUFFER_SIZE];
	int orderIdCounter = 1;

	do {
		memset(recvbuf, 0, BUFFER_SIZE);

		iResult = recv(ClientSocket, recvbuf, BUFFER_SIZE, 0);

		if (iResult > 0) {
			std::string message(recvbuf);

			message.erase(std::remove(message.begin(), message.end(), '\n'), message.end());
			std::cout << "[NET] Received: " << message << std::endl;
			if (message.length() > 5) {
				try {
					Order newOrder = parseOrder(message, orderIdCounter);
					orderBook.addOrder(newOrder);

					std::string response = "Order " + std::to_string(newOrder.id) + " ACK\n";
					send(ClientSocket, response.c_str(), response.length(), 0);
				}
				catch (...) {
					std::cout << "[ERROR] Invalid Order Format" << std::endl;
				}
			}
		}
	} while (iResult > 0);

	std::cout << "Client disconnected." << std::endl;
	closesocket(ClientSocket);
	WSACleanup();
	return 0;
}