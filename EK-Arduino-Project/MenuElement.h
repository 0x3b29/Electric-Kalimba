#include "MyEnums.h"
class MenuElement
{
    public: 
    MenuElement(char * caption, EventType eventType, int arg)
    {
        this->caption = caption;
        this->eventType = eventType;

        args = new int [1]; 
        args[0] = arg;
    }
    const char * caption;
    EventType eventType;
    int * args;

    MenuElement * topNeighbour;
    MenuElement * bottomNeighbour;

    void setCaption(char * newCaption)
    {
        this->caption = newCaption;
    }
};