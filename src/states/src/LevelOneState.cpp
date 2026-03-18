#include "../include/LevelOneState.h"

LevelOneState::LevelOneState()
{
    _ballPosition.x = 100.0;
    _ballPosition.y = 100.0;
    _mousePosition = _ballPosition;
    _textureMap = LoadTexture("../assets/temporary_sprites/LevelOneMap.png");
    _towerWheelChoiceTexture = LoadTexture("../assets/temporary_sprites/TowerWheelChoice.png");
    _fullTower = LoadTexture("../assets/temporary_sprites/Chat_GPT_Towers_SpriteSheet.png");
    _archerTowerTexture = LoadTexture("../assets/temporary_sprites/ArcherTowers.png");
    _preciseTower.x = 150;      // position X dans le sprite
    _preciseTower.y = 55;      // position Y dans le sprite
    _preciseTower.width = 320; // largeur du sprite
    _preciseTower.height = 440; // hauteur du sprite
    _towerWheelChoiceRectangle = { 0, 0, static_cast<float>(_towerWheelChoiceTexture.width), static_cast<float>(_towerWheelChoiceTexture.height) };
    _textureMapRectangle = { 0, 0, static_cast<float>(_textureMap.width), static_cast<float>(_textureMap.height) };
    _bDoTowerWheelAnimationStarted = false;
    _animTime = 0.0f;
    _bIsPlaceTowerWheelIsDisplayed = false;
    _tempoBool = false;
    _bIsArcherTowerDisplayed = false;
}

LevelOneState::~LevelOneState()
{
    _ballPosition.x = 100.0;
    _ballPosition.y = 100.0;
    _mousePosition = _ballPosition;
    UnloadTexture(_fullTower);
    UnloadTexture(_towerWheelChoiceTexture);
    UnloadTexture(_textureMap);
    UnloadTexture(_archerTowerTexture);
    _bDoTowerWheelAnimationStarted = false;
    _animTime = 0.0f;
    _tempoBool = false;
    _bIsPlaceTowerWheelIsDisplayed = false;
    _bIsArcherTowerDisplayed = false;
}

void LevelOneState::onEnter(StateManager &sm)
{
    Vector2 ballPosition = { -100.0f, -100.0f };

    Rectangle dest = { 0, 0, 1280, 800 };
    // clear previous screen draws

    BeginDrawing();

    ShowCursor();
    ClearBackground(RAYWHITE);
    DrawTexturePro(_textureMap, _textureMapRectangle, dest, {0,0}, 0.0f, WHITE);

    EndDrawing();
}

void LevelOneState::displayTowerWheelAnimation(float dt)
{
    if (_bDoTowerWheelAnimationStarted)
    {
        _animTime += dt;

        if (_animTime >= _animDuration)
        {
            _animTime = _animDuration;
            _bDoTowerWheelAnimationStarted = false;
        }
        else
        {
            float t = _animTime / _animDuration;
            float scale = t;

            Rectangle source = { 0, 0, static_cast<float>(_towerWheelChoiceTexture.width), static_cast<float>(_towerWheelChoiceTexture.height) };

            float finalSize = 256;

            Rectangle dest = { 550.0f, 430.0f, finalSize * scale, finalSize * scale };

            Vector2 origin = { dest.width / 2, dest.height / 2 };

            DrawTexturePro(_towerWheelChoiceTexture, source, dest, origin, 0.0f, WHITE);
        }
    }
    else
    {
        _bDoTowerWheelAnimationStarted = true;
        displayTowerWheelAnimation(dt);
    }
}

