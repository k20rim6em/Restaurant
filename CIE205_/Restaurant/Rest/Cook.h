#ifndef COOK_H
#define COOK_H

#include "../Defs.h"  

class Order;        

class Cook
{
private:
	int ID;
	ORD_TYPE type;
	int speed;

	bool isBusy;
	bool inBreak;

	Order* currentOrder;
	int ordersDone;

	int breakDuration;
	int breakStartTime;

public:
	Cook(int id, ORD_TYPE ctype, int cspeed);
	~Cook();

	int GetID() const;
	ORD_TYPE GetType() const;
	int GetSpeed() const;

	void setID(int id);
	void setType(ORD_TYPE t);
	void SetSpeed(int s);

	// States
	bool IsBusy() const;
	bool IsInBreak() const;
	bool CanTakeOrder() const;

	// Orders
	void AssignOrder(Order* ord);
	void FinishOrder();
	int GetOrdersDone() const;

	// Breaks
	void SetBreakDuration(int duration);
	void StartBreak(int timestep);
	void EndBreak();
};

#endif
