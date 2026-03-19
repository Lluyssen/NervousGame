#pragma once

class StateManager;

// Interface de base pour tous les états du jeu
class IGameState
{
public:
    // Lifecycle
    virtual void onEnter(StateManager &) {}
    virtual void onExit(StateManager &) {}

    // Update logique
    virtual void update(StateManager &, float) {}

    // Render graphique
    virtual void render(StateManager &) {}

    // Permet de laisser les états en dessous s'update
    virtual bool allowUpdateBelow(void) const { return false; }

    // Permet de laisser les états en dessous se rendre
    virtual bool allowRenderBelow(void) const { return false; }

    virtual void startLoading(StateManager &) {}
    virtual void updateLoading(StateManager&, float) {}
    virtual bool isReady(void) const { return true; }
};