#include "Menu.h"
#include "EventManager.h"
#include "Music.h"
#include <LiquidCrystal.h>

// Forward declarations
void printToLCD(String firstLine, String secondLine, int selectedLine);
void clearLCDLine(int line);

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

String selectedPrefix = "> ";
String emptyPrefix = "  ";
bool wasLastdirectionUp = false;

// Create the menu items
MenuElement *currentMenu;

const MenuElement *mainMenu[] = {new MenuElement((char *)"Play Songs", SetMenu, new (EventArg[1]){EventArg(SongMenu)}),
                                 new MenuElement((char *)"Play Notes", SetMenu, new (EventArg[1]){EventArg(NotesMenu)}),
                                 new MenuElement((char *)"Play Stair", PlaySong, new (EventArg[1]){EventArg(Stairs)}),
                                 new MenuElement((char *)"Init All Up", Init, new (EventArg[1]){EventArg(Up)}),
                                 new MenuElement((char *)"Init All Down", Init, new (EventArg[1]){EventArg(Down)}),
                                 new MenuElement((char *)"Init All Center", Init, new (EventArg[1]){EventArg(Center)}),
                                 new MenuElement((char *)"Show Info", SetMenu, new (EventArg[1]){EventArg(InfoMenu)}),
                                 new MenuElement((char *)"Buzzer is Off", ToggleBuzzer, NULL)};

const MenuElement *songMenu[] = {new MenuElement((char *)"Age of Empires", PlaySong, new (EventArg[1]){EventArg(AgeOfEmpiresTheme)}),
                                 new MenuElement((char *)"Luxebourg Anth.", PlaySong, new (EventArg[1]){EventArg(LuxembourgAnthem)}),
                                 new MenuElement((char *)"Pipi Theme", PlaySong, new (EventArg[1]){EventArg(PipiTheme)}),
                                 new MenuElement((char *)"Pommersche", PlaySong, new (EventArg[1]){EventArg(PommerscheTheme)}),
                                 new MenuElement((char *)"HTTYD Impro", PlaySong, new (EventArg[1]){EventArg(HttydImpro)}),
                                 new MenuElement((char *)">> Stop All <<", EmptyQueue, NULL),
                                 new MenuElement((char *)"Back", SetMenu, new (EventArg[1]){EventArg(MainMenu)})};

const MenuElement *infoMenu[] = {
    new MenuElement((char *)"Played 0", Info, NULL),    new MenuElement((char *)"Remaining 0", Info, NULL),
    new MenuElement((char *)"Last Note ?", Info, NULL), new MenuElement((char *)"Next Note ?", Info, NULL),
    new MenuElement((char *)"Queue 0", Info, NULL),     new MenuElement((char *)"Back", SetMenu, new (EventArg[1]){EventArg(MainMenu)})};

const MenuElement *notesMenu[17];
const char notesLabels[][20] = {"Play Note 1",  "Play Note 2",  "Play Note 3",  "Play Note 4",  "Play Note 5",  "Play Note 6",
                                "Play Note 7",  "Play Note 8",  "Play Note 9",  "Play Note 10", "Play Note 11", "Play Note 12",
                                "Play Note 13", "Play Note 14", "Play Note 15", "Play Note 16", "Play Note 17"};

bool isInfoMenuOpen = false;
char notesPlayedString[15] = "Played 0";
char remainingNotesString[15] = "Remaining 0";
char lastNoteString[15] = "Last Note ?";
char nextNoteString[15] = "Next Note ?";
char queuedNotesString[15] = "Queued ?";

void setMenu(uint8_t menu)
{
    isInfoMenuOpen = false;

    if (menu == NotesMenu)
    {
        currentMenu = notesMenu[0];
    }
    else if (menu == SongMenu)
    {
        currentMenu = songMenu[0];
    }
    else if (menu == MainMenu)
    {
        currentMenu = mainMenu[0];
    }
    else if (menu == InfoMenu)
    {
        currentMenu = infoMenu[0];
        isInfoMenuOpen = true;
    }
    else
    {
        Serial.print("Unknown menu option: '");
        Serial.print(menu);
        Serial.println("'");
    }

    printToLCD(currentMenu->caption, currentMenu->bottomNeighbour->caption, 0);
}

