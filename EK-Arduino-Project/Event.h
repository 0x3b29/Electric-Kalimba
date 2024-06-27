#ifndef EVENT_H
#define EVENT_H

#include "Arduino.h"
#include "Enums.h"
class Event
{
    private:
    unsigned long invokeTime;
    
    EventType eventType;
    int * args;

    Event* next;
    Event* previous;

    public: 
    Event(long invokeTime, EventType eventType, int * args)
    {
        this->invokeTime = invokeTime;
        this->eventType = eventType;
        this->args = args;
        //  Serial.print("I am due at: '");
        //  Serial.print(invokeTime);
        //  Serial.println("'");

        next = NULL;
        previous = NULL;
    }

    ~Event()
    {
        // The args array is created with "new" to be persistent. 
        // Not calling delete would therefore result in a memory leak
        delete[] args;
    }

    void setNext(Event* next)
    {
        this->next = next;
    }

    Event* getNext()
    {
        return this->next; 
    }

    void setPrevious(Event* previous)
    {
        this->previous = previous;
    }

    Event* getPrevious()
    {
        return this->previous; 
    }

    unsigned long getInvokeTime()
    {
        return this->invokeTime;
    }

    EventType getEventType()
    {
        return this->eventType;
    }

    int* getArguments()
    {
        return this->args;
    }
};

#endif