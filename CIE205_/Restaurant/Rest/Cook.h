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

	int handledNormal = 0;
	int handledVegan = 0;
	int handledVIP = 0;

	int busyTime=0;      
	int idleTime=0;      
	int breakTime=0;     

	double Utilization; 

public:
	Cook(int id, ORD_TYPE ctype, int cspeed);
	~Cook();

	int GetID() const;
	ORD_TYPE GetType() const;
	int GetSpeed() const;

	void setID(int id);
	void setType(ORD_TYPE t);
	void SetSpeed(int s);

	bool IsBusy() const;
	bool IsInBreak() const;
	bool CanTakeOrder() const;
	
	void SetBusy(bool b) { isBusy = b; }
	

	void AssignOrder(Order* ord);
	void FinishOrder();
	int GetOrdersDone() const;

	void SetBreakDuration(int duration);
	void StartBreak(int timestep);
	void EndBreak();

	void AddHandledNormal() { handledNormal++; }
	void AddHandledVegan() { handledVegan++; }
	void AddHandledVIP() { handledVIP++; }

	void AddBusyTime(int t) { busyTime += t; }
	void AddIdleTime(int t) { idleTime += t; }
	void AddBreakTime(int t) { breakTime += t; }

	void CalculateUtilization();
	
	// getters
	int GetHandledNormal() const { return handledNormal; }
	int GetHandledVegan() const { return handledVegan; }
	int GetHandledVIP()   const { return handledVIP; }

	int GetBusyTime() const { return busyTime; }
	int GetIdleTime() const { return idleTime; }
	int GetBreakTime() const { return breakTime; }

	double GetUtilization() const { return Utilization; }
	

};

#endif
