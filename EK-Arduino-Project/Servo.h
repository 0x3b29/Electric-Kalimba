#ifndef SERVO_H
#define SERVO_H

#define LEFT_SERVO_BOARD 1
#define RIGHT_SERVO_BOARD 2

void initializeServos();
void setServoPosition(int board, int servo, int position);
void toggleServo(int board, int servo);
void moveServoCenter(int board, int servo);
void moveAllServosCenter();
void moveAllServosUp();
void moveAllServosDown();
void printServoPositions();
void adjustServoPosition(char restingPoint, char board, char servoChar, char direction);

int getServoCenterPosition(int board, int servo);

#endif