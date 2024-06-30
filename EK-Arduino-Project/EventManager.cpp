#include "EventManager.h"
#include "Info.h"
#include "Menu.h"
#include "Music.h"
#include "MusicNotes.h"
#include "Servo.h"

Event *headNode;
Event *nodeToDelete;

unsigned long lastEventDue = 0;
unsigned long lastOffset;
unsigned long eventCounter = 0;

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
        eventCounter++;
    }
}

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

uint16_t getRemainingNotesCount()
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
    lastEventDue = 0;
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

void generateEventsFromPROGMEM(const char *startChar)
{
    char c;
    char *currentChar = startChar;

    char buffer[10];
    int bufferIndex = 0;

    int eventCounter = 0;
    const int maxEventCounter = 100;

    while (eventCounter < maxEventCounter)
    {
        c = pgm_read_byte(currentChar);

        if (c == NULL)
        {
            break;
        }

        if (c == ';')
        {
            buffer[bufferIndex] = '\0';
            createEventFromStr(buffer);
            eventCounter++;
            bufferIndex = 0;
        }
        else
        {
            buffer[bufferIndex] = c;
            bufferIndex++;
        }

        currentChar++;
    }

    if (eventCounter >= maxEventCounter)
    {
        Serial.println("Found more events than maxEventCounter, added Buffer operation.");
        EventArg *eventArgs = new EventArg[1]{EventArg(currentChar)};
        addEvent(new Event(lastEventDue + lastOffset, BufferMoreNotes, eventArgs));
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
        isBuzzerEnabled = !isBuzzerEnabled;
        setBuzzerMenuItem(isBuzzerEnabled);
        preparePrintMenuToLCD();
        break;

    case PlaySong:
        if (eventArgs[0].intValue == Stairs)
        {
            playStairs();
        }
        else if (eventArgs[0].intValue == AgeOfEmpiresTheme)
        {
            generateEventsFromPROGMEM(ageOfEmpires);
        }
        else if (eventArgs[0].intValue == LuxembourgAnthem)
        {
            currentArrayPosition = 0;
            currentArraySize = luxembourgAnthemSize;
            currentNotesArray = luxembourgAnthemNotes;
            currentOffsetsArray = luxembourgAnthemOffsets;

            processNotesArray();
        }
        else if (eventArgs[0].intValue == PipiTheme)
        {
            generateEventsFromPROGMEM(pipiTheme);
        }
        else if (eventArgs[0].intValue == PommerscheTheme)
        {
            generateEventsFromPROGMEM(pommerscheTheme);
        }
        else if (eventArgs[0].intValue == HttydImpro)
        {
            generateEventsFromPROGMEM(httydImpro);
        }
        else if (eventArgs[0].intValue == HttydYt)
        {
            generateEventsFromPROGMEM(httydYt);
        }
        else if (eventArgs[0].intValue == InterstellardYt)
        {
            generateEventsFromPROGMEM(interstellarYt);
        }
        else
        {
            Serial.println("Tried to play undefined song!");
        }
        break;

    case BufferMoreNotes:
        generateEventsFromPROGMEM(eventArgs[0].ptrValue);
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

void createEventFromStr(char input[])
{
    // Search for position of ','
    char *commaPos = strchr(input, ',');

    // Replace ',' with string termination
    *commaPos = 0;

    EventArg *eventArgs = new EventArg[1]{EventArg(atoi(input))};

    // Shift the pointer from the Null terminator to start of the notes offset
    ++commaPos;

    // Parse the notes offset
    int offset = atoi(commaPos);

    if (lastEventDue >= millis())
    {
        // Last event has not yet happened
        addEvent(new Event((lastEventDue + lastOffset), PlayNote, eventArgs));
        lastEventDue = lastEventDue + lastOffset;
        lastOffset = offset;
    }
    else
    {
        // Last event is over
        addEvent(new Event((millis()), PlayNote, eventArgs));
        lastEventDue = millis();
        lastOffset = offset;
    }
}