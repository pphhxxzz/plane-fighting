#pragma once
#include "allClass.h"




int main()
{
    
    srand(time(NULL));
    mciSendString(L"open ./music/game_music.mp3", NULL, 0, NULL);
    mciSendString(L"play ./music/game_music.mp3 repeat", NULL, 0, NULL);
    
    Widget widget(800, 640);
    widget.init();
    widget.run();
    widget.close();
    return 0;
}