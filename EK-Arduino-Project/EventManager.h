#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H
#include "Event.h"

extern Event *headNode;
extern Event *nodeToDelete;

void updateEventManager();

void addEvent(Event *newEvent);
void parseEvent(EventType what, EventArg *eventArgs);
void generateEventsFromPROGMEM(const char *startChar);
void createEventFromStr(char input[]);

uint8_t getNextNote();
uint16_t getRemainingNotesCount();

extern unsigned long lastEventDue;
extern unsigned long lastOffset;
extern unsigned long eventCounter;

#endif