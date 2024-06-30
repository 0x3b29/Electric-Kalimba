#include "Enums.h"
#include "Event.h"

class MenuElement
{
  public:
    MenuElement(char *caption, EventType eventType, EventArg *eventArgs)
    {
        this->caption = caption;
        this->eventType = eventType;
        this->eventArgs = eventArgs;
    }

    const char *caption;
    EventType eventType;
    EventArg *eventArgs;

    MenuElement *topNeighbour;
    MenuElement *bottomNeighbour;

    void setCaption(char *newCaption) { this->caption = newCaption; }
};