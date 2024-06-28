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
            return event->getArguments()[0];
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
    const char *currentChar = startChar;

    char buffer[10];
    int bufferIndex = 0;

    while ((c = pgm_read_byte(currentChar++)))
    {
        if (c == ';')
        {
            buffer[bufferIndex] = '\0';
            createEventFromStr(buffer);
            bufferIndex = 0;
        }
        else
        {
            buffer[bufferIndex] = c;
            bufferIndex++;
        }
    }
}

void parseEvent(EventType eventType, int *args)
{
    switch (eventType)
    {
    case SetServoPosition:
        setServoPosition(args[0], args[1], args[2]);
        break;

    case PlayNote:
        playNote(args[0]);
        break;

    case SetMenu:
        setMenu(args[0]);
        break;

    case Init:
        if (args[0] == Up)
        {
            moveAllServosUp();
        }
        else if (args[0] == Down)
        {
            moveAllServosDown();
        }
        else if (args[0] == Center)
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
        if (args[0] == Stairs)
        {
            playStairs();
        }
        else if (args[0] == AgeOfEmpiresTheme)
        {
            generateEventsFromPROGMEM(ageOfEmpires);
        }
        else if (args[0] == LuxembourgAnthem)
        {
            currentArrayPosition = 0;
            currentArraySize = luxembourgAnthemSize;
            currentNotesArray = luxembourgAnthemNotes;
            currentOffsetsArray = luxembourgAnthemOffsets;

            processNotesArray();
        }
        else if (args[0] == PipiTheme)
        {
            generateEventsFromPROGMEM(pipiTheme);
        }
        else if (args[0] == PommerscheTheme)
        {
            generateEventsFromPROGMEM(pommerscheTheme);
        }
        else if (args[0] == HttydImpro)
        {
            generateEventsFromPROGMEM(httydImpro);
        }
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

    // Create int array to store all args inside (here only one)
    int *args = new int[1];
    args[0] = atoi(input);

    // Shift the pointer from the Null terminator to start of the notes offset
    ++commaPos;

    // Parse the notes offset
    unsigned long offset = atoi(commaPos);

    if (lastEventDue >= millis())
    {
        // Last event has not yet happened
        addEvent(new Event((lastEventDue + lastOffset), PlayNote, args));
        lastEventDue = lastEventDue + lastOffset;
        lastOffset = offset;
    }
    else
    {
        // Last event is over
        addEvent(new Event((millis() + 250), PlayNote, args));
        lastEventDue = millis() + 250;
        lastOffset = offset;
    }
}
