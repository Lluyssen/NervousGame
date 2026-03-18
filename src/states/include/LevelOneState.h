#ifndef _LEVEL_ONE_STATE_H_
#define _LEVEL_ONE_STATE_H_

#include "raylib.h"
#include "../../ui/petitMenu/PetitMenu.hpp"
#include "../PauseMenuState.hpp"
#include "../MapState.hpp"

class LevelOneState : public IGameState
{
// # PUBLIC FUNCTIONS
public:
    LevelOneState();
    ~LevelOneState();
    void onEnter(StateManager& rStateManager) override;
    void update(StateManager& rStateManager, float dt) override;
    void render(StateManager& rStateManager) override;
    void onExit(StateManager& rStateManager) override;

// # PRIVATE FUNCTIONS
private:
    void displayTowerWheelAnimation(float dt);
    bool didPlayerClickedOnWheel();
    bool didPlayerClickedOnTowerPlace();
    void drawTowerWheelChoice();
    void drawArcherTower();

// # PRIVATE VARIABLES
private:
    Vector2 _ballPosition;
    Vector2 _mousePosition;
    Texture2D _fullTower;
    Texture2D _towerWheelChoiceTexture;
    Texture2D _textureMap;
    Texture2D _archerTowerTexture;
    Rectangle _preciseTower;
    Rectangle _towerWheelChoiceRectangle;
    Rectangle _textureMapRectangle;
    bool _bDoTowerWheelAnimationStarted;
    float _animTime;
    const float _animDuration = 1.0f;
    bool _bIsPlaceTowerWheelIsDisplayed;
    Vector2 _wheelCenter;
    bool _tempoBool;
    bool _bIsArcherTowerDisplayed;
    Vector2 _temporaryMousePositionVector;
};

#endif