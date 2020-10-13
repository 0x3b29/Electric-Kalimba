class MyEvent
{
  private:
  unsigned long when;
  String what;

  MyEvent* next;
  
  public: 
  MyEvent(long when, String what)
  {
    this->when = when;
    this->what = what;

    Serial.print("I am due at: '");
    Serial.print(when);
    Serial.println("'");
  }

  void setNext(MyEvent* next)
  {
    this->next = next;
  }

  MyEvent* getNext()
  {
    return this->next; 
  }

  unsigned long getWhen()
  {
    return this->when;
  }
};
