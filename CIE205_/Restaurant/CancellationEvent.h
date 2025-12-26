#ifndef __CANCEL_EVENT_H_
#define __CANCEL_EVENT_H_

#include "Events/Event.h"

class CancellationEvent : public Event
{
public:
    CancellationEvent(int eTime, int oID);
    virtual void Execute(Restaurant* pRest);
};

#endif


