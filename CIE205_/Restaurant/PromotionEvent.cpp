#include "PromotionEvent.h"
#include "Rest/Restaurant.h"
#include <iostream>
using namespace std;

PromotionEvent::PromotionEvent(int eTime, int oID, double extra): Event(eTime, oID)
{
    ExtraMoney = extra;
}

void PromotionEvent::Execute(Restaurant* pRest)
{
    pRest->PromoteOrder(OrderID, ExtraMoney);

    cout << "Promotion Event Executed: Order " << OrderID << " promoted (extra money = " << ExtraMoney << ")" << endl;

}
