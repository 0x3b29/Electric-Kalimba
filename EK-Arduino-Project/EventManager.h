#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H
#include "Event.h"

extern Event *headNode;
extern Event *nodeToDelete;

void updateEventManager();

void addEvent(Event *newEvent);
void parseEvent(EventType eventType, EventArg *eventArgs);
void generateEventsFromPROGMEM(const char *startChar, unsigned long eventInvokeTime);
unsigned long createEventFromStr(char input[], unsigned long eventInvokeTime);

uint8_t getNextNote();
uint16_t getRemainingNotesInQueue();
unsigned long getProcessedEventsCounter();

bool hasSetServoPosition(int board, int servo);
void replaceSetServoPosition(int board, int servo, int newPosition);

#endif