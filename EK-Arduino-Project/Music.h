#ifndef MUSIC_H
#define MUSIC_H

#include "Arduino.h"

#define NO_NOTE 255

void playNote(uint8_t note);
void buzzerNoteIfBuzzerEnabled(uint16_t note);
void processNotesArray();
void playStairs();
void playSong(int song);
void toggleBuzzer();

uint16_t getTotalNotesPlayed();
uint8_t getLastNotePlayed();

#endif