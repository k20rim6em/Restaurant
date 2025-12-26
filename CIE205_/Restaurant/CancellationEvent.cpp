#include "CancellationEvent.h"
#include "Rest/Restaurant.h"
#include <iostream>
using namespace std;

CancellationEvent::CancellationEvent(int eTime, int oID) : Event(eTime, oID)
{
}

void CancellationEvent::Execute(Restaurant* pRest)
{
    bool cancelled = pRest->CancelOrder(OrderID);

    if (cancelled) {
        cout << "Cancellation Event Executed: Order " << OrderID << " cancelled" << endl;
    }
    else {
        cout << "Cancellation Event Executed: Order " << OrderID << " not found" << endl;
    }
}
