#include "ArrivalEvent.h"
#include "..\Rest\Restaurant.h"
#include "../Pri_Queue.h"
#include <iostream>
using namespace std;


ArrivalEvent::ArrivalEvent(int eTime,ORD_TYPE oType ,int oID,int distance,int Ordsize, double money):Event(eTime, oID)
{
	
	ordSize = Ordsize;
	OrdDistance =distance; 
	OrdType = oType;
	OrdMoney = money;
}



void ArrivalEvent::Execute(Restaurant* pRest)
{
    Order* pOrd = new Order(OrderID, OrdType, ordSize, OrdMoney);
    pOrd->SetDistance(OrdDistance);
    pOrd->setMoney(OrdMoney);
    pOrd->setArrTime(EventTime);
    pOrd->setStatus(WAIT);

    cout << "-Arrival Event Executed: Order " << OrderID << " arrived (";

    if (OrdType == TYPE_NRM) cout << "N";
    else if (OrdType == TYPE_VGAN) cout << "G";
    else cout << "V";

    cout << ", size=" << ordSize << ", money=" << OrdMoney << ")" << endl;

    if (OrdType == TYPE_NRM)
    {
        pRest->AddToNormalWaiting(pOrd);
		pOrd->setType(TYPE_NRM);
    }
    else if (OrdType == TYPE_VGAN)
    {
        pRest->AddToVeganWaiting(pOrd);
        pOrd->setType(TYPE_VGAN);
    }
    else
    {
        pRest->AddToVIPWaiting(pOrd);
        pOrd->setType(TYPE_VIP);
    }

}
