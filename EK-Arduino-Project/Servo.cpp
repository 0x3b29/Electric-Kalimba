#include "Servo.h"
#include "EK-Arduino-Project.h"
#include "Event.h"
#include "EventManager.h"
#include <Adafruit_PWMServoDriver.h>

// Forward declarations
void moveServoDown(int board, int servo);
void moveServoUp(int board, int servo);

// Create both servo contoller objects
Adafruit_PWMServoDriver leftServoBoard = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver rightServoBoard = Adafruit_PWMServoDriver(0x41);

// This delay is used to minimize stress on the hardware when initializing all notes at once
const int initializeServoDelay = 20;
const int initializeServoFinalDelay = 100;

// This is the servos minimum pulse length count (out of 4096)
int servoMin = 150;

// This is the servos maximum pulse length count (out of 4096)
int servoMax = 600;

// Servo update frequency, analog servos typically run at ~50 Hz
int servoFrequency = 50;

int servoTravelTime = 100;
int servoRelaxAmount = 20;

// Left side servos work as expected, lower numbers = lower down
// Left numbers are outer servos, towards the right -> towards the center
#if PROTOTYPE1
int leftServosUpPositions[9] = {122, 116, 110, 114, 114, 110, 108, 104, 96};
int leftServosCenterPositions[9] = {92, 86, 80, 84, 84, 80, 78, 74, 66};
int leftServosDownPositions[9] = {62, 56, 50, 54, 54, 50, 48, 44, 36};
#else
int leftServosUpPositions[9] = {108, 107, 105, 111, 101, 106, 104, 109, 111};
int leftServosCenterPositions[9] = {78, 77, 75, 81, 71, 76, 74, 79, 81};
int leftServosDownPositions[9] = {48, 47, 45, 51, 41, 46, 44, 49, 51};
#endif

// Right side servos work in opposite directions, bigger numbers = lower down
// Left numbers are inner servos, towards the right -> towards outside
#if PROTOTYPE1
int rightServosUpPositions[9] = {72, 64, 70, 78, 74, 70, 70, 72};
int rightServosCenterPositions[9] = {102, 94, 100, 108, 104, 100, 100, 102};
int rightServosDownPositions[9] = {132, 124, 130, 138, 134, 130, 130, 132};
#else
int rightServosUpPositions[9] = {34, 32, 36, 20, 36, 27, 27, 34};
int rightServosCenterPositions[9] = {64, 62, 66, 50, 66, 57, 57, 64};
int rightServosDownPositions[9] = {94, 92, 96, 80, 96, 87, 87, 94};
#endif

// Store the up or down state of each servo
bool leftServosUp[9];
bool rightServosUp[8];

void adjustServoPosition(char restingPoint, char board, char servoChar, char direction)
{
    bool isLeftBoard = true;

    if (board == 'r')
    {
        isLeftBoard = false;
    }
    else if (board == 'l')
    {
        isLeftBoard = true;
    }
    else
    {
        Serial.println("Bad board position (l || r)");
        return;
    }

    int servo = servoChar - '0';

    if (isLeftBoard)
    {
        if (servo < 0 || servo > 8)
        {
            Serial.println("Bad servo position (0 .. 8)");
            return;
        }
    }
    else
    {
        if (servo < 0 || servo > 7)
        {
            Serial.println("Bad servo position (0 .. 7)");
            return;
        }
    }

    int amountToAdd = 0;
    if (direction == '+')
    {
        amountToAdd = 2;
    }
    else if (direction == '-')
    {
        amountToAdd = -2;
    }
    else
    {
        Serial.println("Bad servo direction (+ || -)");
        return;
    }

    if (restingPoint == 'u')
    {
        if (isLeftBoard)
        {
            leftServosUpPositions[servo] += amountToAdd;
        }
        else
        {
            rightServosUpPositions[servo] += amountToAdd;
        }

        moveAllServosUp();
    }
    else if (restingPoint == 'c')
    {
        if (isLeftBoard)
        {
            leftServosCenterPositions[servo] += amountToAdd;
        }
        else
        {
            rightServosCenterPositions[servo] += amountToAdd;
        }

        moveAllServosCenter();
    }
    else if (restingPoint == 'd')
    {
        if (isLeftBoard)
        {
            leftServosDownPositions[servo] += amountToAdd;
        }
        else
        {
            rightServosDownPositions[servo] += amountToAdd;
        }

        moveAllServosDown();
    }
}

