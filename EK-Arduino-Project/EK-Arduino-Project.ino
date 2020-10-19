#include <Adafruit_PWMServoDriver.h>
#include <LiquidCrystal.h>
#include "MyEvent.h"
#include <Encoder.h>

Adafruit_PWMServoDriver leftServoBoard = Adafruit_PWMServoDriver(0x40);    //Create an object of board 1
Adafruit_PWMServoDriver rightServoBoard = Adafruit_PWMServoDriver(0x41);    //Create an object of board 2 

int servoMin =  150;        // This is the servos minimum pulse length count (out of 4096)
int servoMax =  600;        // This is the servos maximum pulse length count (out of 4096)
int servoFrequency = 50;    // Servo update frequency, analog servos typically run at ~50 Hz

int servoTravelTime = 150;
int servoRelaxAmount = 20;

int leftServosUpPositions[9] = {100, 102, 104, 108, 110, 112, 114, 116, 118};
int board1ServoCenterPos[9] = {85, 85, 85, 85, 85, 85, 85, 85, 85};
int leftServosDownPositions[9] = {50, 50, 50, 50, 50, 50, 50, 50, 50};

int rightServosDownPositions[8] = {130, 130, 130, 130, 130, 130, 130, 130};
int board2ServoCenterPos[8] = {85, 85, 85, 90, 91, 92, 93, 94};
int rightServosUpPositions[8] = {62, 55, 64, 65, 66, 69, 71, 73};

bool leftServosUp[9];
bool rightServosUp[8];

String serialInput;
String notesString;

MyEvent* headNode;
MyEvent* nodeToDelete;

// Prepare the encoder
const int PinSW = 4;
long oldEncoderValue = -123456;
bool oldPressedState = false;
Encoder encoder(2, 3);

// Prepare the LCD
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

void addEvent(MyEvent* newEvent)
{
    MyEvent* previousNode = NULL;
    MyEvent* currentNode = headNode;

    while (currentNode != NULL && newEvent->getWhen() > currentNode->getWhen())
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
            // Serial.print(currentNode->getPrevious()->getWhen());
            // Serial.print(" and ");
            // Serial.println(currentNode->getWhen());

            newEvent->setPrevious(currentNode->getPrevious());
            newEvent->setNext(currentNode);

            currentNode->getPrevious()->setNext(newEvent);
            currentNode->setPrevious(newEvent);
        }
    }
}

