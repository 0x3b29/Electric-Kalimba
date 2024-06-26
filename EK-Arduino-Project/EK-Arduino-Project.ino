#include <Adafruit_PWMServoDriver.h>
#include <LiquidCrystal.h>
#include "MyEvent.h"
#include "MyMenu.h"
#include <Encoder.h>
#include "MyEnums.h"
#include "MyNotes.h"
#include "Adafruit_Debounce.h"

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

String selectedPrefix = "> ";
String emptyPrefix = "  ";
bool wasLastdirectionUp = false;

short currentArrayPosition;
short currentArraySize;
const unsigned char * currentNotesArray;
const unsigned short int * currentOffsetsArray;

short int luxembourgAnthemSize = 146;
const PROGMEM unsigned char luxembourgAnthemNotes[146] = {5, 12, 5, 12, 8, 15, 8, 15, 3, 10, 5, 12, 4, 11, 4, 11, 4, 11, 4, 11, 5, 12, 6, 13, 5, 12, 4, 11, 3, 10, 5, 12, 5, 12, 8, 15, 8, 15, 7, 14, 7, 14, 6, 13, 9, 16, 8, 15, 7, 14, 5, 12, 7, 14, 6, 13, 5, 12, 2, 9, 2, 9, 3, 10, 4, 11, 5, 12, 6, 13, 6, 13, 5, 12, 4, 11, 3, 10, 5, 12, 5, 12, 8, 15, 7, 14, 5, 12, 5, 12, 6, 13, 7, 14, 8, 15, 9, 16, 10, 17, 9, 16, 8, 15, 6, 13, 5, 12, 3, 10, 5, 12, 4, 11, 3, 10, 5, 12, 5, 12, 6, 13, 7, 14, 8, 15, 9, 16, 10, 17, 9, 16, 8, 15, 6, 13, 5, 12, 8, 15, 7, 14, 9, 16, 8, 15};
const PROGMEM unsigned short int luxembourgAnthemOffsets[146] = {2, 475, 3, 848, 3, 192, 3, 522, 3, 440, 3, 468, 3, 492, 3, 807, 3, 153, 3, 455, 3, 225, 2, 250, 3, 686, 3, 257, 2, 1366, 2, 451, 3, 895, 3, 184, 3, 464, 3, 423, 3, 481, 3, 473, 3, 751, 3, 197, 2, 479, 3, 452, 3, 704, 3, 212, 3, 1476, 2, 478, 2, 700, 3, 241, 3, 543, 3, 471, 2, 417, 3, 438, 3, 822, 3, 201, 3, 467, 3, 456, 3, 488, 3, 492, 3, 1426, 2, 465, 3, 505, 3, 214, 3, 224, 3, 454, 3, 560, 3, 677, 2, 189, 3, 521, 3, 455, 3, 780, 3, 205, 3, 428, 3, 479, 3, 1498, 3, 446, 3, 540, 2, 214, 2, 245, 3, 442, 3, 505, 3, 669, 3, 189, 2, 547, 3, 497, 3, 737, 3, 228, 3, 522, 3, 490, 3, 0};

