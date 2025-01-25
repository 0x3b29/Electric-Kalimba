#ifndef ENUMS_H
#define ENUMS_H
enum EventType
{
    SetServoPosition,
    PlayNote,
    ProcessCurrentArray,
    SetMenu,
    Init,
    PlaySong,
    BufferMoreNotes,
    ToggleBuzzer,
    EmptyQueue,
    Info,
};

enum InitOption
{
    Up,
    Down,
    Center
};

enum MenuOptions
{
    MainMenu,
    NotesMenu,
    SongMenu,
    InfoMenu,
};

enum Song
{
    Stairs,
    LuxembourgAnthem,
    AgeOfEmpiresTheme,
    PipiTheme,
    HttydImpro,
    Ouverture,
};
#endif