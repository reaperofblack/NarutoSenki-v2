#pragma once
#include "GameMode/IGameModeHandler.hpp"

class Mode1v1 : public IGameModeHandler
{
public:
    void init()
    {
    }

    CCArray *onInitHeros(SelectLayer *selectLayer)
    {
        return initHeros(selectLayer);
    }

    void onGameStart()
    {
    }

    void onGameOver()
    {
    }

    void onCharacterInit(CharacterBase *c)
    {
    }

    void onCharacterDead(CharacterBase *c)
    {
    }

    void onCharacterReborn(CharacterBase *c)
    {
    }
};
