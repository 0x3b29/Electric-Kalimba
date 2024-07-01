#ifndef SERVO_H
#define SERVO_H

void initializeServos();
void setServoPosition(int board, int servo, int position);
void toggleServo(int board, int servo);
void moveAllServosCenter();
void moveAllServosUp();
void moveAllServosDown();

#endif