void parseEvent(String what)
{
    if (what.startsWith("relax"))
    {
        // Serial.println("Got " + what);

        int comma1Index = what.indexOf(',');
        int comma2Index = what.indexOf(',', comma1Index + 1);
        int comma3Index = what.indexOf(',', comma2Index + 1);

        String command = what.substring(0, comma1Index);
        String firstArg = what.substring(comma1Index + 1, comma2Index);
        String secondArg = what.substring(comma2Index + 1, comma3Index);
        String thirdArg = what.substring(comma3Index + 1, what.length());

        /*
        Serial.println("command " + command);
        Serial.println("firstArg " + firstArg);
        Serial.println("secondArg " + secondArg);
        Serial.println("thirdArg " + thirdArg);
        */

        setServoPosition(firstArg.toInt(), secondArg.toInt(), thirdArg.toInt());
    }
  
    if (what.startsWith("process"))
    {
        processNotesString();
    }
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

void toogleServo(int board, int servo)
{
    if (board == 1)
    {
        if (leftServosUp[servo] == true)
        {
            setServoPosition(board, servo, map(leftServosDownPositions[servo], 0, 180, servoMin, servoMax));

            addEvent(new MyEvent(millis() + servoTravelTime, "relax, " + 
                    String(board) + ", " + 
                    String(servo) + ", " + 
                    String(map(leftServosDownPositions[servo] + servoRelaxAmount, 0, 180, servoMin, servoMax))));   
        }
        else
        {
            leftServoBoard.setPWM(servo, 0, map(leftServosUpPositions[servo], 0, 180, servoMin, servoMax));

            addEvent(new MyEvent(millis() + servoTravelTime, "relax, " + 
                    String(board) + ", " + 
                    String(servo) + ", " + 
                    String(map(leftServosUpPositions[servo] - servoRelaxAmount, 0, 180, servoMin, servoMax))));   
        }

        leftServosUp[servo] = !leftServosUp[servo];
    }
    else
    {
        if (rightServosUp[servo] == true)
        {
            setServoPosition(board, servo, map(rightServosDownPositions[servo], 0, 180, servoMin, servoMax));

            addEvent(new MyEvent(millis() + servoTravelTime, "relax, " + 
                    String(board) + ", " + 
                    String(servo) + ", " + 
                    String(map(rightServosDownPositions[servo] - servoRelaxAmount, 0, 180, servoMin, servoMax))));   
        }
        else
        {
            rightServoBoard.setPWM(servo, 0, map(rightServosUpPositions[servo], 0, 180, servoMin, servoMax));

            addEvent(new MyEvent(millis() + servoTravelTime, "relax, " + 
                    String(board) + ", " + 
                    String(servo) + ", " + 
                    String(map(rightServosUpPositions[servo] + servoRelaxAmount, 0, 180, servoMin, servoMax))));   
        }

        rightServosUp[servo] = !rightServosUp[servo];
    }
}

void playNote (int note)
{
  switch(note)
  {
    case 1:
    toogleServo(1, 8);
    break;
    
    case 2:
    toogleServo(1, 7);
    break;
        
    case 3:
    toogleServo(2, 0);
    break;

    case 4:
    toogleServo(1, 6);
    break;

    case 5:
    toogleServo(2, 1);
    break;

    case 6:
    toogleServo(1, 5);
    break;
    
    case 7:
    toogleServo(2, 2);
    break;

    case 8:
    toogleServo(1, 4);
    break;

    case 9:
    toogleServo(2, 3);
    break;

    case 10:
    toogleServo(1, 3);
    break;

    case 11:
    toogleServo(2, 4);
    break;
    
    case 12:
    toogleServo(1, 2);
    break;

    case 13:
    toogleServo(2, 5);
    break;

    case 14:
    toogleServo(1, 1);
    break;

    case 15:
    toogleServo(2, 6);
    break;

    case 16:
    toogleServo(1, 0);
    break;
    
    case 17:
    toogleServo(2, 7);
    break;
  }
}

void processNotesString()
{
    // Serial.println("notesString: '" + notesString + "'");

    if (notesString.indexOf(',') > 0)
    {
        int colonIndex = notesString.indexOf(';');
    
        String data = notesString.substring(0, colonIndex);
        notesString.remove(0, colonIndex + 1);
        
        int commaIndex = data.indexOf(',');
        String firstValue = data.substring(0, commaIndex);
        String secondValue = data.substring(commaIndex + 1);
    
        Serial.println("Play Note " + firstValue + " and then wait " + secondValue + "ms");
    
        playNote(firstValue.toInt());
        
        addEvent(new MyEvent(millis() + secondValue.toInt(), "process"));       
    }
}

void setup() 
{
    Serial.begin(115200);

    //Start each board
    leftServoBoard.begin();             
    rightServoBoard.begin();

    //Set the PWM oscillator frequency, used for fine calibration
    leftServoBoard.setOscillatorFrequency(27000000);    
    rightServoBoard.setOscillatorFrequency(27000000);

    //Set the servo operating frequency
    leftServoBoard.setPWMFreq(servoFrequency);          
    rightServoBoard.setPWMFreq(servoFrequency);

    // Define the switch on the rotary encoder
    pinMode(PinSW,INPUT);
    digitalWrite(PinSW, HIGH);

    // Initialize lcd with size
    lcd.begin(16, 2);

    // Move all servos to defined positions
    Serial.println("Initialize Up");
    lcd.print("Initialize Up");
    up();
}

void clearLCDLine(int line)
{               
    lcd.setCursor(0, line);

    for(int n = 0; n < 16; n++) 
    {
        lcd.print(" ");
    }
}

void loop()
{
    // Check if rotary encoder got turned
    long encoderValue = encoder.read();

    if (encoderValue != oldEncoderValue)
    {
        Serial.println(encoder.read());
        oldEncoderValue = encoderValue;

        clearLCDLine(1);
        lcd.setCursor(0, 1);
        lcd.println(encoderValue);
    }

    // Check if rotary encoder got pressed
    if (oldPressedState != digitalRead(PinSW)) 
    {
        Serial.println("Pressed");
        
        encoderValue = 0;
        clearLCDLine(1);
        lcd.setCursor(0, 1);
        lcd.println(encoderValue);

        oldPressedState = digitalRead(PinSW);
    }

    // Process event queue
    while (headNode != NULL && headNode->getWhen() < millis())
    {
        // Serial.print(headNode->getWhen());
        // Serial.println(" is now due " + headNode->getWhat());      
        
        parseEvent(headNode->getWhat());
        nodeToDelete = headNode;
        headNode = headNode->getNext();
        delete nodeToDelete;
        nodeToDelete = NULL;
    }

    // Process serial input 
    if (Serial.available() > 0)
    {    
        serialInput = Serial.readString();
        Serial.println("Got '" + serialInput + "'");

        if (serialInput.charAt(0) == 'c')
        {
            Serial.println("Centering ...");
            center();
        }

        if (serialInput.charAt(0) == 'u')
        {
            Serial.println("Upping ...");
            up();
        }

        if (serialInput.charAt(0) == 'd')
        {
            Serial.println("Downing ...");
            down();
        }

        if (serialInput.indexOf(',') > 0)
        {
            notesString = serialInput;
            processNotesString();
        }
        else
        {
            int val = serialInput.toInt();

            if (val > 0)
            {
                playNote(val);
            }
        }

        if (serialInput.startsWith("100"))
        {
            for (int i = 1; i <= 17; i++)
            {
                playNote(i);
                Serial.println(i);
                delay(100);
            }
        }
    }
}

void center()
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

void up()
{
    for (int i = 0; i <= 8; i++)
    {
        leftServoBoard.setPWM(i, 0, map(leftServosUpPositions[i], 0, 180, servoMin, servoMax));
        leftServosUp[i] = true;
    }

    for (int i = 0; i <= 7; i++)
    {
        rightServoBoard.setPWM(i, 0, map(rightServosUpPositions[i], 0, 180, servoMin, servoMax));
        rightServosUp[i] = true;
    }
}

void down()
{
  for (int i = 0; i <= 8; i++)
  {
    leftServoBoard.setPWM(i, 0, map(leftServosDownPositions[i], 0, 180, servoMin, servoMax));
    leftServosUp[i] = false;
  }
  
  for (int i = 0; i <= 7; i++)
  {
    rightServoBoard.setPWM(i, 0, map(rightServosDownPositions[i], 0, 180, servoMin, servoMax));
    rightServosUp[i] = false;
  }
}
