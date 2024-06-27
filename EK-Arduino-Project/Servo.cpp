#include "Servo.h"
#include "EventManager.h"
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver leftServoBoard = Adafruit_PWMServoDriver(0x40);    //Create an object of board 1
Adafruit_PWMServoDriver rightServoBoard = Adafruit_PWMServoDriver(0x41);    //Create an object of board 2 

int servoMin =  150;        // This is the servos minimum pulse length count (out of 4096)
int servoMax =  600;        // This is the servos maximum pulse length count (out of 4096)
int servoFrequency = 50;    // Servo update frequency, analog servos typically run at ~50 Hz

int servoTravelTime = 150;
int servoRelaxAmount = 20;

int leftServosUpPositions[9] = {110, 105, 105, 105, 110, 110, 110, 110, 110};
int board1ServoCenterPos[9] = {80, 80, 80, 85, 85, 85, 85, 85, 85};
int leftServosDownPositions[9] = {45, 45, 45, 47, 48, 49, 50, 50, 50};

int rightServosDownPositions[8] = {120, 120, 120, 122, 124, 126, 128, 130};
int board2ServoCenterPos[8] = {85, 85, 87, 90, 91, 92, 93, 94};
int rightServosUpPositions[8] = {58, 58, 64, 65, 66, 62, 66, 66};

bool leftServosUp[9];
bool rightServosUp[8];

void initializeServos(){
    //Start each board
    leftServoBoard.begin();             
    rightServoBoard.begin();

    //Set the PWM oscillator frequency, used for fine calibration
    leftServoBoard.setOscillatorFrequency(27000000);    
    rightServoBoard.setOscillatorFrequency(27000000);

    //Set the servo operating frequency
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

    int* args = new int [3];

    args[0] = board;
    args[1] = servo;
    args[2] = map(servoDownPosition + servoRelaxAmountToAdd, 0, 180, servoMin, servoMax);

    addEvent(new Event(millis() + servoTravelTime, SetServoPosition, args));
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

    int* args = new int [3];

    args[0] = board;
    args[1] = servo;
    args[2] = map(servoUpPosition + servoRelaxAmountToAdd, 0, 180, servoMin, servoMax);

    addEvent(new Event(millis() + servoTravelTime, SetServoPosition, args));
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