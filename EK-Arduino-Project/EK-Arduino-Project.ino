#include <Adafruit_PWMServoDriver.h>
#include <LiquidCrystal.h>
#include "MyEvent.h"
#include "MyMenu.h"
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

String selectedPrefix = "> ";
String emptyPrefix = "  ";
MyEvent* headNode;
MyEvent* nodeToDelete;

MyMenu* currentMenu;  

// Prepare the encoder
const int PinSW = 4;
long oldEncoderValue = -123456;
bool encoderButtonState = true;
Encoder encoder(2, 3);

// Prepare the LCD
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

MyMenu * mainMenu1 = new MyMenu("Play Notes","SetMenu,NotesMenu1");
MyMenu * mainMenu2 = new MyMenu("Play Songs","SetMenu,SongMenu1");
MyMenu * item3 = new MyMenu("Init All Up","Init,Up");
MyMenu * item4 = new MyMenu("Init All Down","Init,Down");

MyMenu * notesMenu1 = new MyMenu("Play Note 1","Play,1");
MyMenu * notesMenu2 = new MyMenu("Play Note 2","Play,2");
MyMenu * notesMenu3 = new MyMenu("Play Note 3","Play,3");
MyMenu * notesMenu4 = new MyMenu("Play Note 4","Play,4");
MyMenu * notesMenu5 = new MyMenu("Play Note 5","Play,5");
MyMenu * notesMenu6 = new MyMenu("Play Note 6","Play,6");
MyMenu * notesMenu7 = new MyMenu("Play Note 7","Play,7");
MyMenu * notesMenu8 = new MyMenu("Play Note 8","Play,8");
MyMenu * notesMenu9 = new MyMenu("Play Note 9","Play,9");
MyMenu * notesMenu10 = new MyMenu("Play Note 10","Play,10");
MyMenu * notesMenu11 = new MyMenu("Play Note 11","Play,11");
MyMenu * notesMenu12 = new MyMenu("Play Note 12","Play,12");
MyMenu * notesMenu13 = new MyMenu("Play Note 13","Play,13");
MyMenu * notesMenu14 = new MyMenu("Play Note 14","Play,14");
MyMenu * notesMenu15 = new MyMenu("Play Note 15","Play,15");
MyMenu * notesMenu16 = new MyMenu("Play Note 16","Play,16");
MyMenu * notesMenu17 = new MyMenu("Play Note 17","Play,17");

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

    if (what.startsWith("SetMenu"))
    {
        int comma1Index = what.indexOf(',');

        String command = what.substring(0, comma1Index);
        String firstArg = what.substring(comma1Index + 1, what.length());

        if (firstArg.equals("NotesMenu1"))
        {
            currentMenu = notesMenu1;
        }

        String selectedLine = selectedPrefix;
        selectedLine.concat(currentMenu->caption);

        String otherLine = emptyPrefix;
        otherLine.concat(currentMenu->bottomNeighbour->caption);

        clearLCDLine(0);
        clearLCDLine(1);

        lcd.setCursor(0, 0);
        lcd.print(selectedLine);
        lcd.setCursor(0, 1);
        lcd.print(otherLine);
    }

    if (what.startsWith("Play"))
    {
        int comma1Index = what.indexOf(',');
        String firstArg = what.substring(comma1Index + 1, what.length());

        playNote(firstArg.toInt());
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

            addEvent(new MyEvent(millis() + servoTravelTime, "relax, " + 
                    String(board) + ", " + 
                    String(servo) + ", " + 
            String(map(servoDownPosition + servoRelaxAmountToAdd, 0, 180, servoMin, servoMax)))); 
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

            addEvent(new MyEvent(millis() + servoTravelTime, "relax, " + 
                    String(board) + ", " + 
                    String(servo) + ", " + 
            String(map(servoUpPosition + servoRelaxAmountToAdd, 0, 180, servoMin, servoMax)))); 
        }

void toogleServo(int board, int servo)
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

