#include "Order.h"
#include "Cook.h"

Order::Order(int id, ORD_TYPE r_Type,int s ,double m)
{
	ID = (id>0&&id<1000)?id:0;	//1<ID<999
	type = r_Type;
	status = WAIT;

	Distance = 0;
	totalMoney = 0;
	status = WAIT;
	money = m;
	ArrTime = 0;
	ServTime = 0;
	FinishTime = 0;
	size = s;
	pAssignedCook = nullptr;
}

Order::~Order()
{
}

int Order::GetID()
{
	return ID;
}


ORD_TYPE Order::GetType() const
{
	return type;
}


void Order::SetDistance(int d)
{
	Distance = d>0?d:0;
}

int Order::GetDistance() const
{
	return Distance;
}


void Order::setStatus(ORD_STATUS s)
{
	status = s;
}

void Order::setMoney(double m)
{
	totalMoney = m;
}


ORD_STATUS Order::getStatus() const
{
	return status;
}

double Order::getMoney() const
{
	return totalMoney;
}

void Order::setType(ORD_TYPE t)
{
	type = t;
}

void Order::setArrTime(int t)
{
	ArrTime = t;
}



int Order::GetArrTime() const {
	return ArrTime; 
}







