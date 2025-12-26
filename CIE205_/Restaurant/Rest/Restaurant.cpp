#include <cstdlib>
#include <time.h>
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
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

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

    while (!EventsQueue.isEmpty() || !InService.isEmpty()
        || !VIP_Waiting.isEmpty() || !Normal_Waiting.isEmpty()
        || !Vegan_Waiting.isEmpty())
    {
        pGUI->PrintMessage("TS = " + to_string(CurrentTimeStep));

        ExecuteEvents(CurrentTimeStep);
        AutoPromoteOrders(CurrentTimeStep);
        AssignCookToOrder(CurrentTimeStep);
        UpdateCookTimes();

        Queue<Order*> temp;
        Order* o;

        while (InService.dequeue(o))
        {
            if (o->GetFinishTime() == CurrentTimeStep)
            {

                if (o->GetType() == TYPE_NRM) normalOrders++;
                else if (o->GetType() == TYPE_VGAN) veganOrders++;
                else vipOrders++;

                if (o->GetCook())
                    o->GetCook()->FinishOrder();

                InsertFinishedOrder(o);

                totalOrders++;

                
                totalWaitingTime += o->GetWaitingTime();
                totalServiceTime += o->GetServTime();

                avgWaitingTime = (double)totalWaitingTime / totalOrders;
                avgServiceTime = (double)totalServiceTime / totalOrders;

                if (o->WasAutoPromoted())
                    autoPromotedOrders++;
            }
            else
                temp.enqueue(o);
        }

        while (temp.dequeue(o))
            InService.enqueue(o);

    // 2. Vegan Orders Priority: Vegan Cook ONLY
    while (Vegan_Waiting.peekFront(pOrd)) {
        if (AvailableVeganCooks.dequeue(pCook)) {
            Vegan_Waiting.dequeue(pOrd);
            Assign(pCook, pOrd, currentTS);
        }
        else break;
    }

        if (mode == MODE_INTR || mode == MODE_DEMO)
            pGUI->waitForClick();
        else if (mode == MODE_STEP)
            Sleep(1000);

        CurrentTimeStep++;
    }

    CalculateAllCooksUtilization();
    WriteOutputFile();
    pGUI->waitForClick();
}

            // Return cook to their specific available queue
            ReturnCookToQueue(pCook);
        }
        else break;
    }
}


