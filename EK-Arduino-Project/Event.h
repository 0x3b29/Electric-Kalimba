#ifndef EVENT_H
#define EVENT_H

#include "Arduino.h"
#include "Enums.h"

class EventArg
{
  public:
    enum Type
    {
        INT,
        PTR
    } type;

    union
    {
        int intValue;
        const char *ptrValue;
    };

    EventArg(int intValue) : type(INT), intValue(intValue) {}
    EventArg(const char *ptrValue) : type(PTR), ptrValue(ptrValue) {}

    bool isInt() const { return type == INT; }
    bool isPtr() const { return type == PTR; }
};

class Event
{
  private:
    unsigned long invokeTime;

    EventType eventType;
    EventArg *eventArgs;

    Event *next;
    Event *previous;

  public:
    Event(long invokeTime, EventType eventType, EventArg *eventArgs)
    {
        this->invokeTime = invokeTime;
        this->eventType = eventType;
        this->eventArgs = eventArgs;

        next = NULL;
        previous = NULL;
    }

    ~Event() { delete[] eventArgs; }

    void setNext(Event *next) { this->next = next; }

    Event *getNext() { return this->next; }

    void setPrevious(Event *previous) { this->previous = previous; }

    Event *getPrevious() { return this->previous; }

    unsigned long getInvokeTime() { return this->invokeTime; }

    EventType getEventType() { return this->eventType; }

    EventArg *getArguments() { return this->eventArgs; }
};

#endif