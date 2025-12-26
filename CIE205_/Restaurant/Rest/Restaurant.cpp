#include <cstdlib>
#include <time.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath> 
using namespace std;
#include "Restaurant.h"
#include "Cook.h"
#include "Order.h"
#include "..\Defs.h"
#include "..\Events\ArrivalEvent.h"
#include "../PromotionEvent.h"
#include "../CancellationEvent.h"
#include "../Generic_DS/Queue.h"
#include "../GUI/GUI.h"
#include "../Pri_Queue.h"

Restaurant::Restaurant()
{
    pGUI = NULL;
}

void Restaurant::RunSimulation()
{
    pGUI = new GUI;
    PROG_MODE mode = pGUI->getGUIMode();

    string filename = AskForFileName();
    if (!LoadData(filename))
    {
        pGUI->waitForClick();   
        return;                
    }


    int CurrentTimeStep = 1;

    while (!EventsQueue.isEmpty())
    {
      
        pGUI->PrintMessage("TS = " + to_string(CurrentTimeStep));

      
        ExecuteEvents(CurrentTimeStep);
        AssignCookToOrder(CurrentTimeStep);
        Order* o;
        int priority;

       
       if (VIP_Waiting.dequeue(o, priority))
        {
            o->SetServTime(CurrentTimeStep);
            o->SetFinishTime(CurrentTimeStep + 1);   
            InService.enqueue(o);
        }

       
        if (Vegan_Waiting.dequeue(o))
        {
            o->SetServTime(CurrentTimeStep);
            o->SetFinishTime(CurrentTimeStep + 1);
            InService.enqueue(o);
        }

       
        if (Normal_Waiting.dequeue(o))
        {
            o->SetServTime(CurrentTimeStep);
            o->SetFinishTime(CurrentTimeStep + 1);
            InService.enqueue(o);
        }

       
        Queue<Order*> temp;
        while (InService.dequeue(o))
        {
            if (o->GetFinishTime() == CurrentTimeStep)  
            { 
                Finished.enqueue(o);
            }
            else
            {
                temp.enqueue(o); 
            }

        }

        while (temp.dequeue(o))
        { 
            InService.enqueue(o);
        }

        pGUI->ResetDrawingList();
        FillDrawingList();
        pGUI->UpdateInterface();

   
        if ( mode == MODE_INTR || mode == MODE_DEMO)
        { 
            pGUI->waitForClick();
        }
        else if(mode == MODE_STEP )
        {
			Sleep(1000);
        }
        CurrentTimeStep++;
    }

    pGUI->PrintMessage("Simulation End | Cooks: N=" + to_string(N_Cooks)+ " G=" + to_string(G_Cooks)+ " V=" + to_string(V_Cooks));
    cout << "Processing TS = " << CurrentTimeStep << endl;
    cout << "VIP waiting = " << VIP_Waiting.getSize() << endl;
    cout << "Normal waiting = " << Normal_Waiting.getSize() << endl;
    cout << "Vegan waiting = " << Vegan_Waiting.getSize() << endl;
    pGUI->waitForClick();
}



void Restaurant::ExecuteEvents(int CurrentTimeStep)
{
    Event* pE;
    while (EventsQueue.peekFront(pE))
    {
        if (pE->getEventTime() > CurrentTimeStep)
        { 
            return;
        }
        pE->Execute(this);
        EventsQueue.dequeue(pE);
        delete pE;
    }
}

Restaurant::~Restaurant()
{
    if (pGUI) delete pGUI;
}
int Restaurant::CalculateST(Order* o, Cook* c)
{
    return (int)ceil((double)o->GetSize() / c->GetSpeed());
}

