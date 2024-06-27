
#include "Adafruit_Debounce.h"
#include "Enums.h"
#include "EventManager.h"
#include "Memory.h"
#include "Menu.h"
#include "Music.h"
#include "Servo.h"
#include <Encoder.h>

int bufferPosition;
char serialInputBuffer[100];

// Prepare the encoder
const int PinSW = 4;
Adafruit_Debounce encoderButton(PinSW, LOW);
long oldEncoderValue = -123456;
Encoder encoder(2, 3);

// Keep global encoder state
int oldEncoderDiv4Value = 0;
int lastEncoderDiv4ButtonPressedValue = 0;
unsigned long millisLastEncodeButtonPressed = 0;
bool freshButtonPressed = false;

void setup()
{
    Serial.begin(115200);
    encoderButton.begin();

    initializeServos();

    // Define the switch on the rotary encoder
    pinMode(PinSW, INPUT);
    digitalWrite(PinSW, HIGH);

    initializeLcd();
    initializeMenu();

    // Move all servos to defined positions
    Serial.println("Initialize Up");
    moveAllServosUp();

    Serial.setTimeout(5000);
}

void loop()
{
    encoderButton.update();

    // Check if rotary encoder got turned
    long encoderValue = encoder.read();
    int newEncoderDiv4Value = encoderValue / 4;

    if (encoderValue != oldEncoderValue)
    {
        oldEncoderValue = encoderValue;

        if (newEncoderDiv4Value != oldEncoderDiv4Value)
        {
            Serial.print("New Value: ");
            Serial.println(newEncoderDiv4Value);

            bool directionUp;

            if (newEncoderDiv4Value > oldEncoderDiv4Value)
            {
                Serial.println("Scroll up");

                directionUp = true;
                currentMenu = currentMenu->topNeighbour;
            }
            else
            {
                Serial.println("Scroll down");

                directionUp = false;
                currentMenu = currentMenu->bottomNeighbour;
            }

            if (directionUp)
            {
                wasLastdirectionUp = true;
            }
            else
            {
                wasLastdirectionUp = false;
            }

            preparePrintMenuToLCD();

            oldEncoderDiv4Value = newEncoderDiv4Value;
        }
    }

    // Check if rotary encoder got pressed
    // Also check if encoder state has changed which is important for selecting multiple things in a row (e.g. different notes)
    if (encoderButton.justPressed())
    {
        freshButtonPressed = true;
        millisLastEncodeButtonPressed = millis();
        lastEncoderDiv4ButtonPressedValue = newEncoderDiv4Value;
    }

    unsigned long delta = millis() - millisLastEncodeButtonPressed;

    bool hasNewClick = false;
    if (freshButtonPressed && encoderButton.isPressed() && delta > 100)
    {
        freshButtonPressed = false;
        hasNewClick = true;
    }

    if (hasNewClick || (encoderButton.isPressed() && newEncoderDiv4Value != lastEncoderDiv4ButtonPressedValue))
    {
        if (encoderButton.isPressed() && newEncoderDiv4Value != lastEncoderDiv4ButtonPressedValue)
        {
            Serial.print("Retrigger ");
            Serial.println(currentMenu->eventType);
        }

        if (hasNewClick)
        {
            Serial.print("Trigger ");
            Serial.println(currentMenu->eventType);
        }

        lastEncoderDiv4ButtonPressedValue = newEncoderDiv4Value;

        parseEvent(currentMenu->eventType, currentMenu->args);
    }

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
            for (int i = 1; i <= 17; i++)
            {
                int *args = new int[1];
                args[0] = i;
                addEvent(new Event(millis() + (i * 250), PlayNote, args));
            }
        }
        else if (serialInputBuffer[0] == 'b')
        {
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
