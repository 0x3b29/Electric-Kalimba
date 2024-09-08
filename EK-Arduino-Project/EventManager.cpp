#include "EventManager.h"
#include "Info.h"
#include "Menu.h"
#include "Music.h"
#include "MusicNotes.h"
#include "Servo.h"

Event *headNode;
Event *nodeToDelete;

unsigned long processedEventsCounter = 0;

void updateEventManager()
{
    // Process event queue
    while (headNode != NULL && headNode->getInvokeTime() < millis())
    {
        // Serial.print(headNode->getInvokeTime());
        // Serial.println(" is now due " + headNode->getEventType());

        parseEvent(headNode->getEventType(), headNode->getArguments());
        nodeToDelete = headNode;
        headNode = headNode->getNext();
        delete nodeToDelete;
        nodeToDelete = NULL;
        processedEventsCounter++;
    }
}

unsigned long getProcessedEventsCounter() { return processedEventsCounter; }

uint8_t getNextNote()
{
    Event *event = headNode;

    while (event != NULL)
    {
        if (event->getEventType() == PlayNote)
        {
            return event->getArguments()[0].intValue;
        }
        else
        {
            event = event->getNext();
        }
    }

    return NO_NOTE;
}

uint16_t getRemainingNotesInQueue()
{
    uint16_t noteEventsCounter = 0;
    Event *event = headNode;

    while (event != NULL)
    {
        if (event->getEventType() == PlayNote)
        {
            noteEventsCounter++;
        }

        event = event->getNext();
    }

    return noteEventsCounter;
}

void emptyEventQueue()
{
    Event *previousNode = NULL;
    Event *currentNode = headNode;

    while (currentNode != NULL)
    {
        previousNode = currentNode;
        currentNode = currentNode->getNext();

        delete previousNode;
    }

    headNode = NULL;
}

void addEvent(Event *newEvent)
{
    Event *previousNode = NULL;
    Event *currentNode = headNode;

    while (currentNode != NULL && newEvent->getInvokeTime() > currentNode->getInvokeTime())
    {
        previousNode = currentNode;
        currentNode = currentNode->getNext();
    }

    if (headNode == NULL)
    {
        // Serial.println("Event was first");

        headNode = newEvent;
    }
    else if (currentNode == NULL)
    {
        // Serial.println("Event was last");

        newEvent->setPrevious(previousNode);
        previousNode->setNext(newEvent);
    }
    else
    {
        if (currentNode == headNode)
        {
            // Serial.print("Event will be new head");

            headNode->setPrevious(newEvent);
            newEvent->setNext(headNode);

            headNode = newEvent;
        }
        else
        {
            // Serial.print("Event was inbetween ");
            // Serial.print(currentNode->getPrevious()->getInvokeTime());
            // Serial.print(" and ");
            // Serial.println(currentNode->getInvokeTime());

            newEvent->setPrevious(currentNode->getPrevious());
            newEvent->setNext(currentNode);

            currentNode->getPrevious()->setNext(newEvent);
            currentNode->setPrevious(newEvent);
        }
    }
}

bool hasSetServoPosition(int board, int servo)
{
    Event *event = headNode;

    while (event != NULL)
    {
        if (event->getEventType() == SetServoPosition)
        {
            EventArg *eventArgs = event->getArguments();

            if (eventArgs[0].intValue == board && eventArgs[1].intValue == servo)
            {
                return true;
            }
        }

        event = event->getNext();
    }

    return false;
}

void replaceSetServoPosition(int board, int servo, int newPosition)
{
    Event *event = headNode;

    while (event != NULL)
    {
        if (event->getEventType() == SetServoPosition)
        {
            EventArg *eventArgs = event->getArguments();

            if (eventArgs[0].intValue == board && eventArgs[1].intValue == servo)
            {
                eventArgs[2].intValue = newPosition;
            }
        }

        event = event->getNext();
    }
}

void generateEventsFromPROGMEM(const char *startChar, unsigned long eventInvokeTime)
{
    char c;
    const char *currentChar = startChar;

    char buffer[10];
    int bufferIndex = 0;

    int processedEventsCounter = 0;
    const int maxEventCounter = 100;

    unsigned long nextEventInvokeTime = eventInvokeTime;

    while (processedEventsCounter < maxEventCounter)
    {
        c = pgm_read_byte(currentChar);

        if (c == NULL)
        {
            break;
        }

        if (c == ';')
        {
            buffer[bufferIndex] = '\0';
            nextEventInvokeTime = createEventFromStr(buffer, nextEventInvokeTime);
            processedEventsCounter++;
            bufferIndex = 0;
        }
        else
        {
            buffer[bufferIndex] = c;
            bufferIndex++;
        }

        currentChar++;
    }

    if (processedEventsCounter >= maxEventCounter)
    {
        EventArg *eventArgs = new EventArg[1]{EventArg(currentChar)};
        addEvent(new Event(nextEventInvokeTime, BufferMoreNotes, eventArgs));
    }
}

void parseEvent(EventType eventType, EventArg *eventArgs)
{
    switch (eventType)
    {
    case SetServoPosition:
        setServoPosition(eventArgs[0].intValue, eventArgs[1].intValue, eventArgs[2].intValue);
        break;

    case PlayNote:
        decreaseRemainingNotesCounter();
        playNote(eventArgs[0].intValue);
        break;

    case SetMenu:
        setMenu(eventArgs[0].intValue);
        break;

    case Init:
        if (eventArgs[0].intValue == Up)
        {
            moveAllServosUp();
        }
        else if (eventArgs[0].intValue == Down)
        {
            moveAllServosDown();
        }
        else if (eventArgs[0].intValue == Center)
        {
            moveAllServosCenter();
        }
        break;

    case ToggleBuzzer:
        toggleBuzzer();
        break;

    case PlaySong:
        playSong(eventArgs[0].intValue);
        break;

    case BufferMoreNotes:
        generateEventsFromPROGMEM(eventArgs[0].ptrValue, millis());
        break;

    case ProcessCurrentArray:
        processNotesArray();
        break;

    case EmptyQueue:
        emptyEventQueue();
        break;

    default:
        break;
    }
}

unsigned long createEventFromStr(char input[], unsigned long eventInvokeTime)
{
    // Search for position of ','
    char *commaPos = strchr(input, ',');

    // Replace ',' with string termination
    *commaPos = 0;

    EventArg *eventArgs = new EventArg[1]{EventArg(getNoteValue(input))};
    addEvent(new Event(eventInvokeTime, PlayNote, eventArgs));

    // Shift the pointer from the Null terminator to start of the notes offset
    ++commaPos;

    // Parse the notes delay before the next note gets played
    int offsetBeforeNextEvent = atoi(commaPos);

    // Return the time the following note event should be invoked
    return eventInvokeTime + offsetBeforeNextEvent;
}