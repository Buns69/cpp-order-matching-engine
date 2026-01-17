#include <iostream>
#include "OrderBook.h"

void logTrade(int buyid, int sellid, double price, double quantity) {

	std::cout << "[TRADE] Buy Order: " << buyid
		<< " match with Sell Order: " << sellid
		<< " at $" << price << " (" << quantity << " units)" << std::endl;
}

void OrderBook::addOrder(Order order) {

	if (order.type == OrderType::BUY) {
		matchBuyOrder(order);
	}
	else {
		matchSellOrder(order);
	}

	if (order.amount > 0) {
		if (order.type == OrderType::BUY) {
			bids[order.price].push_back(order);
		}
		else {
			asks[order.price].push_back(order);
		}
	}
}

void OrderBook::matchBuyOrder(Order& incomingOrder) {

	while (incomingOrder.amount > 0 && !asks.empty()) {
		auto bestAskIterator = asks.begin();
		double bestAskPrice = bestAskIterator->first;
		std::vector<Order>& ordersAtPrice = bestAskIterator->second;
		if (incomingOrder.price < bestAskPrice) {
			break;
		}
		while (incomingOrder.amount > 0 && !ordersAtPrice.empty()) {
			Order& sellOrder = ordersAtPrice.front();

			double tradeAmount = std::min(incomingOrder.amount, sellOrder.amount);

			logTrade(incomingOrder.id, sellOrder.id, bestAskPrice, tradeAmount);
			incomingOrder.amount -= tradeAmount;
			sellOrder.amount -= tradeAmount;
			if (sellOrder.amount == 0) {
				ordersAtPrice.erase(ordersAtPrice.begin());
			}
		}

		if (ordersAtPrice.empty()) {
			asks.erase(bestAskIterator);
		}
	}
}

void OrderBook::matchSellOrder(Order& incomingOrder) {

	while (incomingOrder.amount > 0 && !bids.empty()) {

		auto bestBidIterator = bids.begin();
		double bestBidPrice = bestBidIterator->first;
		std::vector<Order>& ordersAtPrice = bestBidIterator -> second;
		if (incomingOrder.price > bestBidPrice) {
			break;
		}

		while (incomingOrder.amount > 0 && !ordersAtPrice.empty()) {

			Order& buyOrder = ordersAtPrice.front();
			double tradeAmount = std::min(incomingOrder.amount, buyOrder.amount);

			logTrade(buyOrder.id, incomingOrder.id, bestBidPrice, tradeAmount);

			incomingOrder.amount -= tradeAmount;
			buyOrder.amount -= tradeAmount;

			if (buyOrder.amount == 0) {
				ordersAtPrice.erase(ordersAtPrice.begin());
			}
		}
		if (ordersAtPrice.empty()) {
			bids.erase(bestBidIterator);
		}
	}
}