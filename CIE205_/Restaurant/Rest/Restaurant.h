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


// it is the maestro of the project
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
    int ServiceCounter = 0; 


	
	
	
	//
	// TODO: Add More Data Members As Needed
	//
	int AutoP;
	int N_Cooks, G_Cooks, V_Cooks;
	int SN, SG, SV;
	int BO, BN, BG, BV;

public:
	
	Restaurant();
	~Restaurant();
	
	void ExecuteEvents(int TimeStep);	//executes all events at current timestep
	void RunSimulation();
	bool LoadData(string inputfilename);
	string AskForFileName();

	


	void FillDrawingList();

	//
	// TODO: Add More Member Functions As Needed
	//

	void AddToNormalWaiting(Order* p);
	void AddToVeganWaiting(Order* p);
	void AddToVIPWaiting(Order* p);
	bool CancelNormalOrder(int id);
	void PromoteOrder(int id, double extra);


	void AssignCookToOrder(int CurrentTimeStep);
	int CalculateST(Order* o, Cook* c);


	/*string AskForFileName();*/

	

	// helpers
	Order* GetNextNormal();
	Order* GetNextVegan();
	Order* GetNextVIP();


};

#endif
