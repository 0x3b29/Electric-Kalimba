#include "Info.h"
#include "EventManager.h"
#include "Menu.h"
#include "Music.h"

long lastScreenUpdate = 0;
long screenUpdateDelay = 250;

unsigned long lastObservedNotesPlayed = 0;
unsigned long lastObservedEventCounter = 0;

void updateInfo()
{
    // Never update info when info is hidden
    if (getIsInfoMenuOpen() == false)
    {
        return;
    }

    bool shouldUpdateLcd = false;
    unsigned long processedEventsCounter = getProcessedEventsCounter();
    uint16_t totalNotesPlayed = getTotalNotesPlayed();
    uint8_t lastNotePlayed = getLastNotePlayed();

    // Only update notes played when nessesary
    if (lastObservedEventCounter != processedEventsCounter)
    {
        lastObservedEventCounter = processedEventsCounter;

        setInfoMenuNextNote(getNextNote());
        setInfoMenuQueuedNotes(getRemainingNotesInQueue());
        setInfoMenuRemainingNotes(getRemainingNotes());

        shouldUpdateLcd = true;
    }

    // Only update queue info when nessesary
    if (lastObservedNotesPlayed != totalNotesPlayed)
    {
        lastObservedNotesPlayed = totalNotesPlayed;

        setInfoMenuNotesPlayed(totalNotesPlayed);
        setInfoMenuLastNote(lastNotePlayed);
        shouldUpdateLcd = true;
    }

    // Only update screen when nessesary
    if (shouldUpdateLcd == true)
    {
        updateLcd();
    }
}