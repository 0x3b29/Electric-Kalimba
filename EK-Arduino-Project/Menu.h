#ifndef MENU_H
#define MENU_H

#include "Arduino.h"
#include "MenuElement.h"

extern bool wasLastdirectionUp;
// Create the menu items
extern MenuElement *currentMenu;

extern const MenuElement *mainMenu[];
extern const MenuElement *songMenu[];
extern const MenuElement *notesMenu[];

void initializeLcd();
void initializeMenu();
void printToLCD(String firstLine, String secondLine, int selectedLine);
void clearLCDLine(int line);
void preparePrintMenuToLCD();
void setBuzzerMenuItem(bool isBuzzerActive);
#endif