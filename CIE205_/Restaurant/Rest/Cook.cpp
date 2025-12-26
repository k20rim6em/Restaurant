#include "Cook.h"


Cook::Cook(int id, ORD_TYPE ctype, int cspeed) 
{ 
	ID = id;
	type = ctype;
	speed = cspeed;
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
