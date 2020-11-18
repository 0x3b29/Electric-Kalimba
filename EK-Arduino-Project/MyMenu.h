#include "MyEnums.h"
class MyMenu
{
    public: 
    MyMenu(char * caption, EventType what, int arg)
    {
        this->caption = caption;
        this->what = what;

        args = new int [1]; 
        args[0] = arg;
    }
    const char * caption;
    EventType what;
    int * args;

    MyMenu * topNeighbour;
    MyMenu * bottomNeighbour;

    void setCaption(char * newCaption)
    {
        this->caption = newCaption;
    }
};