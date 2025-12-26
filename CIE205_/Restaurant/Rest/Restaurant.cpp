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

    while (!EventsQueue.isEmpty() || !Normal_Waiting.isEmpty() || !Vegan_Waiting.isEmpty() ||
        !VIP_Waiting.isEmpty() || !BusyCooks.isEmpty())
    {
        pGUI->PrintMessage("TS = " + to_string(CurrentTimeStep));

        // 1. Execute events (Arrivals, Cancellations, Promotions)
        ExecuteEvents(CurrentTimeStep);

        // 2. Handle Auto-Promotion (Optional but recommended)
        // CheckAutoPromotion(CurrentTimeStep);

        // 3. Move waiting orders to cooks
        ServeOrders(CurrentTimeStep);

        // 4. Check which cooks finished their orders
        CheckFinishedOrders(CurrentTimeStep);

        // 5. Interface Update
        pGUI->ResetDrawingList();
        FillDrawingList();
        pGUI->UpdateInterface();

        // Draw the numbers/stats
        pGUI->DrawCounts(Normal_Waiting.getSize(), VIP_Waiting.getSize(),
            Vegan_Waiting.getSize(), InService.getSize(), Finished.getSize());

        // Timing logic
        if (mode == MODE_INTR) pGUI->waitForClick();
        else if (mode == MODE_STEP) Sleep(1000);

        CurrentTimeStep++;
    }

    pGUI->PrintMessage("Simulation End | Cooks: N=" + to_string(N_Cooks) + " G=" + to_string(G_Cooks) + " V=" + to_string(V_Cooks));
    cout << "Processing TS = " << CurrentTimeStep << endl;
    cout << "VIP waiting = " << VIP_Waiting.getSize() << endl;
    cout << "Normal waiting = " << Normal_Waiting.getSize() << endl;
    cout << "Vegan waiting = " << Vegan_Waiting.getSize() << endl;
    pGUI->waitForClick();

    
}
void Restaurant::ReturnCookToQueue(Cook* pCook) {
    if (pCook->GetType() == TYPE_NRM)
        AvailableNormalCooks.enqueue(pCook);
    else if (pCook->GetType() == TYPE_VGAN)
        AvailableVeganCooks.enqueue(pCook);
    else if (pCook->GetType() == TYPE_VIP)
        AvailableVIPCooks.enqueue(pCook);
}
void Restaurant::Assign(Cook* pCook, Order* pOrd, int currentTS) {
    pOrd->setStatus(SRV);
    pOrd->SetServTime(currentTS);

    // Calculate Service Time: ST = ceil(OrderSize / CookSpeed)
    // Integer formula for ceil: (a + b - 1) / b
    int serviceTime = (pOrd->getSize() + pCook->GetSpeed() - 1) / pCook->GetSpeed();
    int finishTime = currentTS + serviceTime;

    pOrd->SetFinishTime(finishTime);
    pCook->setCurrentOrder(pOrd);
    pCook->setFinishTime(finishTime); // You may need to add this setter to Cook class

    // Add to In-Service and Busy Containers
    InService.enqueue(pOrd);

    // Using negative finishTime as priority so the smallest finishTime (earliest) is at top
    BusyCooks.enqueue(pCook, -finishTime);
}


void Restaurant::AddToNormalWaiting(Order* p) {
    Normal_Waiting.enqueue(p);
    N_Orders++; // Increment the total counter
}

void Restaurant::AddToVeganWaiting(Order* p) {
    Vegan_Waiting.enqueue(p);
    G_Orders++; // Increment the total counter
}

void Restaurant::AddToVIPWaiting(Order* p) {
    int priority = (int)(p->getMoney());
    VIP_Waiting.enqueue(p, priority);
    V_Orders++; // Increment the total counter
}

void Restaurant::ServeOrders(int currentTS) {
    Order* pOrd;
    Cook* pCook;
    int pri;

    // 1. VIP Orders Priority: VIP Cook -> Normal -> Vegan
    while (VIP_Waiting.peek(pOrd, pri)) {
        if (AvailableVIPCooks.dequeue(pCook)) {}
        else if (AvailableNormalCooks.dequeue(pCook)) {}
        else if (AvailableVeganCooks.dequeue(pCook)) {}
        else break; // No cooks for VIP

        VIP_Waiting.dequeue(pOrd, pri);
        Assign(pCook, pOrd, currentTS);
    }

    // 2. Vegan Orders Priority: Vegan Cook ONLY
    while (Vegan_Waiting.peekFront(pOrd)) {
        if (AvailableVeganCooks.dequeue(pCook)) {
            Vegan_Waiting.dequeue(pOrd);
            Assign(pCook, pOrd, currentTS);
        }
        else break;
    }

    // 3. Normal Orders Priority: Normal Cook -> VIP
    while (Normal_Waiting.peekFront(pOrd)) {
        if (AvailableNormalCooks.dequeue(pCook)) {}
        else if (AvailableVIPCooks.dequeue(pCook)) {}
        else break;

        Normal_Waiting.dequeue(pOrd);
        Assign(pCook, pOrd, currentTS);
    }
}


void Restaurant::CheckAutoPromotion(int currentTS) {
    Order* pOrd;
    Queue<Order*> temp;
    while (Normal_Waiting.dequeue(pOrd)) {
        // If (CurrentTime - ArrivalTime) >= AutoP limit
        if (currentTS - pOrd->GetArrTime() >= AutoP) {
            pOrd->setType(TYPE_VIP);
            AddToVIPWaiting(pOrd); // Moves it to VIP queue
        }
        else {
            temp.enqueue(pOrd);
        }
    }
    while (temp.dequeue(pOrd)) Normal_Waiting.enqueue(pOrd);
}


void Restaurant::CheckFinishedOrders(int currentTS) {
    Cook* pCook;
    Order* pOrd;
    int pri;

    // Peek at BusyCooks (Priority Queue sorted by Finish Time)
    // We use a negative finish time or custom priority so the lowest TS is at the top
    while (BusyCooks.peek(pCook, pri)) {
        if (pCook->getFinishTime() <= currentTS) {
            BusyCooks.dequeue(pCook, pri);
            pOrd = pCook->getCurrentOrder();

            pOrd->setStatus(DONE);
            Finished.enqueue(pOrd);

            pCook->incrementOrdersServed();
            pCook->setCurrentOrder(nullptr);

            // Check if cook needs a break (BO = Break After X Orders)
            if (pCook->getOrdersServed() == BO) {
                pCook->setBreak(currentTS);
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


    for (int i = 0; i < N; i++) {
        Cook* pC = new Cook(i + 1, TYPE_NRM, SN);
        Normal_Cooks.enqueue(pC); // For drawing
        AvailableNormalCooks.enqueue(pC); // FOR LOGIC
    }
    for (int i = 0; i < G; i++) {
        Cook* pC = new Cook(i + 1, TYPE_VGAN, SG);
        Vegan_Cooks.enqueue(pC);
        AvailableVeganCooks.enqueue(pC);
    }
    for (int i = 0; i < V; i++) {
        Cook* pC = new Cook(i + 1, TYPE_VIP, SV);
        VIP_Cooks.enqueue(pC);
        AvailableVIPCooks.enqueue(pC);
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