void printServoPositions()
{
    // Left board
    Serial.print("int leftServosUpPositions[9] = {");

    for (int i = 0; i < 9; i++)
    {
        if (i != 0)
        {
            Serial.print(',');
        }
        Serial.print(leftServosUpPositions[i]);
    }
    Serial.println("};");

    Serial.print("int leftServosCenterPositions[9] = {");

    for (int i = 0; i < 9; i++)
    {
        if (i != 0)
        {
            Serial.print(',');
        }
        Serial.print(leftServosCenterPositions[i]);
    }
    Serial.println("};");

    Serial.print("int leftServosDownPositions[9] = {");

    for (int i = 0; i < 9; i++)
    {
        if (i != 0)
        {
            Serial.print(',');
        }
        Serial.print(leftServosDownPositions[i]);
    }
    Serial.println("};");
    Serial.println("");

    // Right board
    Serial.print("int rightServosUpPositions[9] = {");

    for (int i = 0; i < 8; i++)
    {
        if (i != 0)
        {
            Serial.print(',');
        }
        Serial.print(rightServosUpPositions[i]);
    }
    Serial.println("};");

    Serial.print("int rightServosCenterPositions[9] = {");

    for (int i = 0; i < 8; i++)
    {
        if (i != 0)
        {
            Serial.print(',');
        }
        Serial.print(rightServosCenterPositions[i]);
    }
    Serial.println("};");

    Serial.print("int rightServosDownPositions[9] = {");

    for (int i = 0; i < 8; i++)
    {
        if (i != 0)
        {
            Serial.print(',');
        }
        Serial.print(rightServosDownPositions[i]);
    }
    Serial.println("};");
}

void offsetAllServoPositions()
{
    for (int i = 0; i < 9; i++)
    {
        leftServosUpPositions[i] += 30;
    }

    for (int i = 0; i < 9; i++)
    {
        leftServosDownPositions[i] -= 30;
    }

    for (int i = 0; i < 8; i++)
    {
        rightServosUpPositions[i] -= 30;
    }

    for (int i = 0; i < 8; i++)
    {
        rightServosDownPositions[i] += 30;
    }
}

void initializeServos()
{
    // Comment in the following line when testing
    // offsetAllServoPositions();

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
    if (board == LEFT_SERVO_BOARD)
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

    if (board == LEFT_SERVO_BOARD)
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

    if (board == LEFT_SERVO_BOARD)
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
    if (board == LEFT_SERVO_BOARD)
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

int getServoCenterPosition(int board, int servo)
{
    if (board == LEFT_SERVO_BOARD)
    {
        return map(leftServosCenterPositions[servo], 0, 180, servoMin, servoMax);
    }
    else
    {
        return map(rightServosCenterPositions[servo], 0, 180, servoMin, servoMax);
    }
}

void moveServoCenter(int board, int servo)
{
    if (board == LEFT_SERVO_BOARD)
    {
        leftServoBoard.setPWM(servo, 0, getServoCenterPosition(board, servo));
    }
    else
    {
        rightServoBoard.setPWM(servo, 0, getServoCenterPosition(board, servo));
    }
}

void moveAllServosCenter()
{
    for (int i = 0; i <= 8; i++)
    {
        leftServoBoard.setPWM(i, 0, map(leftServosCenterPositions[i], 0, 180, servoMin, servoMax));
        delay(initializeServoDelay);
    }

    for (int i = 0; i <= 7; i++)
    {
        rightServoBoard.setPWM(i, 0, map(rightServosCenterPositions[i], 0, 180, servoMin, servoMax));
        delay(initializeServoDelay);
    }

    delay(initializeServoFinalDelay);
}

void moveAllServosUp()
{
    for (int i = 0; i <= 8; i++)
    {
        moveServoUp(1, i);
        delay(initializeServoDelay);
    }

    for (int i = 0; i <= 7; i++)
    {
        moveServoUp(2, i);
        delay(initializeServoDelay);
    }

    delay(initializeServoFinalDelay);
}

void moveAllServosDown()
{
    for (int i = 0; i <= 8; i++)
    {
        moveServoDown(1, i);
        delay(initializeServoDelay);
    }

    for (int i = 0; i <= 7; i++)
    {
        moveServoDown(2, i);
        delay(initializeServoDelay);
    }

    delay(initializeServoFinalDelay);
}