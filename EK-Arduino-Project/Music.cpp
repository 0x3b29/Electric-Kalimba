
#include "Music.h"
#include "Event.h"
#include "EventManager.h"
#include "Info.h"
#include "Menu.h"
#include "MusicNotes.h"
#include "Servo.h"

// Add a passive Buzzer
int passiveBuzzerDuration = 100;
int passiveBuzzerPin = 6;
bool isBuzzerEnabled = false;

const unsigned char *currentNotesArray;
const unsigned short int *currentOffsetsArray;
short currentArrayPosition;
short currentArraySize;

uint16_t totalNotesPlayed = 0;
uint8_t lastNotePlayed = NO_NOTE;
unsigned long millisLastNotePlayed = 0;

short int luxembourgAnthemSize = 146;
const PROGMEM unsigned char luxembourgAnthemNotes[146] = {
    5,  12, 5,  12, 8,  15, 8,  15, 3,  10, 5,  12, 4,  11, 4,  11, 4,  11, 4,  11, 5,  12, 6,  13, 5,  12, 4,  11, 3,  10, 5,  12, 5,  12, 8,  15, 8,
    15, 7,  14, 7,  14, 6,  13, 9,  16, 8,  15, 7,  14, 5,  12, 7,  14, 6,  13, 5,  12, 2,  9,  2,  9,  3,  10, 4,  11, 5,  12, 6,  13, 6,  13, 5,  12,
    4,  11, 3,  10, 5,  12, 5,  12, 8,  15, 7,  14, 5,  12, 5,  12, 6,  13, 7,  14, 8,  15, 9,  16, 10, 17, 9,  16, 8,  15, 6,  13, 5,  12, 3,  10, 5,
    12, 4,  11, 3,  10, 5,  12, 5,  12, 6,  13, 7,  14, 8,  15, 9,  16, 10, 17, 9,  16, 8,  15, 6,  13, 5,  12, 8,  15, 7,  14, 9,  16, 8,  15};
const PROGMEM unsigned short int luxembourgAnthemOffsets[146] = {
    2, 475,  3, 848, 3, 192, 3, 522, 3, 440,  3, 468, 3, 492, 3, 807, 3, 153, 3, 455,  3, 225, 2, 250, 3, 686, 3, 257, 2, 1366, 2, 451, 3, 895, 3, 184, 3, 464,
    3, 423,  3, 481, 3, 473, 3, 751, 3, 197,  2, 479, 3, 452, 3, 704, 3, 212, 3, 1476, 2, 478, 2, 700, 3, 241, 3, 543, 3, 471,  2, 417, 3, 438, 3, 822, 3, 201,
    3, 467,  3, 456, 3, 488, 3, 492, 3, 1426, 2, 465, 3, 505, 3, 214, 3, 224, 3, 454,  3, 560, 3, 677, 2, 189, 3, 521, 3, 455,  3, 780, 3, 205, 3, 428, 3, 479,
    3, 1498, 3, 446, 3, 540, 2, 214, 2, 245,  3, 442, 3, 505, 3, 669, 3, 189, 2, 547,  3, 497, 3, 737, 3, 228, 3, 522, 3, 490,  3, 0};

const PROGMEM char ageOfEmpires[] =
    "6,500;9,700;11,300;10,300;9,300;10,800;6,500;9,700;11,300;10,300;9,300;12,800;6,500;9,700;11,300;10,300;9,300;10,800;10,500;10,700;11,300;10,300;8,300;9,"
    "1500;10,300;13,300;12,300;14,800;17,300;16,700;14,200;13,200;14,300;16,1000;10,300;13,300;12,300;10,500;9,400;12,600;10,1500;3,20;10,500;6,20;13,700;8,20;"
    "15,300;7,20;14,300;6,20;13,300;7,20;14,800;3,20;10,500;6,20;13,700;8,20;15,300;7,20;14,300;6,20;13,300;9,20;16,800;3,20;10,500;6,20;13,700;8,20;15,300;7,"
    "20;14,300;6,20;13,300;7,20;14,800;7,20;14,500;7,20;14,700;8,20;15,300;7,20;14,300;5,20;12,300;6,20;13,800;";