void Restaurant::ExecuteEvents(int CurrentTimeStep)
{
    Event* pE;
    while (EventsQueue.peekFront(pE) && pE->getEventTime() <= CurrentTimeStep)
    {
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

    while (!VIP_Waiting.isEmpty() && (!VIP_Cooks.isEmpty() || !Normal_Cooks.isEmpty()))
    {
        if (!VIP_Cooks.isEmpty())
            VIP_Cooks.dequeue(c);
        else
            Normal_Cooks.dequeue(c);

        if (!c->CanTakeOrder())
        {
            if (c->GetType() == TYPE_VIP)
                VIP_Cooks.enqueue(c);
            else
                Normal_Cooks.enqueue(c);
            continue;
        }

        VIP_Waiting.dequeue(o, priority);
        c->AssignOrder(o);
        o->SetCook(c);
        o->SetWaitingTime(CurrentTimeStep - o->GetArrTime());
        int ST = CalculateST(o, c);
        o->SetServTime(ST);
        o->SetFinishTime(CurrentTimeStep + ST);
        c->AddHandledVIP();
        o->setStatus(SRV);
        InService.enqueue(o);

        if (c->GetType() == TYPE_VIP)
            VIP_Cooks.enqueue(c);
        else
            Normal_Cooks.enqueue(c);
    }

    while (!Vegan_Waiting.isEmpty() && !Vegan_Cooks.isEmpty())
    {
        Vegan_Cooks.dequeue(c);

        if (!c->CanTakeOrder())
        {
            Vegan_Cooks.enqueue(c);
            continue;
        }

        Vegan_Waiting.dequeue(o);
        c->AssignOrder(o);
        o->SetCook(c);
        o->SetWaitingTime(CurrentTimeStep - o->GetArrTime());
        int ST = CalculateST(o, c);
        o->SetServTime(ST);
        o->SetFinishTime(CurrentTimeStep + ST);
        c->AddHandledVegan();
        o->setStatus(SRV);
        InService.enqueue(o);

        Vegan_Cooks.enqueue(c);
    }

    while (!Normal_Waiting.isEmpty() && (!Normal_Cooks.isEmpty() || !VIP_Cooks.isEmpty()))
    {
        if (!Normal_Cooks.isEmpty())
            Normal_Cooks.dequeue(c);
        else
            VIP_Cooks.dequeue(c);

        if (!c->CanTakeOrder())
        {
            if (c->GetType() == TYPE_NRM)
                Normal_Cooks.enqueue(c);
            else
                VIP_Cooks.enqueue(c);
            continue;
        }

        Normal_Waiting.dequeue(o);
        c->AssignOrder(o);
        o->SetCook(c);
        o->SetWaitingTime(CurrentTimeStep - o->GetArrTime());
        int ST = CalculateST(o, c);
        o->SetServTime(ST);
        o->SetFinishTime(CurrentTimeStep + ST);
        c->AddHandledNormal();
        o->setStatus(SRV);
        InService.enqueue(o);

        if (c->GetType() == TYPE_NRM)
            Normal_Cooks.enqueue(c);
        else
            VIP_Cooks.enqueue(c);
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

void Restaurant::UpdateCookTimes()
{
    Cook* c;
    Queue<Cook*> temp;

    while (Normal_Cooks.dequeue(c))
    {
        if (c->IsBusy())
            c->AddBusyTime(1);
        else if (c->IsInBreak())
            c->AddBreakTime(1);
        else
            c->AddIdleTime(1);

        temp.enqueue(c);
    }
    while (temp.dequeue(c))
        Normal_Cooks.enqueue(c);

    while (Vegan_Cooks.dequeue(c))
    {
        if (c->IsBusy())
            c->AddBusyTime(1);
        else if (c->IsInBreak())
            c->AddBreakTime(1);
        else
            c->AddIdleTime(1);

        temp.enqueue(c);
    }
    while (temp.dequeue(c))
        Vegan_Cooks.enqueue(c);

    while (VIP_Cooks.dequeue(c))
    {
        if (c->IsBusy())
            c->AddBusyTime(1);
        else if (c->IsInBreak())
            c->AddBreakTime(1);
        else
            c->AddIdleTime(1);

        temp.enqueue(c);
    }
    while (temp.dequeue(c))
        VIP_Cooks.enqueue(c);
}


void Restaurant::CalculateAllCooksUtilization()
{
    Cook* c;
    Queue<Cook*> temp;

    while (Normal_Cooks.dequeue(c))
    {
        c->CalculateUtilization();
        temp.enqueue(c);
    }
    while (temp.dequeue(c))
        Normal_Cooks.enqueue(c);

    while (Vegan_Cooks.dequeue(c))
    {
        c->CalculateUtilization();
        temp.enqueue(c);
    }
    while (temp.dequeue(c))
        Vegan_Cooks.enqueue(c);

    while (VIP_Cooks.dequeue(c))
    {
        c->CalculateUtilization();
        temp.enqueue(c);
    }
    while (temp.dequeue(c))
        VIP_Cooks.enqueue(c);
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
            x->SetAutoPromoted(true);
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
void Restaurant::AutoPromoteOrders(int CurrentTimeStep)
{
    Queue<Order*> temp;
    Order* o;

    while (Normal_Waiting.dequeue(o))
    {
        int waitingTime = CurrentTimeStep - o->GetArrTime();
        if (waitingTime >= AutoP)
        {
            o->setType(TYPE_VIP);
            o->SetAutoPromoted(true);
            int priority = (int)(o->getMoney());
            VIP_Waiting.enqueue(o, priority);
        }
        else
        {
            temp.enqueue(o); 
        }
    }

    while (temp.dequeue(o))
        Normal_Waiting.enqueue(o);
}

bool Restaurant::CancelOrder(int id)
{
    Order* o;
    Queue<Order*> temp;
    bool deleted = false;
    int priority;

    auto cancelFromQueue = [&](Queue<Order*>& q, ORD_TYPE loc) {
        while (q.dequeue(o))
        {
            if (o->GetID() == id)
            {
                deleted = true;
                delete o;
            }
            else
                temp.enqueue(o);
        }
        while (temp.dequeue(o))
            q.enqueue(o);

        if (deleted)
            return true;
        return false;
        };

    if (cancelFromQueue(Normal_Waiting, TYPE_NRM)) return true;
    if (cancelFromQueue(Vegan_Waiting, TYPE_VGAN)) return true;

    vector<pair<Order*, int>> tempVIP;
    while (VIP_Waiting.dequeue(o, priority))
    {
        if (o->GetID() == id)
        {
            deleted = true;
            delete o;
        }
        else
            tempVIP.push_back({ o, priority });
    }
    for (auto& p : tempVIP)
        VIP_Waiting.enqueue(p.first, p.second);

    return deleted;
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

    totalCooks = N_Cooks + G_Cooks + V_Cooks;
    normalCooks = N_Cooks;
    veganCooks = G_Cooks;
    vipCooks = V_Cooks;

    int idCounter = 1;
    for (int i = 0; i < N; i++) {
        Normal_Cooks.enqueue(new Cook(idCounter++, TYPE_NRM, SN));
    }
    for (int i = 0; i < G; i++) {
        Vegan_Cooks.enqueue(new Cook(idCounter++, TYPE_VGAN, SG));
    }
    for (int i = 0; i < V; i++) {
        VIP_Cooks.enqueue(new Cook(idCounter++, TYPE_VIP, SV));
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

void Restaurant::InsertFinishedOrder(Order* o)
{
    Queue<Order*> tempQueue;
    Order* currOrder;
    bool inserted = false;

    while (Finished.peekFront(currOrder))
    {
        Finished.dequeue(currOrder);

        if (!inserted)
        {
            if (o->GetFinishTime() < currOrder->GetFinishTime() ||
                (o->GetFinishTime() == currOrder->GetFinishTime() && o->GetServTime() < currOrder->GetServTime()))
            {
                tempQueue.enqueue(o);  
                inserted = true;
            }
        }
        tempQueue.enqueue(currOrder);
    }

    if (!inserted)
        tempQueue.enqueue(o);

    while (tempQueue.peekFront(currOrder))
    {
        tempQueue.dequeue(currOrder);
        Finished.enqueue(currOrder);
    }
}

void Restaurant::WriteOutputFile()
{
    ofstream outFile("Output.txt");
    if (!outFile.is_open())
        return;

    
    outFile << "FT\tID\tAT\tWT\tST\n";

    Queue<Order*> tempQueue;  
    Order* ordPtr;

    while (Finished.peekFront(ordPtr))
    {
        
        outFile << ordPtr->GetFinishTime() << "\t"
            << ordPtr->GetID() << "\t"
            << ordPtr->GetArrTime() << "\t"
            << ordPtr->GetWaitingTime() << "\t"
            << ordPtr->GetServTime() << "\n";

        
        Finished.dequeue(ordPtr);
        tempQueue.enqueue(ordPtr);
    }

    while (tempQueue.peekFront(ordPtr))
    {
        tempQueue.dequeue(ordPtr);
        Finished.enqueue(ordPtr);
    }

    outFile << "\n";

    outFile << "Orders: " << totalOrders
        << " [Norm:" << normalOrders
        << ", Veg:" << veganOrders
        << ", VIP:" << vipOrders << "]\n";

    outFile << "Cooks: " << totalCooks
        << " [Norm:" << normalCooks
        << ", Veg:" << veganCooks
        << ", VIP:" << vipCooks << "]\n";

    outFile << "Avg Wait = " << avgWaitingTime
        << ", Avg Serv = " << avgServiceTime << "\n";

    outFile << "Auto-promoted: " << autoPromotedOrders << "\n";

    
    auto printCookStats = [&](Queue<Cook*>& cooks) {
        Cook* cPtr;
        Queue<Cook*> tempCooks;

        while (cooks.peekFront(cPtr))
        {
            cPtr->CalculateUtilization();

            outFile << "Cook " << cPtr->GetID() << ": Orders [Norm:" << cPtr->GetHandledNormal()
                << ", Veg:" << cPtr->GetHandledVegan()
                << ", VIP:" << cPtr->GetHandledVIP() << "], "
                << "Busy: " << cPtr->GetBusyTime()
                << ", Idle: " << cPtr->GetIdleTime()
                << ", Break/Injury: " << cPtr->GetBreakTime()
                << ", Utilization: " << cPtr->GetUtilization() << "%\n";

            cooks.dequeue(cPtr);
            tempCooks.enqueue(cPtr);
        }

        
        while (tempCooks.peekFront(cPtr))
        {
            tempCooks.dequeue(cPtr);
            cooks.enqueue(cPtr);
        }
        };

    printCookStats(Normal_Cooks);
    printCookStats(Vegan_Cooks);
    printCookStats(VIP_Cooks);

    outFile.close();
}


