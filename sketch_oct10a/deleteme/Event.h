class Event
{
  private:
  unsigned long when;
  String what;

  Event* next;
  
  public: 
  Event(long when, String what)
  {
    this->when = when;
    this->what = what;

    Serial.print("I am due at: '");
    Serial.print(when);
    Serial.println("'");
  }

  void setNext(Event* next)
  {
    this->next = next;
  }

  Event* getNext()
  {
    return this->next; 
  }

  unsigned long getWhen()
  {
    return this->when;
  }
};