void Restaurant::AssignCookToOrder(int CurrentTimeStep)
{
    Cook* c;
    Order* o;
    int priority;

    // VIP Orders
    if (!VIP_Waiting.isEmpty() && !VIP_Cooks.isEmpty())
    {
        VIP_Cooks.dequeue(c);
        if (c->CanTakeOrder())
        {
            VIP_Waiting.dequeue(o, priority);
            c->AssignOrder(o);

            int ST = CalculateST(o, c);
            o->SetServTime(CurrentTimeStep);
            o->SetFinishTime(CurrentTimeStep + ST);

            InService.enqueue(o);
        }
        VIP_Cooks.enqueue(c);
    }

    // Vegan Orders
    if (!Vegan_Waiting.isEmpty() && !Vegan_Cooks.isEmpty())
    {
        Vegan_Cooks.dequeue(c);
        if (c->CanTakeOrder())
        {
            Vegan_Waiting.dequeue(o);
            c->AssignOrder(o);

            int ST = CalculateST(o, c);
            o->SetServTime(CurrentTimeStep);
            o->SetFinishTime(CurrentTimeStep + ST);

            InService.enqueue(o);
        }
        Vegan_Cooks.enqueue(c);
    }

    // Normal Orders
    if (!Normal_Waiting.isEmpty() && !Normal_Cooks.isEmpty())
    {
        Normal_Cooks.dequeue(c);
        if (c->CanTakeOrder())
        {
            Normal_Waiting.dequeue(o);
            c->AssignOrder(o);

            int ST = CalculateST(o, c);
            o->SetServTime(CurrentTimeStep);
            o->SetFinishTime(CurrentTimeStep + ST);

            InService.enqueue(o);
        }
        Normal_Cooks.enqueue(c);
    }
}

void Restaurant::FillDrawingList()
{
    Order* o;
    Cook* c;

    
   Queue<Order*> tempN;
    while (Normal_Waiting.dequeue(o))
    {
        pGUI->AddToDrawingList(o);
        tempN.enqueue(o);
    }
    while (tempN.dequeue(o))
    { 
        Normal_Waiting.enqueue(o);
    }

    Queue<Order*> tempG;
    while (Vegan_Waiting.dequeue(o))
    {
        pGUI->AddToDrawingList(o);
        tempG.enqueue(o);
    }
    while (tempG.dequeue(o))
    { 
        Vegan_Waiting.enqueue(o);
    }
    Order* vipOrder;
    int priority;
    vector<pair<Order*, int>> tempVIP;

    while (VIP_Waiting.dequeue(vipOrder, priority))
    {
        pGUI->AddToDrawingList(vipOrder);
        tempVIP.push_back({ vipOrder, priority });
    }
    
    for (auto& item : tempVIP)
    {
        VIP_Waiting.enqueue(item.first, item.second);
    }



    Queue<Order*> tempS;
    while (InService.dequeue(o))
    {
        pGUI->AddToDrawingList(o);
        tempS.enqueue(o);
    }
    while (tempS.dequeue(o))
        InService.enqueue(o);


    
    Queue<Order*> tempF;
    while (Finished.dequeue(o))
    {
        pGUI->AddToDrawingList(o);
        tempF.enqueue(o);
    }
    while (tempF.dequeue(o))
    { 
        Finished.enqueue(o);
    }

    
    Queue<Cook*> tempNC;
    while (Normal_Cooks.dequeue(c))
    {
        pGUI->AddToDrawingList(c);
        tempNC.enqueue(c);
    }
    while (tempNC.dequeue(c))
    { 
        Normal_Cooks.enqueue(c);
    }

    
    Queue<Cook*> tempVC;
    while (Vegan_Cooks.dequeue(c))
    {
        pGUI->AddToDrawingList(c);
        tempVC.enqueue(c);
    }
    while (tempVC.dequeue(c))
        Vegan_Cooks.enqueue(c);


    
    Queue<Cook*> tempVIPC;
    while (VIP_Cooks.dequeue(c))
    {
        pGUI->AddToDrawingList(c);
        tempVIPC.enqueue(c);
    }
    while (tempVIPC.dequeue(c))
    { 
        VIP_Cooks.enqueue(c);
    }
}


void Restaurant::AddToNormalWaiting(Order* p)
{ 
    Normal_Waiting.enqueue(p); 
}
void Restaurant::AddToVeganWaiting(Order* p) 
{
    Vegan_Waiting.enqueue(p); 
}
void Restaurant::AddToVIPWaiting(Order* p)
{ 
    int priority = (int)(p->getMoney());
    VIP_Waiting.enqueue(p, priority);
}

bool Restaurant::CancelNormalOrder(int id)
{
    Queue<Order*> temp;
    Order* x;
    bool deleted = false;

    while (Normal_Waiting.dequeue(x))
    {
        if (x->GetID() == id)
        {
            deleted = true;
            delete x;
        }
        else
            temp.enqueue(x);
    }

    while (temp.dequeue(x))
    { 
        Normal_Waiting.enqueue(x);
    }
    return deleted;
}

