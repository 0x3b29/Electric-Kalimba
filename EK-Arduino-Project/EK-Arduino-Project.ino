
#include "EK-Arduino-Project.h"
#include "EncoderButton.h"
#include "Enums.h"
#include "EventManager.h"
#include "Info.h"
#include "Memory.h"
#include "Menu.h"
#include "Music.h"
#include "Servo.h"

int bufferPosition;
char serialInputBuffer[100];
unsigned long nextEventInvokeTime = 0;

void setup()
{
    Serial.begin(115200);

    // Prototype 2 has no display, no encoder button and therefore needs no menu.
    // It is designed as a robot that always has a serial connection.
#if PROTOTYPE1
    initializeEncoderButton();
    initializeLcd();
    initializeMenu();
#endif

    initializeServos();
    moveAllServosUp();

    Serial.println("Initialisation Complete...");
}

void loop()
{
    updateEncoderButton();
    updateEventManager();
    updateInfo();

    bool didReadSomething = false;
    bool isValidCommand = false;
    char serialChar;

    // Process serial input
    if (Serial.available())
    {
        char serialChar = Serial.read();

        if (serialChar > 0)
        {
            didReadSomething = true;
        }

        if (serialChar == ';')
        {
            isValidCommand = true;
            serialInputBuffer[bufferPosition] = '\0';
        }
        else if (serialChar > 40 && serialChar < 127)
        {
            serialInputBuffer[bufferPosition] = serialChar;
            bufferPosition++;
        }
    }

    if (didReadSomething == false)
    {
        return;
    }

    if (didReadSomething == true && isValidCommand == false)
    {
        return;
    }

    /*
    // Note: if these prints are active, reading large data chunks will result in input buffer overflows
    Serial.print("Good: '");
    Serial.print(serialInputBuffer);
    Serial.print("', bufferPosition: ");
    Serial.println (bufferPosition);
    Serial.println (serialInputBuffer[0]);
    */

    char *commaPos = strchr(serialInputBuffer, ',');

    if (nextEventInvokeTime < millis())
    {
        nextEventInvokeTime = millis();
    }

    if (commaPos != NULL)
    {
        nextEventInvokeTime = createEventFromStr(serialInputBuffer, nextEventInvokeTime);
    }
    else
    {
        if (serialInputBuffer[0] == 'c')
        {
            Serial.println("Centering ...");
            moveAllServosCenter();
        }
        else if (serialInputBuffer[0] == 'u')
        {
            Serial.println("Upping ...");
            moveAllServosUp();
        }
        else if (serialInputBuffer[0] == 'd')
        {
            Serial.println("Downing ...");
            moveAllServosDown();
        }
        else if (serialInputBuffer[0] == 'x')
        {
            Serial.println("Xing ...");
            playStairs();
        }
        else if (serialInputBuffer[0] == 'a')
        {
            adjustServoPosition(serialInputBuffer[1], serialInputBuffer[2], serialInputBuffer[3], serialInputBuffer[4]);
        }
        else if (serialInputBuffer[0] == 'p')
        {
            printServoPositions();
        }
        else if (serialInputBuffer[0] == 'b')
        {
            toggleBuzzer();
        }
        else if (serialInputBuffer[0] == 'm')
        {
            freeMemory();
        }
        else if (serialInputBuffer[0] == 's')
        {
            uint8_t note = getNoteValue(&serialInputBuffer[1]);

            if (note != NO_NOTE)
            {
                muteNote(note);
            }
        }
        else
        {
            uint8_t note = getNoteValue(serialInputBuffer);

            if (note != NO_NOTE)
            {
                playNote(note);
            }
        }
    }

    bufferPosition = 0;
    serialInputBuffer[0] = 0;
}
