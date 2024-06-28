#ifndef MUSIC_H
#define MUSIC_H

#include "Arduino.h"

#define NO_NOTE 255

void playNote(uint8_t note);
void buzzerNote(uint8_t note);
void processNotesArray();
void playStairs();

extern uint16_t totalNotesPlayed;
extern uint8_t lastNotePlayed;
extern unsigned long millisLastNotePlayed;
extern bool isBuzzerEnabled;

extern const PROGMEM char ageOfEmpires[];
extern const PROGMEM char pipiTheme[];
extern const PROGMEM char pommerscheTheme[];
extern const PROGMEM char httydImpro[];

extern short currentArrayPosition;
extern short currentArraySize;

extern const unsigned char *currentNotesArray;
extern const unsigned short int *currentOffsetsArray;

extern short int luxembourgAnthemSize;
extern const PROGMEM unsigned char luxembourgAnthemNotes[];
extern const PROGMEM unsigned short int luxembourgAnthemOffsets[];

#endif