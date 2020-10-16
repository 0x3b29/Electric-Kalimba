#include <Adafruit_PWMServoDriver.h>
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

class MyEvent
{
  private:
  unsigned long when;
  String what;

  MyEvent* next;
  MyEvent* previous;
  
  public: 
  MyEvent(long when, String what)
  {
    this->when = when;
    this->what = what;

//    Serial.print("I am due at: '");
//    Serial.print(when);
//    Serial.println("'");

    next = NULL;
    previous = NULL;
  }

  void setNext(MyEvent* next)
  {
    this->next = next;
  }

  MyEvent* getNext()
  {
    return this->next; 
  }

  void setPrevious(MyEvent* previous)
  {
    this->previous = previous;
  }

  MyEvent* getPrevious()
  {
    return this->previous; 
  }

  unsigned long getWhen()
  {
    return this->when;
  }

  String getWhat()
  {
    return this->what;
  }
};


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
    
  leftServoBoard.begin();             //Start each board
  rightServoBoard.begin();
  leftServoBoard.setOscillatorFrequency(27000000);    //Set the PWM oscillator frequency, used for fine calibration
  rightServoBoard.setOscillatorFrequency(27000000);
  leftServoBoard.setPWMFreq(servoFrequency);          //Set the servo operating frequency
  rightServoBoard.setPWMFreq(servoFrequency);

  Serial.println("Up");
  up();
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

int val;

String str;

MyEvent* headNode;
MyEvent* nodeToDelete;

void eventParser(String what)
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
        processString();
    }
}

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

String input;

void processString()
{
    // Serial.println("input: '" + input + "'");

    if (input.indexOf(',') > 0)
    {
        int colonIndex = input.indexOf(';');
    
        String data = input.substring(0, colonIndex);
        input.remove(0, colonIndex + 1);
        
        int commaIndex = data.indexOf(',');
        String firstValue = data.substring(0, commaIndex);
        String secondValue = data.substring(commaIndex + 1);
    
        Serial.println("Play Note " + firstValue + " and then wait " + secondValue + "ms");
    
        playNote(firstValue.toInt());
        
        addEvent(new MyEvent(millis() + secondValue.toInt(), "process"));       
    }
}

void loop()
{
    while (headNode != NULL && headNode->getWhen() < millis())
    {
        // Serial.print(headNode->getWhen());
        // Serial.println(" is now due " + headNode->getWhat());      
        
        eventParser(headNode->getWhat());
        nodeToDelete = headNode;
        headNode = headNode->getNext();
        delete nodeToDelete;
        nodeToDelete = NULL;
    }

    if (Serial.available() > 0)
    {    
        str = Serial.readString();
        Serial.println("Got '" + str + "'");

        if (str.charAt(0) == 'c')
        {
            Serial.println("Centering ...");
            center();
        }

        if (str.charAt(0) == 'u')
        {
            Serial.println("Upping ...");
            up();
        }

        if (str.charAt(0) == 'd')
        {
            Serial.println("Downing ...");
            down();
        }

        if (str.indexOf(',') > 0)
        {
            input = str;
            processString();
        }
        else
        {
            val = str.toInt();

            if (val > 0)
            {
                playNote(val);
            }
        }

        if (str.startsWith("100"))
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
