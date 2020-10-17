class MyEvent
{
    private:
    unsigned long when;
    String what;

    MyEvent* next;
    MyEvent* previous;

    public: 
    MyEvent(long when, String what)
    {
        this->when = when;
        this->what = what;

        //  Serial.print("I am due at: '");
        //  Serial.print(when);
        //  Serial.println("'");

        next = NULL;
        previous = NULL;
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

    String getWhat()
    {
        return this->what;
    }
};