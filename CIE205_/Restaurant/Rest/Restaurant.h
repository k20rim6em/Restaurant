#ifndef __RESTAURANT_H_
#define __RESTAURANT_H_

#include "..\Defs.h"
#include "..\CMUgraphicsLib\CMUgraphics.h"
#include "..\GUI\GUI.h"
#include "..\Generic_DS\Queue.h"
#include "../Pri_Queue.h"
#include "..\Events\Event.h"
#include "../Generic_DS/Node.h"


#include "Order.h"


class Restaurant  
{	
private:
	GUI *pGUI;
	Queue<Event*> EventsQueue;	//Queue of all events that will be loaded from file

	
	/// ==> 
	//	DEMO-related members. Should be removed in phases 1&2
	Queue<Order*> DEMO_Queue;	//Important: This is just for demo
	/// ==>

	Queue<Order*> Normal_Waiting;
	Queue<Order*> Vegan_Waiting;
	Pri_Queue<Order*> VIP_Waiting;
	Queue<Order*> InService;
	Queue<Order*> Finished;
	Queue<Cook*> Normal_Cooks;
	Queue<Cook*> Vegan_Cooks;
	Queue<Cook*> VIP_Cooks;
	Queue<Cook*> AvailableNormalCooks;
	Queue<Cook*> AvailableVeganCooks;
	Queue<Cook*> AvailableVIPCooks;
	int ServiceCounter = 0;

	
	int normalOrders = 0;
	int veganOrders = 0;
	int vipOrders = 0;
	int totalOrders = normalOrders + veganOrders + vipOrders;

	int totalWaitingTime = 0;
	int totalServiceTime = 0;
	double avgWaitingTime = 0.0;
	double avgServiceTime = 0.0;

	int autoPromotedOrders = 0;

	int totalCooks = 0;
	int normalCooks = 0;
	int veganCooks = 0;
	int vipCooks = 0;
	
	
	int AutoP;
	int N_Cooks, G_Cooks, V_Cooks;
	int SN, SG, SV;
	int BO; // Number of orders before a break
	int BN, BG, BV; // Break durations for Normal, Vegan, and VIP cooks
	int N_Orders;
	int G_Orders;
	int V_Orders;

public:
	
	Restaurant();
	~Restaurant();
	
	void ExecuteEvents(int TimeStep);	//executes all events at current timestep
	void RunSimulation();
	bool LoadData(string inputfilename);
	void InsertFinishedOrder(Order* o);
	void WriteOutputFile();
	string AskForFileName();

	


	void FillDrawingList();

	
	void AddToNormalWaiting(Order* p);
	void AddToVeganWaiting(Order* p);
	void AddToVIPWaiting(Order* p);
	bool CancelNormalOrder(int id);
	void UpdateCookTimes();
	void CalculateAllCooksUtilization();
	void PromoteOrder(int id, double extra);

	void AutoPromoteOrders(int CurrentTimeStep);

	bool CancelOrder(int id);


	void AssignCookToOrder(int CurrentTimeStep);
	int CalculateST(Order* o, Cook* c);


	// helpers
	Order* GetNextNormal();
	Order* GetNextVegan();
	Order* GetNextVIP();


};

#endif
