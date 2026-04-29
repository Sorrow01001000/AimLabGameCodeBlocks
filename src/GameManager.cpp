#include "GameManager.h"
#include <GL/freeglut.h>
#include <cmath>
#include <cstdio>
#include <cstdlib> // <-- NEW: Added so rand() works for the disco colors!

GameManager::GameManager() {
    currentState = MENU;
    score = 0;
    timeLeft = 30.0f;
    isShooting = false;
    beamTimer = 0;
    cheatActivated = false; // <-- Start with cheat off
}

void GameManager::initLevel1() {
    for (int i = 0; i < MAX_TARGETS; i++) {
        targets[i].spawn();
        targets[i].size = 1.5f;
    }
}

void GameManager::initLevel2() {
    for (int i = 0; i < MAX_TARGETS; i++) {
        targets[i].spawn();
        targets[i].size = 0.8f;
    }
}

void GameManager::update(bool keys[]) {
    if (currentState == LEVEL_1 || currentState == LEVEL_2) {
        player.update(keys);

        if (beamTimer > 0) {
            beamTimer--;
        } else {
            isShooting = false;
        }

        timeLeft -= 16.0f / 1000.0f;
        if (timeLeft <= 0.0f) {
            currentState = GAME_OVER;
        }

        // --- Win Conditions ---
        if (currentState == LEVEL_1 && score >= 100) {
            currentState = LEVEL_1_COMPLETE;
        }
        else if (currentState == LEVEL_2 && score >= 100) {
            currentState = LEVEL_2_COMPLETE;
        }

        // --- NEW: Cheat Code Explosion ---
        if (cheatActivated) {
            for (int i = 0; i < MAX_TARGETS; i++) {
                targets[i].y += 0.5f; // Fly up!
                targets[i].x += (rand() % 10 - 5) * 0.1f; // Scatter X
                targets[i].z += (rand() % 10 - 5) * 0.1f; // Scatter Z
            }
        }
    }
}

void GameManager::shoot() {
    float rad = player.rotation * (3.14159f / 180.0f);
    float dirX = sin(rad);
    float dirZ = cos(rad);

    float maxDistance = 50.0f;
    float stepSize = 0.5f;

    beamStartX = player.x;
    beamStartY = player.y;
    beamStartZ = player.z;

    beamEndX = player.x + dirX * maxDistance;
    beamEndY = player.y;
    beamEndZ = player.z + dirZ * maxDistance;

    isShooting = true;
    beamTimer = 10;

    for (float d = 0; d < maxDistance; d += stepSize) {
        float checkX = player.x + dirX * d;
        float checkY = player.y;
        float checkZ = player.z + dirZ * d;

        for (int i = 0; i < MAX_TARGETS; i++) {
            if (targets[i].active) {
                float diffX = checkX - targets[i].x;
                float diffY = checkY - targets[i].y;
                float diffZ = checkZ - targets[i].z;

                float distance = sqrt(diffX*diffX + diffY*diffY + diffZ*diffZ);

                if (distance < targets[i].size) {
                    targets[i].active = false;
                    beamEndX = checkX;
                    beamEndY = checkY;
                    beamEndZ = checkZ;
                    score += 10;
                    return;
                }
            }
        }
    }
}

void GameManager::drawText(float x, float y, const char* text) {
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);

    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void GameManager::drawGround() {
    // --- NEW: Disco Floor ---
    if (cheatActivated) {
        float r = (rand() % 100) / 100.0f;
        float g = (rand() % 100) / 100.0f;
        float b = (rand() % 100) / 100.0f;
        glColor3f(r, g, b);
    } else {
        glColor3f(0.3f, 0.8f, 0.3f);
    }

    glBegin(GL_QUADS);
        glVertex3f(-50.0f, 0.0f, -50.0f);
        glVertex3f(-50.0f, 0.0f,  50.0f);
        glVertex3f( 50.0f, 0.0f,  50.0f);
        glVertex3f( 50.0f, 0.0f, -50.0f);
    glEnd();
}

void GameManager::draw() {
    if (currentState == MENU) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        int w = glutGet(GLUT_WINDOW_WIDTH);
        int h = glutGet(GLUT_WINDOW_HEIGHT);

        drawText(w / 2.0f - 100.0f, h / 2.0f + 20.0f, "ACADEMIC AIM TRAINER");
        drawText(w / 2.0f - 80.0f, h / 2.0f - 20.0f, "Press ENTER to Start");
    }
    else if (currentState == LEVEL_1 || currentState == LEVEL_2) {
        glClearColor(0.5f, 0.8f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        float cameraDistance = 6.0f;
        float cameraHeight = 3.0f;
        float rad = player.rotation * (3.14159f / 180.0f);
        float camX = player.x - sin(rad) * cameraDistance;
        float camZ = player.z - cos(rad) * cameraDistance;
        float camY = player.y + cameraHeight;
        float lookX = player.x + sin(rad);
        float lookZ = player.z + cos(rad);

        gluLookAt(camX, camY, camZ, lookX, player.y, lookZ, 0.0f, 1.0f, 0.0f);

        drawGround();

        for (int i = 0; i < MAX_TARGETS; i++) targets[i].draw();

        player.draw();

        if (isShooting) {
            glColor3f(1.0f, 1.0f, 0.0f);
            glLineWidth(3.0f);
            glBegin(GL_LINES);
                glVertex3f(beamStartX, beamStartY, beamStartZ);
                glVertex3f(beamEndX, beamEndY, beamEndZ);
            glEnd();
            glLineWidth(1.0f);
        }

        char hudText[100];
        snprintf(hudText, sizeof(hudText), "Score: %d   Time Left: %.1f", score, timeLeft);
        drawText(10.0f, glutGet(GLUT_WINDOW_HEIGHT) - 30.0f, hudText);
    }
    else if (currentState == LEVEL_1_COMPLETE) {
        glClearColor(0.2f, 0.6f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        int w = glutGet(GLUT_WINDOW_WIDTH);
        int h = glutGet(GLUT_WINDOW_HEIGHT);

        drawText(w / 2.0f - 80.0f, h / 2.0f + 20.0f, "LEVEL 1 COMPLETE!");
        drawText(w / 2.0f - 110.0f, h / 2.0f - 20.0f, "Press ENTER to Start Level 2");
    }
    else if (currentState == LEVEL_2_COMPLETE) {
        glClearColor(0.8f, 0.6f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        int w = glutGet(GLUT_WINDOW_WIDTH);
        int h = glutGet(GLUT_WINDOW_HEIGHT);

        drawText(w / 2.0f - 100.0f, h / 2.0f + 20.0f, "YOU BEAT LEVEL 2!");
        drawText(w / 2.0f - 100.0f, h / 2.0f - 20.0f, "Press ESC to Exit");
    }
    else if (currentState == GAME_OVER) {
        glClearColor(0.8f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        int w = glutGet(GLUT_WINDOW_WIDTH);
        int h = glutGet(GLUT_WINDOW_HEIGHT);

        char finalScoreText[50];
        snprintf(finalScoreText, sizeof(finalScoreText), "FINAL SCORE: %d", score);

        drawText(w / 2.0f - 50.0f, h / 2.0f + 20.0f, "TIME'S UP!");
        drawText(w / 2.0f - 70.0f, h / 2.0f - 20.0f, finalScoreText);
    }
}
