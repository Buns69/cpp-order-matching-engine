#pragma once
#include "Order.h"
#include <map>
#include <vector>
#include <functional>

class OrderBook {
private:

	std::map<double, std::vector<Order>, std::greater<double>> bids;

	std::map<double, std::vector<Order>> asks;

	void matchBuyOrder(Order& order);
	void matchSellOrder(Order& order);

public:

	void addOrder(Order order);
};