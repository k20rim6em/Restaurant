#include "Cook.h"

Cook::Cook(int id, ORD_TYPE ctype, int cspeed)
{
	ID = id;
	type = ctype;
	speed = cspeed;

	isBusy = false;
	inBreak = false;
	pCurrentOrder = nullptr;
	ordersDone = 0;
	finishTime = 0;
	numOrdersServed = 0;
	breakDuration = 0;
	breakStartTime = -1;
	breakEndStep = 0;
	nOrdersServed = 0;
}

Cook::~Cook()
{
}

int Cook::GetID() const
{
	return ID;
}

ORD_TYPE Cook::GetType() const
{
	return type;
}

void Cook::setID(int id)
{
	ID = id;
}

void Cook::setType(ORD_TYPE t)
{
	type = t;
}

int Cook::GetSpeed() const
{
	return speed;
}

void Cook::SetSpeed(int s)
{
	speed = s;
}

// ================= States =================

bool Cook::IsBusy() const
{
	return isBusy;
}

bool Cook::IsInBreak() const
{
	return inBreak;
}

bool Cook::CanTakeOrder() const
{
	return !isBusy && !inBreak;
}

// ================= Orders =================

void Cook::AssignOrder(Order* ord)
{
	pCurrentOrder = ord;
	isBusy = true;
}

void Cook::FinishOrder()
{
	pCurrentOrder = nullptr;
	isBusy = false;
	ordersDone++;
}

int Cook::GetOrdersDone() const
{
	return ordersDone;
}



void Cook::SetBreakDuration(int duration)
{
	breakDuration = duration;
}

void Cook::StartBreak(int timestep)
{
	inBreak = true;
	isBusy = true;          
	breakStartTime = timestep;
}

void Cook::EndBreak()
{
	inBreak = false;
	isBusy = false;
	breakStartTime = -1;
}



