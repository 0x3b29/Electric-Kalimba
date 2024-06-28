#include "Info.h"
#include "EventManager.h"
#include "Menu.h"
#include "Music.h"

char notesPlayedString[15] = "Played: 0";
char lastNoteString[15] = "Last Note: ?";
char nextNoteString[15] = "Next Note: ?";
char queuedNotesString[15] = "Queued: ?";

long lastScreenUpdate = 0;
long screenUpdateDelay = 250;

unsigned long lastObservedNotesPlayed = 0;
unsigned long lastObservedEventCounter = 0;

void updateNotesPlayed()
{
    snprintf(notesPlayedString, sizeof(notesPlayedString), "Played: %d", totalNotesPlayed);
    infoMenu[0]->setCaption(notesPlayedString);

    if (lastNotePlayed == NO_NOTE)
    {
        snprintf(lastNoteString, sizeof(lastNoteString), "Last Note: ?");
    }
    else
    {
        snprintf(lastNoteString, sizeof(lastNoteString), "Last Note: %d", lastNotePlayed);
    }

    infoMenu[1]->setCaption(lastNoteString);
}

void updateNextNote(uint8_t nextNote)
{
    if (nextNote == NO_NOTE)
    {
        snprintf(nextNoteString, sizeof(nextNoteString), "Next Note: ?");
    }
    else
    {
        snprintf(nextNoteString, sizeof(nextNoteString), "Next Note: %d", nextNote);
    }

    infoMenu[2]->setCaption(nextNoteString);
}

void updateRemainingNotes(uint16_t remainingNotes)
{
    snprintf(queuedNotesString, sizeof(queuedNotesString), "Queue: %d", remainingNotes);

    infoMenu[3]->setCaption(queuedNotesString);
}

void updateInfo()
{
    // Never update info when info is hidden
    if (isInfoMenuOpen == false)
    {
        return;
    }

    bool updateLcd = false;

    // Only update notes played when nessesary
    if (lastObservedEventCounter != eventCounter)
    {
        lastObservedEventCounter = eventCounter;

        updateNextNote(getNextNote());
        updateRemainingNotes(getRemainingNotesCount());

        updateLcd = true;
    }

    // Only update queue info when nessesary
    if (lastObservedNotesPlayed != totalNotesPlayed)
    {
        lastObservedNotesPlayed = totalNotesPlayed;

        updateNotesPlayed();
        updateLcd = true;
    }

    // Only update screen when nessesary
    if (updateLcd == true)
    {
        preparePrintMenuToLCD();
    }
}