// true -> on affiche la tour || false -> on degage la roue (dans tous les cas elle degage en fait)
bool LevelOneState::didPlayerClickedOnWheel()
{
    bool retVal { false };

    float wheelSize = 256;
    Rectangle topLeft = { _wheelCenter.x - 64, _wheelCenter.y - wheelSize/2, 64, 64 };
    Rectangle topRight = { _wheelCenter.x, _wheelCenter.y - wheelSize/2, 64, 64 };
    Rectangle bottom = { _wheelCenter.x - 32, _wheelCenter.y + wheelSize/2 - 64, 64, 64 };
    Rectangle left = { _wheelCenter.x - wheelSize/2, _wheelCenter.y - 32, 64, 64 };
    Rectangle right = { _wheelCenter.x + wheelSize/2 - 64, _wheelCenter.y - 32, 64, 64 };

    DrawCircleV(_mousePosition, 5, BLUE);
    DrawRectangleLinesEx(topLeft, 2, RED);
    DrawRectangleLinesEx(topRight, 2, RED);
    DrawRectangleLinesEx(bottom, 2, RED);
    DrawRectangleLinesEx(left, 2, RED);
    DrawRectangleLinesEx(right, 2, RED);

    std::cout << "normalement le top c'est x = " << _mousePosition.x - 32 << " y = " << _mousePosition.y << '\n';

    std::cout << "_mousePosition pour savoir si on clique dans le carre rouge = x" << _mousePosition.x << ", y = " << _mousePosition.y << '\n';

    // la on check si un click dans une des boites est fait pour afficher la tour
    if (CheckCollisionPointRec(_mousePosition, topLeft))
    {
        std::cout << "TOP LEFT\n";
        retVal = true;
    }
    if (CheckCollisionPointRec(_mousePosition, topRight))
    {
        std::cout << "TOP RIGHT\n";
        retVal = true;
    }
    if (CheckCollisionPointRec(_mousePosition, bottom))
    {
        std::cout << "BOTTOM\n";
        retVal = true;
    }
    if (CheckCollisionPointRec(_mousePosition, left))
    {
        std::cout << "LEFT\n";
        retVal = true;
    }
    if (CheckCollisionPointRec(_mousePosition, right))
    {
        std::cout << "RIGHT\n";
        retVal = true;
    }

    return retVal;
}

// true -> on affiche la wheel || false y a r
bool LevelOneState::didPlayerClickedOnTowerPlace()
{
    bool retVal { false };

    if ((_mousePosition.x >= 539 && _mousePosition.x <= 611) && (_mousePosition.y >= 413 && _mousePosition.y <= 452))
    {
        retVal = true;
    }

    return retVal;
}

void LevelOneState::drawTowerWheelChoice()
{
    Rectangle dest = { 580, 434, 256, 256 };
    DrawTexturePro(_towerWheelChoiceTexture, _towerWheelChoiceRectangle, dest, {static_cast<float>(_towerWheelChoiceTexture.width)/2.0f, static_cast<float>(_towerWheelChoiceTexture.height)/2.2f}, 0.0f, WHITE);
}

void LevelOneState::drawArcherTower()
{
    Rectangle origin = { 0, 0, 200.0, static_cast<float>(_archerTowerTexture.height) };
    Rectangle dest = { 800.0f, 360.0f, 256, 256 };
    DrawTexturePro(_archerTowerTexture, origin, dest, {static_cast<float>(_archerTowerTexture.width)/2.0f, static_cast<float>(_archerTowerTexture.height)/2.2f}, 0.0f, WHITE);
}

void LevelOneState::update(StateManager &, float dt)
{
    // draw background map
    Rectangle dest = { 0, 0, 1280, 800 };
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawTexturePro(_textureMap, _textureMapRectangle, dest, { 0, 0 }, 0.0f, WHITE);

    if (_bIsPlaceTowerWheelIsDisplayed)
    {
        drawTowerWheelChoice();
    }
    else if (_bIsArcherTowerDisplayed)
    {
        drawArcherTower();
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        _mousePosition = GetMousePosition();

        // check si la roue pour poser la tour est visible ou non
        if (_bIsPlaceTowerWheelIsDisplayed)
        {
            // check si le joueur a clique sur la roue ou non pour poser une tour
            if (didPlayerClickedOnWheel())
            {
                _bIsPlaceTowerWheelIsDisplayed = false;
                _bIsArcherTowerDisplayed = true;
                _temporaryMousePositionVector = _mousePosition;
                DrawTexturePro(_textureMap, _textureMapRectangle, dest, { 0, 0 }, 0.0f, WHITE);

                // ici faut placer une tourelle et faire disparaitre la roue
            }
            // faire disparaitre la wheel
            _bIsPlaceTowerWheelIsDisplayed = false;
        }
        else
        {
            if (didPlayerClickedOnTowerPlace())
            {
                _wheelCenter = _mousePosition;
                _bIsPlaceTowerWheelIsDisplayed = true;
            }
        }
    }

    EndDrawing();
}

void LevelOneState::onExit(StateManager &) {};
