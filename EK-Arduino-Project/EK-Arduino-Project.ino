
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

void setup()
{
    Serial.begin(115200);
    Serial.setTimeout(5000);

    initializeEncoderButton();
    initializeServos();
    initializeLcd();
    initializeMenu();

    // Move all servos to defined positions
    Serial.println("Initialize Up");
    moveAllServosUp();
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

    if (commaPos != NULL)
    {
        createEventFromStr(serialInputBuffer);
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
        else if (serialInputBuffer[0] == 'b')
        {
            isBuzzerEnabled = !isBuzzerEnabled;
            setBuzzerMenuItem(isBuzzerEnabled);
            preparePrintMenuToLCD();
        }
        else if (serialInputBuffer[0] == 'm')
        {
            freeMemory();
        }
        else
        {
            int val = atoi(serialInputBuffer);

            if (val > 0)
            {
                playNote(val);
            }
        }
    }

    bufferPosition = 0;
    serialInputBuffer[0] = 0;
}