const PROGMEM char ageOfEmpires[] = "6,500;9,700;11,300;10,300;9,300;10,800;6,500;9,700;11,300;10,300;9,300;12,800;6,500;9,700;11,300;10,300;9,300;10,800;10,500;10,700;11,300;10,300;8,300;9,1500;10,300;13,300;12,300;14,800;17,300;16,700;14,200;13,200;14,300;16,1000;10,300;13,300;12,300;10,500;9,400;12,600;10,1500;3,20;10,500;6,20;13,700;8,20;15,300;7,20;14,300;6,20;13,300;7,20;14,800;3,20;10,500;6,20;13,700;8,20;15,300;7,20;14,300;6,20;13,300;9,20;16,800;3,20;10,500;6,20;13,700;8,20;15,300;7,20;14,300;6,20;13,300;7,20;14,800;7,20;14,500;7,20;14,700;8,20;15,300;7,20;14,300;5,20;12,300;6,20;13,800;";
const PROGMEM char pipiTheme[] = "5,336;8,349;10,312;8,337;9,643;11,132;10,151;9,148;8,173;7,322;9,360;5,297;7,286;8,642;10,669;5,315;8,303;10,301;8,298;9,589;11,135;10,117;9,200;8,173;7,301;9,343;5,313;7,360;8,1080;10,55;17,433;10,55;17,240;10,55;17,229;4,55;11,422;4,55;11,174;10,55;17,183;9,55;16,182;9,55;16,154;9,55;16,180;8,55;15,178;7,55;14,163;8,55;15,197;9,55;16,426;10,56;17,390;10,55;17,169;10,56;17,202;4,55;11,409;4,56;11,137;10,55;17,187;9,55;16,201;9,55;16,212;8,56;15,212;7,55;14,255;8,55;15,921;8,388;9,386;10,373;11,572;13,178;12,174;11,208;10,259;9,200;10,565;5,75;12,119;11,74;4,110;10,75;17,122;9,75;16,170;8,75;15,106;9,75;16,463;11,174;10,172;9,190;8,219;7,168;8,372;9,387;10,237;8,97;15,106;9,96;16,113;10,96;17,155;4,96;11,478;13,194;12,177;11,174;10,262;9,171;10,583;12,183;11,191;10,194;9,234;8,194;9,632;11,201;10,210;9,218;8,279;7,277;8,6;15,0;15,0;";
const PROGMEM char pommerscheTheme[] = "5,612;8,529;8,474;8,458;7,427;9,527;8,474;5,514;10,385;12,412;12,412;12,412;11,405;10,420;9,794;11,474;13,460;13,621;12,204;11,183;10,404;12,348;12,353;10,372;9,345;11,381;11,352;10,133;9,192;10,315;11,364;12,667;11,378;13,329;13,200;12,438;11,198;10,312;12,394;12,259;13,361;12,609;12,313;12,306;12,385;13,334;14,427;15,0;";

const char notesArrayText[][13] = {"Play Note 1", "Play Note 2", "Play Note 3", "Play Note 4", 
    "Play Note 5", "Play Note 6", "Play Note 7", "Play Note 8", "Play Note 9", 
    "Play Note 10", "Play Note 11", "Play Note 12", "Play Note 13", "Play Note 14", 
    "Play Note 15", "Play Note 16", "Play Note 17"};

int totalNotesPlayed = 0;
unsigned long lastEventDue = 0;
unsigned long lastOffset;
unsigned long millisLastNotePlayed = 0;

int bufferPosition;
char serialInputBuffer [100];

MyEvent* headNode;
MyEvent* nodeToDelete;

// Create the menu items
MyMenu* currentMenu;

MyMenu* mainMenu1 = new MyMenu((char *)"Play Notes", SetMenu, NotesMenu);
MyMenu* mainMenu2 = new MyMenu((char *)"Play Songs", SetMenu, SongMenu);

MyMenu* mainMenu3 = new MyMenu((char *)"Init All Up", Init, Up);
MyMenu* mainMenu4 = new MyMenu((char *)"Init All Down", Init, Down);
MyMenu* mainMenu5 = new MyMenu((char *)"Init All Center", Init, Center);
MyMenu* mainMenu6 = new MyMenu((char *)"Buzzer is Off", ToggleBuzzer, 0);

MyMenu* songMenu1 = new MyMenu((char *)"Age of Empires",PlaySong, AgeOfEmpiresTheme);
MyMenu* songMenu2 = new MyMenu((char *)"Luxebourg Anth.",PlaySong, LuxembourgAnthem);
MyMenu* songMenu3 = new MyMenu((char *)"Pipi Theme",PlaySong, PipiTheme);
MyMenu* songMenu4 = new MyMenu((char *)"Pommersche",PlaySong, PommerscheTheme);
MyMenu* songMenuBack = new MyMenu((char *)"Back", SetMenu, MainMenu);

MyMenu* notesMenu[17];

// Prepare the LCD
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

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

