#include "EventManager.h"
#include "Menu.h"
#include "Music.h"
#include "MusicNotes.h"
#include "Servo.h"

Event *headNode;
Event *nodeToDelete;

unsigned long lastEventDue = 0;
unsigned long lastOffset;

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
    }
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
        if (args[0] == NotesMenu)
        {
            currentMenu = notesMenu[0];
        }
        else if (args[0] == SongMenu)
        {
            currentMenu = songMenu1;
        }
        else if (args[0] == MainMenu)
        {
            currentMenu = mainMenu1;
        }

        printToLCD(currentMenu->caption, currentMenu->bottomNeighbour->caption, 0);
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
        if (isBuzzerEnabled)
        {
            mainMenu6->setCaption((char *)"Buzzer is Off");
        }
        else
        {
            mainMenu6->setCaption((char *)"Buzzer is On");
        }

        preparePrintMenuToLCD();
        isBuzzerEnabled = !isBuzzerEnabled;

        break;

    case PlaySong:
        if (args[0] == AgeOfEmpiresTheme)
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
        else if (args[0] == RomanticFlight)
        {
            generateEventsFromPROGMEM(romanticFlight);
        }
        break;

    case ProcessCurrentArray:
        processNotesArray();
        break;

    default:
        break;
    }
}

void createEventFromStr(char input[])
{
    char *commaPos = strchr(input, ',');
    *commaPos = 0;

    int *args = new int[1];
    args[0] = atoi(input);

    ++commaPos;

    unsigned long offset = atoi(commaPos);

    if (lastEventDue >= millis())
    {
        addEvent(new Event((lastEventDue + lastOffset), PlayNote, args));
        lastEventDue = lastEventDue + lastOffset;
        lastOffset = offset;
    }
    else
    {
        addEvent(new Event((millis() + 250), PlayNote, args));
        lastEventDue = millis() + 250;
        lastOffset = offset;
    }
}
