#ifndef __ORDER_H_
#define __ORDER_H_

#include "..\Defs.h"
#include "Cook.h"



class Order
{

protected:

	int ID;       
	ORD_TYPE type;
	ORD_STATUS status;	
	int Distance;	
	Cook* pAssignedCook;
	int size;
	double money;
	double totalMoney;	

	int ArrTime;        
	int WaitingTime;    
	int ServTime;    
	int FinishTime;	
	bool wasAutoPromoted = false;
	


	   

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

	bool WasAutoPromoted() const { return wasAutoPromoted; }
	void SetAutoPromoted(bool val) { wasAutoPromoted = val; }


	
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
	
	void SetFinishTime(int t) { FinishTime = t; }
	
	void SetCook(Cook* c) { pAssignedCook = c; }
	Cook* GetCook() const { return pAssignedCook; }

};

#endif
