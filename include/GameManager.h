#pragma once
#include <GL/freeglut.h>
#include "Player.h"
#include "Target.h"

#define MAX_TARGETS 10

enum GameState {
    MENU,
    LEVEL_SELECT,
    SETTINGS,
    LEVEL_1,
    LEVEL_2,
    LEVEL_1_COMPLETE,
    LEVEL_2_COMPLETE,
    GAME_OVER
};

class GameManager {
public:
    GameState currentState;
    Player player;
    Target targets[MAX_TARGETS];
    GLuint groundTexture;
    GLuint skyTexture;
    GLuint loadTexture(const char* filename);

    float treePositions[6][2];
    void drawTree(float x, float z);

    int score;
    float timeLeft;

    // Laser visuals
    bool isShooting;
    float beamStartX, beamStartY, beamStartZ;
    float beamEndX, beamEndY, beamEndZ;
    int beamTimer;

    // --- NEW: Cheat Code Variable ---
    bool cheatActivated;

    // --- Invertible Map ---
    bool mapInverted;

    GameManager(); // Constructor
    void init();
    // Game loop functions
    void initLevel1();
    void initLevel2();
    void update(bool keys[]);
    void draw();
    void shoot();
    void handleMenuInput(unsigned char key);


    // Helper drawing functions
    void drawText(float x, float y, const char* text);
    void drawGround();
    void drawSky();
};
