#ifndef __ORDER_H_
#define __ORDER_H_

#include "..\Defs.h"
#include "Cook.h"

class Order
{

protected:

	int ID;         //Each order has a unique ID (from 1 --> 999 )
	ORD_TYPE type;		//order type: Normal, vegan, VIP
	ORD_STATUS status;	//waiting, in-service, done
	int Distance;	//The distance (in meters) between the order location and the resturant 
	Cook* pAssignedCook;
	int size;
	double money;
	double totalMoney;	//Total order money

	int ArrTime;        
	int WaitingTime;    
	int ServTime;    
	int FinishTime;	
	   
	
	
	//
	// TODO: Add More Data Members As Needed
	//

public:
	Order(int id, ORD_TYPE r_Type, int s, double money);
	virtual ~Order();

	int GetID() ;

	ORD_TYPE GetType() const;
	ORD_STATUS getStatus() const;

	int GetDistance() const;
	double getMoney() const;

	void SetDistance(int d);
	void setMoney(double m);
	void setStatus(ORD_STATUS s);
	void setType(ORD_TYPE t);


	
	//
	// TODO: Add More Member Functions As Needed
	//
	int GetWaitingTime() const { return WaitingTime; }
	int GetServTime() const { return ServTime; }

	void SetWaitingTime(int wt) { WaitingTime = wt; }
	void SetServTime(int st) { ServTime = st; }

	int GetSize() const;
	int GetArrTime() const;
	int GetFinishTime() const { return FinishTime; }

	void setArrTime(int t);   
	
	
	void SetServTime(int t) { ServTime = t; }
	void SetFinishTime(int t) { FinishTime = t; }
	


};

#endif
