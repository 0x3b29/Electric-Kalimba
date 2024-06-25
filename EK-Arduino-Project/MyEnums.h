#ifndef MYENUMS_H
#define MYENUMS_H
    enum EventType
    {
        SetServoPosition,
        PlayNote,
        ProcessCurrentArray,
        SetMenu,
        Init,
        PlaySong,
        ToggleBuzzer,
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
        SongMenu
    };

    enum Song
    {
        LuxembourgAnthem,
        AgeOfEmpiresTheme,
        PipiTheme,
        PommerscheTheme,
    };
#endif 