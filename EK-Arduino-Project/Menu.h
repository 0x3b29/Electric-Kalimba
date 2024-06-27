#ifndef MENU_H
#define MENU_H

#include "MenuElement.h"
#include "Arduino.h"

extern bool wasLastdirectionUp;
// Create the menu items
extern MenuElement* currentMenu;

extern MenuElement* mainMenu1;
extern MenuElement* mainMenu2 ;

extern MenuElement* mainMenu3;
extern MenuElement* mainMenu4 ;
extern MenuElement* mainMenu5 ;
extern MenuElement* mainMenu6 ;

extern MenuElement* songMenu1 ;
extern MenuElement* songMenu2 ;
extern MenuElement* songMenu3 ;
extern MenuElement* songMenu4 ;
extern MenuElement* songMenu5 ;
extern MenuElement* songMenuBack ;

extern MenuElement* notesMenu[];

void initializeLcd();
void initializeMenu();
void printToLCD(String firstLine, String secondLine, int selectedLine);
void clearLCDLine(int line);
void preparePrintMenuToLCD();
#endif 