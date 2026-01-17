#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include "OrderBook.h"
#include "LockFreeQueue.h"

LockFreeQueue<Order> orderQueue;
bool isRunning = true;

void clientThreadFunc() {
	for (int i = 0; i < 100; i++) {
		OrderType type = (i % 2 == 0) ? OrderType::BUY : OrderType::SELL;
		double price = 100.0 + (rand() % 5);
		double amount = 10 + (rand() % 10);

		Order newOrder(i, type, price, amount);
		orderQueue.push(newOrder);

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	std::cout << "[Client] All orders sent." << std::endl;
	isRunning = false;
}

void engineThreadFunc() {
	OrderBook book;
	std::cout << "[Engine] Started processing..." << std::endl;
	while (isRunning || !orderQueue.empty()) {
		if (orderQueue.empty()) {
			std::this_thread::yield();
			continue;
		}

		try {
			Order order = orderQueue.pop();
			book.addOrder(order);
			continue;
		}
		catch (...) {
			break;
		}
	}
	std::cout << "[Engine] Processing complete." << std::endl;
}

int main() {
	std::thread client(clientThreadFunc);
	std::thread engine(engineThreadFunc);

	client.join();
	engine.join();

	return 0;
}