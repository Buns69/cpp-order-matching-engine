#pragma once
#include "OrderType.h"

struct Order {
	int id;
	OrderType type;
	double price;
	double amount;

	Order(int _id, OrderType _type, double _price, double _amount)
		: id(_id), type(_type), price(_price), amount(_amount) { }
};