void clearLCDLine(int line)
{               
    lcd.setCursor(0, line);

    for(int n = 0; n < 16; n++) 
    {
        lcd.print(" ");
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

    clearLCDLine(0);
    clearLCDLine(1);

    // Move all servos to defined positions
    Serial.println("Initialize Up");
    // lcd.print("Initialize Up");
    up();
}

    mainMenu1->topNeighbour = item4;
    mainMenu1->bottomNeighbour = mainMenu2;

    mainMenu2->topNeighbour = mainMenu1;
    mainMenu2->bottomNeighbour = item3;

    item3->topNeighbour = mainMenu2;
    item3->bottomNeighbour = item4;

    item4->topNeighbour = item3;
    item4->bottomNeighbour = mainMenu1;

    // Define the notes menue. Could have been solved in a loop for sure
    notesMenu1->topNeighbour = notesMenu17; notesMenu1->bottomNeighbour = notesMenu2;
    notesMenu2->topNeighbour = notesMenu1; notesMenu2->bottomNeighbour = notesMenu3;
    notesMenu3->topNeighbour = notesMenu2; notesMenu3->bottomNeighbour = notesMenu4;
    notesMenu4->topNeighbour = notesMenu3; notesMenu4->bottomNeighbour = notesMenu5;
    notesMenu5->topNeighbour = notesMenu4; notesMenu5->bottomNeighbour = notesMenu6;
    notesMenu6->topNeighbour = notesMenu5; notesMenu6->bottomNeighbour = notesMenu7;
    notesMenu7->topNeighbour = notesMenu6; notesMenu7->bottomNeighbour = notesMenu8;
    notesMenu8->topNeighbour = notesMenu7; notesMenu8->bottomNeighbour = notesMenu9;
    notesMenu9->topNeighbour = notesMenu8; notesMenu9->bottomNeighbour = notesMenu10;
    notesMenu10->topNeighbour = notesMenu9; notesMenu10->bottomNeighbour = notesMenu11;
    notesMenu11->topNeighbour = notesMenu10; notesMenu11->bottomNeighbour = notesMenu12;
    notesMenu12->topNeighbour = notesMenu11; notesMenu12->bottomNeighbour = notesMenu13;
    notesMenu13->topNeighbour = notesMenu12; notesMenu13->bottomNeighbour = notesMenu14;
    notesMenu14->topNeighbour = notesMenu13; notesMenu14->bottomNeighbour = notesMenu15;
    notesMenu15->topNeighbour = notesMenu14; notesMenu15->bottomNeighbour = notesMenu16;
    notesMenu16->topNeighbour = notesMenu15; notesMenu16->bottomNeighbour = notesMenu17;
    notesMenu17->topNeighbour = notesMenu16; notesMenu17->bottomNeighbour = notesMenu1;

    currentMenu = mainMenu1;

    String selectedLine = selectedPrefix;
    selectedLine.concat(currentMenu->caption);

    String otherLine = emptyPrefix;
    otherLine.concat(currentMenu->bottomNeighbour->caption);

    lcd.setCursor(0, 0);
    lcd.print(selectedLine);
    lcd.setCursor(0, 1);
    lcd.print(otherLine);
}

int oldEncoderDiv4Value = 0;
long lastEncoderDiv4ButtonPressedValue = 0;

void loop()
{
    // Check if rotary encoder got turned
    long encoderValue = encoder.read();

    if (encoderValue != oldEncoderValue)
    {
        oldEncoderValue = encoderValue;

        int newEncoderDiv4Value = encoderValue / 4;

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
            
            clearLCDLine(0);
        clearLCDLine(1);

            String selectedLine = selectedPrefix;
            selectedLine.concat(currentMenu->caption);

            if (directionUp)
            {
                String otherLine = emptyPrefix;
                otherLine.concat(currentMenu->bottomNeighbour->caption);

                lcd.setCursor(0, 0);
                lcd.print(selectedLine);
                lcd.setCursor(0, 1);
                lcd.print(otherLine);
            }
            else
            {
                String otherLine = emptyPrefix;
                otherLine.concat(currentMenu->topNeighbour->caption);

                lcd.setCursor(0, 0);
                lcd.print(otherLine);
        lcd.setCursor(0, 1);
                lcd.print(selectedLine);
            }

            oldEncoderDiv4Value = newEncoderDiv4Value;
        }
    }

    // Check if rotary encoder got pressed
    // Also check if encoder state has changed which is important for selecting multiple things in a row (e.g. different notes)
    if (encoderButtonState != digitalRead(PinSW) || newEncoderDiv4Value != lastEncoderDiv4ButtonPressedValue) 
    {
        lastEncoderDiv4ButtonPressedValue = newEncoderDiv4Value;
        encoderButtonState = digitalRead(PinSW);

        if (encoderButtonState == true)
        {
             Serial.println("Button Relaxed");
        }
        else
    {
             Serial.println("Button Pressed");
        
            Serial.println(currentMenu->what);

            parseEvent(currentMenu->what);
        }
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
                String what = "Play,";
                what.concat(i);
                addEvent(new MyEvent(millis() + (i * 100), what));   
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
