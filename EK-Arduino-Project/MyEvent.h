#include "Enums.h"

class MyEvent
{
    private:
    unsigned long when;
    
    EventType what;
    int * args;

    MyEvent* next;
    MyEvent* previous;

    public: 
    MyEvent(long when, EventType what, int * args)
    {
        this->when = when;
        this->what = what;
        this->args = args;
        //  Serial.print("I am due at: '");
        //  Serial.print(when);
        //  Serial.println("'");

        next = NULL;
        previous = NULL;
    }

    ~MyEvent()
    {
        // The args array is created with "new" to be persistent. 
        // Not calling delete would therefore result in a memory leak
        delete[] args;
    }

    void setNext(MyEvent* next)
    {
        this->next = next;
    }

    MyEvent* getNext()
    {
        return this->next; 
    }

    void setPrevious(MyEvent* previous)
    {
        this->previous = previous;
    }

    MyEvent* getPrevious()
    {
        return this->previous; 
    }

    unsigned long getWhen()
    {
        return this->when;
    }

    EventType getWhat()
    {
        return this->what;
    }

    int* getArguments()
    {
        return this->args;
    }
};