// Add a passive Buzzer
int passiveBuzzerDuration = 100;
int passiveBuzzerPin = 6;
bool isBuzzerEnabled = false;

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

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}

void generateEventsFromPROGMEM(const char * startChar)
{
    char c;
    const char * currentChar = startChar;

    char buffer[10];
    int bufferIndex = 0;

    while((c = pgm_read_byte(currentChar++)))
    {
        if (c == ';')
        {
            buffer[bufferIndex] = '\0';
            createEventFromStr(buffer);
            bufferIndex = 0;
        }
        else
        {
            buffer[bufferIndex] = c;
            bufferIndex++;
        }
    }
}

void preparePrintMenuToLCD()
{
    if (wasLastdirectionUp)
    {
        printToLCD(currentMenu->caption, currentMenu->bottomNeighbour->caption, 0);
    }
    else
    {
        printToLCD(currentMenu->topNeighbour->caption, currentMenu->caption, 1);
    }
}

void parseEvent(EventType what, int * args)
{
    switch (what)
    {
        case SetServoPosition:
            setServoPosition(args[0], args[1], args[2]);
            break;
        
        case PlayNote:
            playNote(args[0]);
            break;

        case SetMenu:
            if (args[0] == NotesMenu)
            {
                currentMenu = notesMenu[0];
            }
            else if (args[0] == SongMenu)
            {
                currentMenu = songMenu1;
            }
            else if (args[0] == MainMenu)
            {
                currentMenu = mainMenu1;
            }

            printToLCD(currentMenu->caption, currentMenu->bottomNeighbour->caption, 0);
            break;

        case Init:
            if (args[0] == Up)
            {
                moveAllServosUp();
            }
            else if (args[0] == Down)
            {
                moveAllServosDown();
            }
            else if (args[0] == Center)
            {
                moveAllServosCenter();
            }
            break;

        case ToggleBuzzer:
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

            break;

        case PlaySong:
            if (args[0] == AgeOfEmpiresTheme)
            {
                generateEventsFromPROGMEM(ageOfEmpires);
            }
            else if (args[0] == LuxembourgAnthem)
            {
                currentArrayPosition = 0;
                currentArraySize = luxembourgAnthemSize;
                currentNotesArray = luxembourgAnthemNotes;
                currentOffsetsArray = luxembourgAnthemOffsets;

                processNotesArray();
            }
            else if (args[0] == PipiTheme)
            {
                generateEventsFromPROGMEM(pipiTheme);
            }
            else if (args[0] == PommerscheTheme)
            {
                generateEventsFromPROGMEM(pommerscheTheme);
            }
            break;

        case ProcessCurrentArray:
            processNotesArray();
            break;

        default:
            break;
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

    int* args = new int [3];

    args[0] = board;
    args[1] = servo;
    args[2] = map(servoDownPosition + servoRelaxAmountToAdd, 0, 180, servoMin, servoMax);

    addEvent(new MyEvent(millis() + servoTravelTime, SetServoPosition, args));
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

    addEvent(new MyEvent(millis() + servoTravelTime, SetServoPosition, args));
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

void buzzerNote(int note)
{
    if (isBuzzerEnabled)
    {
        tone(passiveBuzzerPin, note, passiveBuzzerDuration);;
    }
}

void playNote(int note)
{
    totalNotesPlayed++;

    switch(note)
    {
        case 1:
        buzzerNote(NOTE_C5);
        toggleServo(1, 8);
        break;

        case 2:
        buzzerNote(NOTE_D5);
        toggleServo(1, 7);
        break;
            
        case 3:
        buzzerNote(NOTE_E5);
        toggleServo(2, 0);
        break;

        case 4:
        buzzerNote(NOTE_F5);
        toggleServo(1, 6);
        break;

        case 5:
        buzzerNote(NOTE_G5);
        toggleServo(2, 1);
        break;

        case 6:
        buzzerNote(NOTE_A6);
        toggleServo(1, 5);
        break;

        case 7:
        buzzerNote(NOTE_B6);
        toggleServo(2, 2);
        break;

        case 8:
        buzzerNote(NOTE_C6);
        toggleServo(1, 4);
        break;

        case 9:
        buzzerNote(NOTE_D6);
        toggleServo(2, 3);
        break;

        case 10:
        buzzerNote(NOTE_E6);
        toggleServo(1, 3);
        break;

        case 11:
        buzzerNote(NOTE_F6);
        toggleServo(2, 4);
        break;

        case 12:
        buzzerNote(NOTE_G6);
        toggleServo(1, 2);
        break;

        case 13:
        buzzerNote(NOTE_A7);
        toggleServo(2, 5);
        break;

        case 14:
        buzzerNote(NOTE_B7);
        toggleServo(1, 1);
        break;

        case 15:
        buzzerNote(NOTE_C7);
        toggleServo(2, 6);
        break;

        case 16:
        buzzerNote(NOTE_D7);
        toggleServo(1, 0);
        break;

        case 17:
        buzzerNote(NOTE_E7);
        toggleServo(2, 7);
        break;
    }

    long millisCurrentNote = millis();

    Serial.print("ms: ");
    Serial.print(millisCurrentNote);
    Serial.print(" diff: ");
    Serial.print(millisCurrentNote - millisLastNotePlayed);
    Serial.print(" Note: ");
    Serial.print(note);
    Serial.print(" Total: ");
    Serial.println(totalNotesPlayed);

    millisLastNotePlayed = millisCurrentNote;
}

void processNotesArray()
{
    unsigned char currentNote = pgm_read_byte_near(&currentNotesArray[currentArrayPosition]);
    unsigned short int currentOffset = pgm_read_word_near(&currentOffsetsArray[currentArrayPosition]);

    playNote(currentNote);

    currentArrayPosition++;

    if (currentArrayPosition < currentArraySize)
    {
        addEvent(new MyEvent(millis() + currentOffset, ProcessCurrentArray, NULL));    
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

void printToLCD(String firstLine, String secondLine, int selectedLine)
{
    String upperLine;
    String lowerLine;

    if (selectedLine == 0)
    {
        upperLine = selectedPrefix;
        upperLine.concat(firstLine);

        lowerLine = emptyPrefix;
        lowerLine.concat(secondLine);
    }
    else if (selectedLine == 1)
    {
        upperLine = emptyPrefix;
        upperLine.concat(firstLine);

        lowerLine = selectedPrefix;
        lowerLine.concat(secondLine);
    }
    else
    {
        upperLine = emptyPrefix;
        upperLine.concat(firstLine);

        lowerLine = emptyPrefix;
        lowerLine.concat(secondLine);
    }

    // Clear LCD
    clearLCDLine(0);
    clearLCDLine(1);

    // Print lines to lcd
    lcd.setCursor(0, 0);
    lcd.print(upperLine);
    lcd.setCursor(0, 1);
    lcd.print(lowerLine);
}

void createEventFromStr(char input[])
{
    char* commaPos = strchr(input, ',');
    * commaPos = 0;
    
    int* args = new int [1];
    args[0] = atoi(input);

    ++commaPos;
    
    unsigned long offset = atoi(commaPos);

    if (lastEventDue >= millis())
    {
        addEvent(new MyEvent((lastEventDue + lastOffset), PlayNote, args)); 
        lastEventDue = lastEventDue + lastOffset;
        lastOffset = offset;
    }
    else
    {
        addEvent(new MyEvent((millis() + 250), PlayNote, args));
        lastEventDue = millis() + 250;
        lastOffset = offset;
    }
}

void setup() 
{
    Serial.begin(115200);
    encoderButton.begin();

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
    moveAllServosUp();

    // Link the main menu items
    mainMenu1->topNeighbour = mainMenu6;
    mainMenu1->bottomNeighbour = mainMenu2;

    mainMenu2->topNeighbour = mainMenu1;
    mainMenu2->bottomNeighbour = mainMenu3;

    mainMenu3->topNeighbour = mainMenu2;
    mainMenu3->bottomNeighbour = mainMenu4;

    mainMenu4->topNeighbour = mainMenu3;
    mainMenu4->bottomNeighbour = mainMenu5;

    mainMenu5->topNeighbour = mainMenu4;
    mainMenu5->bottomNeighbour = mainMenu6;

    mainMenu6->topNeighbour = mainMenu5;
    mainMenu6->bottomNeighbour = mainMenu1;

    // Link the song menu items
    songMenu1->topNeighbour = songMenuBack;
    songMenu1->bottomNeighbour = songMenu2;

    songMenu2->topNeighbour = songMenu1;
    songMenu2->bottomNeighbour = songMenu3;

    songMenu3->topNeighbour = songMenu2;
    songMenu3->bottomNeighbour = songMenu4; 

    songMenu4->topNeighbour = songMenu3;
    songMenu4->bottomNeighbour = songMenuBack;  

    songMenuBack->topNeighbour = songMenu4;
    songMenuBack->bottomNeighbour = songMenu1;   

    // Create all the entries for the notes Menu
    for (int i = 0; i < 17; i++)
    {
        notesMenu[i] = new MyMenu((char *)notesArrayText[i], PlayNote, i + 1);
    }

    // Link all the inner notes menu items togeather
    for (int i = 1; i < 16; i++)
    {
        notesMenu[i]->topNeighbour = notesMenu[i - 1];
        notesMenu[i]->bottomNeighbour = notesMenu[i + 1];
    }

    // Link all the extreme notes menu items togeather
    notesMenu[0]->topNeighbour = notesMenu[16];
    notesMenu[0]->bottomNeighbour = notesMenu[1];

    notesMenu[16]->topNeighbour = notesMenu[15];
    notesMenu[16]->bottomNeighbour = notesMenu[0];

    // Set entry node of menu
    currentMenu = mainMenu1;

    // Output menu 
    printToLCD(currentMenu->caption, currentMenu->bottomNeighbour->caption, 0);

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


    if (encoderButton.justPressed()) {
      freshButtonPressed = true;
      millisLastEncodeButtonPressed = millis();
      lastEncoderDiv4ButtonPressedValue = newEncoderDiv4Value;
    }

    unsigned long delta =  millis() - millisLastEncodeButtonPressed;

    bool hasNewClick = false;
    if (freshButtonPressed && encoderButton.isPressed() && delta > 100) {
      freshButtonPressed = false;
      hasNewClick = true;
    }

    if (hasNewClick || (encoderButton.isPressed() && newEncoderDiv4Value != lastEncoderDiv4ButtonPressedValue))
    {
        if (encoderButton.isPressed() && newEncoderDiv4Value != lastEncoderDiv4ButtonPressedValue) {
          Serial.print("Retrigger ");
          Serial.println(currentMenu->what);
        }

        if (hasNewClick) {
          Serial.print("Trigger ");
          Serial.println(currentMenu->what);
        }

        lastEncoderDiv4ButtonPressedValue = newEncoderDiv4Value;
        
        parseEvent(currentMenu->what, currentMenu->args);
    }

    // Process event queue
    while (headNode != NULL && headNode->getWhen() < millis())
    {
        // Serial.print(headNode->getWhen());
        // Serial.println(" is now due " + headNode->getWhat());      
        
        parseEvent(headNode->getWhat(), headNode->getArguments());
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

        if (serialChar > 0) {
            didReadSomething = true;
        }

        if (serialChar == ';')
        {
            isValidCommand = true;
            serialInputBuffer[bufferPosition] = '\0';
        }else if (serialChar > 40 && serialChar < 127) {
            serialInputBuffer[bufferPosition] = serialChar;
            bufferPosition++;
        }
    }

    if (didReadSomething == false) {
        return;
    }

    if (didReadSomething == true && isValidCommand == false) {
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

    char* commaPos = strchr(serialInputBuffer, ',');

    if(commaPos != NULL)
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
                int* args = new int [1];
                args[0] = i;
                addEvent(new MyEvent(millis() + (i * 250), PlayNote, args));
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
