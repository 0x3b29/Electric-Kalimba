#include "EncoderButton.h"
#include "Adafruit_Debounce.h"
#include "EventManager.h"
#include "Menu.h"
#include <Encoder.h>

// Prepare the encoder
const int PinSW = 4;
Adafruit_Debounce encoderButton(PinSW, LOW);
long oldEncoderValue = -123456;
Encoder encoder(2, 3);

int oldEncoderDiv4Value = 0;
int lastEncoderDiv4ButtonPressedValue = 0;
unsigned long millisLastEncodeButtonPressed = 0;
bool freshButtonPressed = false;

void initializeEncoderButton()
{
    encoderButton.begin();
    pinMode(PinSW, INPUT);
    digitalWrite(PinSW, HIGH);
}

void updateEncoderButton()
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
}