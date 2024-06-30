#include "Servo.h"
#include "Event.h"
#include "EventManager.h"
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver leftServoBoard = Adafruit_PWMServoDriver(0x40);  // Create an object of board 1
Adafruit_PWMServoDriver rightServoBoard = Adafruit_PWMServoDriver(0x41); // Create an object of board 2

int servoMin = 150;      // This is the servos minimum pulse length count (out of 4096)
int servoMax = 600;      // This is the servos maximum pulse length count (out of 4096)
int servoFrequency = 50; // Servo update frequency, analog servos typically run at ~50 Hz

int servoTravelTime = 150;
int servoRelaxAmount = 20;

// Left side servos work as expected, lower numbers = lower down
int leftServosUpPositions[9] = {105, 100, 100, 100, 105, 105, 105, 105, 105};
int board1ServoCenterPos[9] = {75, 75, 75, 80, 80, 80, 80, 80, 80};
int leftServosDownPositions[9] = {40, 40, 40, 42, 43, 44, 45, 45, 45};

// Right side servos work in opposite directions, bigger numbers = lower down
int rightServosDownPositions[8] = {125, 120, 125, 127, 129, 131, 133, 135};
int board2ServoCenterPos[8] = {90, 85, 92, 95, 96, 97, 98, 99};
int rightServosUpPositions[8] = {63, 58, 69, 70, 71, 67, 71, 71};

bool leftServosUp[9];
bool rightServosUp[8];

void initializeServos()
{
    // Start each board
    leftServoBoard.begin();
    rightServoBoard.begin();

    // Set the PWM oscillator frequency, used for fine calibration
    leftServoBoard.setOscillatorFrequency(27000000);
    rightServoBoard.setOscillatorFrequency(27000000);

    // Set the servo operating frequency
    leftServoBoard.setPWMFreq(servoFrequency);
    rightServoBoard.setPWMFreq(servoFrequency);
}

void setServoPosition(int board, int servo, int position)
{
    if (board == 1)
    {
        leftServoBoard.setPWM(servo, 0, position);
    }
    else
    {
        rightServoBoard.setPWM(servo, 0, position);
    }
}

void moveServoDown(int board, int servo)
{
    int servoDownPosition = 0;
    int servoRelaxAmountToAdd = 0;

    if (board == 1)
    {
        servoDownPosition = leftServosDownPositions[servo];
        servoRelaxAmountToAdd = servoRelaxAmount;
        leftServosUp[servo] = false;
    }
    else
    {
        servoDownPosition = rightServosDownPositions[servo];
        servoRelaxAmountToAdd = -servoRelaxAmount;
        rightServosUp[servo] = false;
    }

    setServoPosition(board, servo, map(servoDownPosition, 0, 180, servoMin, servoMax));

    EventArg *eventArgs =
        new EventArg[3]{EventArg(board), EventArg(servo), EventArg(map(servoDownPosition + servoRelaxAmountToAdd, 0, 180, servoMin, servoMax))};

    addEvent(new Event(millis() + servoTravelTime, SetServoPosition, eventArgs));
}

void moveServoUp(int board, int servo)
{
    int servoUpPosition = 0;
    int servoRelaxAmountToAdd = 0;

    if (board == 1)
    {
        servoUpPosition = leftServosUpPositions[servo];
        servoRelaxAmountToAdd = -servoRelaxAmount;
        leftServosUp[servo] = true;
    }
    else
    {
        servoUpPosition = rightServosUpPositions[servo];
        servoRelaxAmountToAdd = +servoRelaxAmount;
        rightServosUp[servo] = true;
    }

    setServoPosition(board, servo, map(servoUpPosition, 0, 180, servoMin, servoMax));

    EventArg *eventArgs = new EventArg[3]{EventArg(board), EventArg(servo), EventArg(map(servoUpPosition + servoRelaxAmountToAdd, 0, 180, servoMin, servoMax))};

    addEvent(new Event(millis() + servoTravelTime, SetServoPosition, eventArgs));
}

void toggleServo(int board, int servo)
{
    if (board == 1)
    {
        if (leftServosUp[servo] == true)
        {
            moveServoDown(board, servo);
        }
        else
        {
            moveServoUp(board, servo);
        }
    }
    else
    {
        if (rightServosUp[servo] == true)
        {
            moveServoDown(board, servo);
        }
        else
        {
            moveServoUp(board, servo);
        }
    }
}

void moveAllServosCenter()
{
    for (int i = 0; i <= 8; i++)
    {
        leftServoBoard.setPWM(i, 0, map(board1ServoCenterPos[i], 0, 180, servoMin, servoMax));
    }

    for (int i = 0; i <= 7; i++)
    {
        rightServoBoard.setPWM(i, 0, map(board2ServoCenterPos[i], 0, 180, servoMin, servoMax));
    }
}

void moveAllServosUp()
{
    for (int i = 0; i <= 8; i++)
    {
        moveServoUp(1, i);
    }

    for (int i = 0; i <= 7; i++)
    {
        moveServoUp(2, i);
    }
}

void moveAllServosDown()
{
    for (int i = 0; i <= 8; i++)
    {
        moveServoDown(1, i);
    }

    for (int i = 0; i <= 7; i++)
    {
        moveServoDown(2, i);
    }
}