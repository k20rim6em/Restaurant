#include "Cook.h"

Cook::Cook(int id, ORD_TYPE ctype, int cspeed)
{
	ID = id;
	type = ctype;
	speed = cspeed;

	isBusy = false;
	inBreak = false;
	currentOrder = nullptr;
	ordersDone = 0;

	breakDuration = 0;
	breakStartTime = -1;
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
	currentOrder = ord;
	isBusy = true;
}

void Cook::FinishOrder()
{
	currentOrder = nullptr;
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

void Cook::CalculateUtilization()
{
	int total = busyTime + idleTime + breakTime;
	if (total > 0)
		Utilization = ((double)busyTime / total) * 100.0;
	else
		Utilization = 0;
}
