#pragma once
#include "Player.h"
#include "Target.h"

#define MAX_TARGETS 10

enum GameState {
    MENU,
    LEVEL_1,
    LEVEL_1_COMPLETE,
    LEVEL_2,
    LEVEL_2_COMPLETE,
    GAME_OVER
};

class GameManager {
public:
    GameState currentState;
    Player player;
    Target targets[MAX_TARGETS];

    int score;
    float timeLeft;

    // Laser visuals
    bool isShooting;
    float beamStartX, beamStartY, beamStartZ;
    float beamEndX, beamEndY, beamEndZ;
    int beamTimer;

    // --- NEW: Cheat Code Variable ---
    bool cheatActivated;

    GameManager(); // Constructor

    // Game loop functions
    void initLevel1();
    void initLevel2();
    void update(bool keys[]);
    void draw();
    void shoot();

    // Helper drawing functions
    void drawText(float x, float y, const char* text);
    void drawGround();
};