void setBuzzerMenuItem(bool isBuzzerActive)
{
    if (isBuzzerActive)
    {
        mainMenu[7]->setCaption((char *)"Buzzer is On");
    }
    else
    {
        mainMenu[7]->setCaption((char *)"Buzzer is Off");
    }
}

void initializeLcd()
{
    // Initialize lcd with size
    lcd.begin(16, 2);

    clearLCDLine(0);
    clearLCDLine(1);
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

void clearLCDLine(int line)
{
    lcd.setCursor(0, line);

    for (int n = 0; n < 16; n++)
    {
        lcd.print(" ");
    }
}

void updateLcd()
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

void linkMenuItems(MenuElement *menuElements[], uint8_t menuElementsLength)
{
    // Link all the inner notes menu items togeather
    for (int i = 1; i < menuElementsLength - 1; i++)
    {
        menuElements[i]->topNeighbour = menuElements[i - 1];
        menuElements[i]->bottomNeighbour = menuElements[i + 1];
    }

    // Link all the extreme notes menu items togeather
    menuElements[0]->topNeighbour = menuElements[menuElementsLength - 1];
    menuElements[0]->bottomNeighbour = menuElements[1];

    menuElements[menuElementsLength - 1]->topNeighbour = menuElements[menuElementsLength - 2];
    menuElements[menuElementsLength - 1]->bottomNeighbour = menuElements[0];
}

void initializeMenu()
{
    linkMenuItems(mainMenu, sizeof(mainMenu) / sizeof(mainMenu[0]));
    linkMenuItems(songMenu, sizeof(songMenu) / sizeof(songMenu[0]));
    linkMenuItems(infoMenu, sizeof(infoMenu) / sizeof(infoMenu[0]));

    // Create all the entries for the notes Menu
    for (int i = 0; i < 17; i++)
    {
        notesMenu[i] = new MenuElement(notesLabels[i], PlayNote, new (EventArg[1]){EventArg(i + 1)});
    }

    linkMenuItems(notesMenu, 17);

    // Set entry node of menu
    currentMenu = mainMenu[0];

    // Output menu
    printToLCD(currentMenu->caption, currentMenu->bottomNeighbour->caption, 0);
}

bool getIsInfoMenuOpen() { return isInfoMenuOpen; }

void setInfoMenuNotesPlayed(uint16_t notesPlayed)
{
    snprintf(notesPlayedString, sizeof(notesPlayedString), "Played %d", notesPlayed);
    infoMenu[0]->setCaption(notesPlayedString);
}

void setInfoMenuLastNote(uint8_t lastNotePlayed)
{

    if (lastNotePlayed == NO_NOTE)
    {
        snprintf(lastNoteString, sizeof(lastNoteString), "Last Note ?");
    }
    else
    {
        snprintf(lastNoteString, sizeof(lastNoteString), "Last Note %d", lastNotePlayed);
    }

    infoMenu[2]->setCaption(lastNoteString);
}

void setInfoMenuNextNote(uint8_t nextNote)
{
    if (nextNote == NO_NOTE)
    {
        snprintf(nextNoteString, sizeof(nextNoteString), "Next Note ?");
    }
    else
    {
        snprintf(nextNoteString, sizeof(nextNoteString), "Next Note %d", nextNote);
    }

    infoMenu[3]->setCaption(nextNoteString);
}

void setInfoMenuQueuedNotes(uint16_t queuedNotes)
{
    snprintf(queuedNotesString, sizeof(queuedNotesString), "Queue %d", queuedNotes);

    infoMenu[4]->setCaption(queuedNotesString);
}

void setInfoMenuRemainingNotes(uint16_t remainingNotes)
{
    snprintf(remainingNotesString, sizeof(remainingNotesString), "Remaining %d", remainingNotes);
    infoMenu[1]->setCaption(remainingNotesString);
}

void scrollMenuUp()
{
    currentMenu = currentMenu->topNeighbour;
    wasLastdirectionUp = true;
    updateLcd();
}

void scrollMenuDown()
{
    currentMenu = currentMenu->bottomNeighbour;
    wasLastdirectionUp = false;
    updateLcd();
}

void invokeCurrentMenuEvent() { parseEvent(currentMenu->eventType, currentMenu->eventArgs); }