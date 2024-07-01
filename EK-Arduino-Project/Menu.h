#ifndef MENU_H
#define MENU_H

#include "Arduino.h"
#include "MenuElement.h"

void scrollMenuUp();
void scrollMenuDown();

void setMenu(uint8_t menu);
void initializeLcd();
void initializeMenu();
void updateLcd();
void invokeCurrentMenuEvent();

void setBuzzerMenuItem(bool isBuzzerActive);
bool getIsInfoMenuOpen();
void setInfoMenuNotesPlayed(uint16_t notesPlayed);
void setInfoMenuLastNote(uint8_t lastNote);
void setInfoMenuNextNote(uint8_t nextNote);
void setInfoMenuQueuedNotes(uint16_t queuedNotes);

#endif