#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H
#include "Event.h"

extern Event *headNode;
extern Event *nodeToDelete;

void addEvent(Event *newEvent);
void parseEvent(EventType what, int *args);
void generateEventsFromPROGMEM(const char *startChar);
void createEventFromStr(char input[]);

extern unsigned long lastEventDue;
extern unsigned long lastOffset;

#endif