const PROGMEM char pipiTheme[] =
    "5,336;8,349;10,312;8,337;9,643;11,132;10,151;9,148;8,173;7,322;9,360;5,297;7,286;8,642;10,669;5,315;8,303;10,301;8,298;9,589;11,135;10,117;9,200;8,173;7,"
    "301;9,343;5,313;7,360;8,1080;10,55;17,433;10,55;17,240;10,55;17,229;4,55;11,422;4,55;11,174;10,55;17,183;9,55;16,182;9,55;16,154;9,55;16,180;8,55;15,178;"
    "7,55;14,163;8,55;15,197;9,55;16,426;10,56;17,390;10,55;17,169;10,56;17,202;4,55;11,409;4,56;11,137;10,55;17,187;9,55;16,201;9,55;16,212;8,56;15,212;7,55;"
    "14,255;8,55;15,921;8,388;9,386;10,373;11,572;13,178;12,174;11,208;10,259;9,200;10,565;5,75;12,119;11,74;4,110;10,75;17,122;9,75;16,170;8,75;15,106;9,75;"
    "16,463;11,174;10,172;9,190;8,219;7,168;8,372;9,387;10,237;8,97;15,106;9,96;16,113;10,96;17,155;4,96;11,478;13,194;12,177;11,174;10,262;9,171;10,583;12,"
    "183;11,191;10,194;9,234;8,194;9,632;11,201;10,210;9,218;8,279;7,277;8,6;15,0;15,0;";

const PROGMEM char pommerscheTheme[] = "5,612;8,529;8,474;8,458;7,427;9,527;8,474;5,514;10,385;12,412;12,412;12,412;11,405;10,420;9,794;11,474;13,460;13,621;"
                                       "12,204;11,183;10,404;12,348;12,353;10,372;9,345;11,381;11,352;10,133;9,192;10,315;11,364;12,667;11,378;13,329;13,200;"
                                       "12,438;11,198;10,312;12,394;12,259;13,361;12,609;12,313;12,306;12,385;13,334;14,427;15,0;";

const PROGMEM char httydImpro[] =
    "9,139;10,181;11,845;12,181;11,184;10,847;9,335;8,635;10,308;5,600;9,356;10,323;12,325;15,708;16,63;15,195;16,373;17,237;16,704;12,422;13,459;15,439;13,"
    "224;12,245;12,421;10,451;8,555;9,216;12,422;10,1088;5,214;6,185;6,1080;5,188;6,262;6,852;5,177;8,251;6,967;5,447;6,427;8,438;8,250;5,233;8,197;9,232;7,"
    "227;5,272;7,178;8,268;6,213;5,276;5,196;4,248;4,220;3,244;2,238;1,182;8,251;5,185;8,245;9,250;7,192;5,263;7,132;8,310;6,231;5,291;5,128;4,232;4,181;3,217;"
    "2,219;1,222;3,885;4,672;2,252;2,269;3,167;4,242;3,243;3,1000;3,454;4,766;2,189;2,288;3,119;4,249;3,253;3,963;6,477;6,402;7,492;7,424;6,229;5,252;3,1049;6,"
    "435;6,425;7,440;7,424;6,221;5,210;8,874;8,871;9,619;10,282;10,400;5,399;10,407;12,454;9,596;10,103;9,208;10,556;10,182;13,190;12,253;10,435;10,240;10,186;"
    "9,413;9,184;10,295;6,367;6,285;7,115;8,501;8,159;9,220;8,283;7,438;6,70;5,184;6,768;8,316;8,194;5,232;8,229;9,223;7,218;5,285;7,192;8,266;6,252;5,252;5,"
    "221;4,278;4,183;3,257;2,221;1,163;8,270;5,214;8,262;9,258;7,214;5,264;7,290;8,294;6,311;5,307;5,289;4,332;4,360;3,405;2,637;1,1000;";

// To convert notes names to numbers, we have this lookup construct where we can string compare against
const char *notesStringLookup[] = {"C4", "D4", "E4", "F4", "G4", "A4", "B4", "C5", "D5", "E5", "F5", "G5", "A5", "B5", "C6", "D6", "E6"};
const int notesValueLookup[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};
const int sizeOfLookup = sizeof(notesValueLookup) / sizeof(notesValueLookup[0]);

uint8_t getNoteValue(char *noteString)
{
    if (noteString[0] >= 48 && noteString[0] <= 57)
    {
        // In case the note started with a number, we parse it as int
        uint8_t note = atoi(noteString);

        if (note == -1)
        {
            // Parsing failed
            return NO_NOTE;
        }
        else if (note > 17 && note < 1)
        {
            // Parsing worked, but not expected range
            return NO_NOTE;
        }
        else
        {
            // Note is valid
            return note;
        }
    }

    // If we get here, we know that noteString started with anything else than a number.

    // Iterate over the array and find the matching note
    for (int i = 0; i < sizeOfLookup; i++)
    {
        if (strcmp(noteString, notesStringLookup[i]) == 0)
        {
            return notesValueLookup[i];
        }
    }

    // If the note is not found, return NO_NOTE as an error code
    return NO_NOTE;
}

