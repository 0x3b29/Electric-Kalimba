#include "Menu.h"
#include <LiquidCrystal.h>

// Prepare the LCD
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

String selectedPrefix = "> ";
String emptyPrefix = "  ";
bool wasLastdirectionUp = false;

// Create the menu items
MenuElement *currentMenu;

MenuElement *mainMenu1 = new MenuElement((char *)"Play Notes", SetMenu, NotesMenu);
MenuElement *mainMenu2 = new MenuElement((char *)"Play Songs", SetMenu, SongMenu);

MenuElement *mainMenu3 = new MenuElement((char *)"Init All Up", Init, Up);
MenuElement *mainMenu4 = new MenuElement((char *)"Init All Down", Init, Down);
MenuElement *mainMenu5 = new MenuElement((char *)"Init All Center", Init, Center);
MenuElement *mainMenu6 = new MenuElement((char *)"Buzzer is Off", ToggleBuzzer, 0);

MenuElement *songMenu1 = new MenuElement((char *)"Age of Empires", PlaySong, AgeOfEmpiresTheme);
MenuElement *songMenu2 = new MenuElement((char *)"Luxebourg Anth.", PlaySong, LuxembourgAnthem);
MenuElement *songMenu3 = new MenuElement((char *)"Pipi Theme", PlaySong, PipiTheme);
MenuElement *songMenu4 = new MenuElement((char *)"Pommersche", PlaySong, PommerscheTheme);
MenuElement *songMenu5 = new MenuElement((char *)"Romantic Flight", PlaySong, RomanticFlight);
MenuElement *songMenuBack = new MenuElement((char *)"Back", SetMenu, MainMenu);

MenuElement *notesMenu[17];
const char notesArrayText[][13] = {"Play Note 1",  "Play Note 2",  "Play Note 3",  "Play Note 4",  "Play Note 5",  "Play Note 6",
                                   "Play Note 7",  "Play Note 8",  "Play Note 9",  "Play Note 10", "Play Note 11", "Play Note 12",
                                   "Play Note 13", "Play Note 14", "Play Note 15", "Play Note 16", "Play Note 17"};

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

void initializeMenu()
{
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
    songMenu4->bottomNeighbour = songMenu5;

    songMenu5->topNeighbour = songMenu4;
    songMenu5->bottomNeighbour = songMenuBack;

    songMenuBack->topNeighbour = songMenu5;
    songMenuBack->bottomNeighbour = songMenu1;

    // Create all the entries for the notes Menu
    for (int i = 0; i < 17; i++)
    {
        notesMenu[i] = new MenuElement((char *)notesArrayText[i], PlayNote, i + 1);
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
}