void Restaurant::PromoteOrder(int id, double extra)
{
    Queue<Order*> temp;
    Order* x;
    bool found = false;

    while (Normal_Waiting.dequeue(x))
    {
        if (x->GetID() == id)
        {
            x->setMoney(x->getMoney() + extra);
            x->setType(TYPE_VIP);
            int priority = (int)(x->getMoney());
            VIP_Waiting.enqueue(x, priority);
            found = true;
        }
        else
        { 
            temp.enqueue(x);
        }
    }

    while (temp.dequeue(x))
    { 
        Normal_Waiting.enqueue(x);
    }
}

Order* Restaurant::GetNextNormal()
{
    if (Normal_Waiting.isEmpty())
    {
        return nullptr;
    }

   Order* o;
    Normal_Waiting.dequeue(o);
    return o;
}

Order* Restaurant::GetNextVegan()
{
    if (Vegan_Waiting.isEmpty())
    {
        return nullptr;
    }

    Order* o;
    Vegan_Waiting.dequeue(o);
    return o;
}

Order* Restaurant::GetNextVIP()
{
    if (VIP_Waiting.isEmpty()) 
    {
        return nullptr;
    }

    Order* o;
    int priority;
    VIP_Waiting.dequeue(o, priority);


    return o;
}


string Restaurant::AskForFileName()
{
    pGUI->PrintMessage("Enter input file name (example: test.txt):");
    string filename = pGUI->GetString();   
    return filename;
}


// Data Load function
bool Restaurant::LoadData(string inputfilename) 
{
    ifstream file(inputfilename);

    if (!file.is_open())
    {
        if (pGUI) pGUI->PrintMessage("Error: File not found! Please restart.");
        else cerr << "Error: cannot open input file!" << endl;
        return false;
    }


    int N = 0, G = 0, V = 0;       
    int SN = 0, SG = 0, SV = 0;    
                  
    int M = 0;                    

    file >> N >> G >> V;
    file >> SN >> SG >> SV;
    file >> BO >> BN >> BG >> BV;
    file >> AutoP;
    file >> M;

  
    N_Cooks = N; 
    G_Cooks = G; 
    V_Cooks = V;

    for (int i = 0; i < N; i++)
    { 
        Normal_Cooks.enqueue(new Cook(i + 1, TYPE_NRM, SN));
    }
    for (int i = 0; i < G; i++)
    { 
        Vegan_Cooks.enqueue(new Cook(i + 1, TYPE_VGAN, SG));
    }
    for (int i = 0; i < V; i++)
    { 
        VIP_Cooks.enqueue(new Cook(i + 1, TYPE_VIP, SV));
    }
    
    for (int i = 0; i < M; i++) 
    {
        char eventtype;
        file >> eventtype;

        if (!file)
        {
            break;
        }

        if (eventtype == 'R')
        {
            char TYP;
            int TS, ID, DIST, SIZE;
            double MONY;

            file >> TYP >> TS >> ID >> DIST >> SIZE >> MONY;
            ORD_TYPE Type;

            if (TYP == 'N')
            { 
                Type = TYPE_NRM;
            }
            else if (TYP == 'G')
            {
                Type = TYPE_VGAN;
            }
            else 
            { 
            Type = TYPE_VIP;
            }

            Event* pEv = new ArrivalEvent(TS, Type, ID, DIST, SIZE, MONY);
            EventsQueue.enqueue(pEv);
        }
        else if (eventtype == 'X') 
        {
            int TS, ID;
            file >> TS >> ID;
            Event* pEv = new CancellationEvent(TS, ID);
            EventsQueue.enqueue(pEv);
        }
        else if (eventtype == 'P') 
        {
            int TS, ID; double ExMony;
            file >> TS >> ID >> ExMony;
            Event* pEv = new PromotionEvent(TS, ID, ExMony);
            EventsQueue.enqueue(pEv);
        }
        else 
        {
            string rest;
            getline(file, rest);
            if (pGUI) pGUI->PrintMessage("Warning: unknown event type in input file.");
        }
    }


    file.close();
    return true;
}