void playNote(uint8_t note)
{
    totalNotesPlayed++;
    lastNotePlayed = note;

    switch (note)
    {
    case 1:
        buzzerNoteIfBuzzerEnabled(NOTE_C5);
        toggleServo(1, 8);
        break;

    case 2:
        buzzerNoteIfBuzzerEnabled(NOTE_D5);
        toggleServo(1, 7);
        break;

    case 3:
        buzzerNoteIfBuzzerEnabled(NOTE_E5);
        toggleServo(2, 0);
        break;

    case 4:
        buzzerNoteIfBuzzerEnabled(NOTE_F5);
        toggleServo(1, 6);
        break;

    case 5:
        buzzerNoteIfBuzzerEnabled(NOTE_G5);
        toggleServo(2, 1);
        break;

    case 6:
        buzzerNoteIfBuzzerEnabled(NOTE_A6);
        toggleServo(1, 5);
        break;

    case 7:
        buzzerNoteIfBuzzerEnabled(NOTE_B6);
        toggleServo(2, 2);
        break;

    case 8:
        buzzerNoteIfBuzzerEnabled(NOTE_C6);
        toggleServo(1, 4);
        break;

    case 9:
        buzzerNoteIfBuzzerEnabled(NOTE_D6);
        toggleServo(2, 3);
        break;

    case 10:
        buzzerNoteIfBuzzerEnabled(NOTE_E6);
        toggleServo(1, 3);
        break;

    case 11:
        buzzerNoteIfBuzzerEnabled(NOTE_F6);
        toggleServo(2, 4);
        break;

    case 12:
        buzzerNoteIfBuzzerEnabled(NOTE_G6);
        toggleServo(1, 2);
        break;

    case 13:
        buzzerNoteIfBuzzerEnabled(NOTE_A7);
        toggleServo(2, 5);
        break;

    case 14:
        buzzerNoteIfBuzzerEnabled(NOTE_B7);
        toggleServo(1, 1);
        break;

    case 15:
        buzzerNoteIfBuzzerEnabled(NOTE_C7);
        toggleServo(2, 6);
        break;

    case 16:
        buzzerNoteIfBuzzerEnabled(NOTE_D7);
        toggleServo(1, 0);
        break;

    case 17:
        buzzerNoteIfBuzzerEnabled(NOTE_E7);
        toggleServo(2, 7);
        break;
    }

    long millisCurrentNote = millis();

    /*
        Serial.print("ms: ");
        Serial.print(millisCurrentNote);
        Serial.print(" diff: ");
        Serial.print(millisCurrentNote - millisLastNotePlayed);
        Serial.print(" Note: ");
        Serial.print(note);
        Serial.print(" Total: ");
        Serial.println(totalNotesPlayed);
    */

    millisLastNotePlayed = millisCurrentNote;
}

void buzzerNoteIfBuzzerEnabled(uint16_t note)
{
    if (isBuzzerEnabled)
    {
        tone(passiveBuzzerPin, note, passiveBuzzerDuration);
    }
}

void processNotesArray()
{
    unsigned char currentNote = pgm_read_byte_near(&currentNotesArray[currentArrayPosition]);
    unsigned short int currentOffset = pgm_read_word_near(&currentOffsetsArray[currentArrayPosition]);

    playNote(currentNote);

    currentArrayPosition++;

    if (currentArrayPosition < currentArraySize)
    {
        addEvent(new Event(millis() + currentOffset, ProcessCurrentArray, NULL));
    }
}

void playStairs()
{
    for (int i = 1; i <= 17; i++)
    {
        EventArg *eventArgs = new EventArg[1]{EventArg(i)};
        addEvent(new Event(millis() + (i * 250), PlayNote, eventArgs));
    }
}

void playSong(int song)
{
    if (song == Stairs)
    {
        playStairs();
    }
    else if (song == AgeOfEmpiresTheme)
    {
        generateEventsFromPROGMEM(ageOfEmpires, millis());
    }
    else if (song == LuxembourgAnthem)
    {
        currentArrayPosition = 0;
        currentArraySize = luxembourgAnthemSize;
        currentNotesArray = luxembourgAnthemNotes;
        currentOffsetsArray = luxembourgAnthemOffsets;

        processNotesArray();
    }
    else if (song == PipiTheme)
    {
        generateEventsFromPROGMEM(pipiTheme, millis());
    }
    else if (song == PommerscheTheme)
    {
        generateEventsFromPROGMEM(pommerscheTheme, millis());
    }
    else if (song == HttydImpro)
    {
        generateEventsFromPROGMEM(httydImpro, millis());
    }
    else
    {
        Serial.println("Tried to play undefined song!");
    }
}

void toggleBuzzer()
{
    isBuzzerEnabled = !isBuzzerEnabled;
    setBuzzerMenuItem(isBuzzerEnabled);
    updateLcd();
}

uint16_t getTotalNotesPlayed() { return totalNotesPlayed; }
uint8_t getLastNotePlayed() { return lastNotePlayed; }