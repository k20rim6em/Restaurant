#ifndef __ARRIVAL_EVENT_H_
#define __ARRIVAL_EVENT_H_

#include "Event.h"


//class for the arrival event
class ArrivalEvent: public Event
{
	//info about the order ralted to arrival event
	int OrdDistance;	//order distance
	ORD_TYPE OrdType;		//order type: Normal, vegan, VIP	                
	double OrdMoney;	//Total order money
	int ordSize;       // size of the order

public:
	ArrivalEvent(int eTime, ORD_TYPE oType, int oID, int distance, int Ordsize, double money);
	//Add more constructors if needed
	

	virtual void Execute(Restaurant *pRest);	//override execute function
	


};

#endif