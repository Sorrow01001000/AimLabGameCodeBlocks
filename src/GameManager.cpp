#include "GameManager.h"
#include <GL/freeglut.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <windows.h>
#include <mmsystem.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#pragma comment(lib, "winmm.lib")

GameManager::GameManager() {
    currentState = MENU;
    score = 0;
    timeLeft = 30.0f;
    isShooting = false;
    beamTimer = 0;
    cheatActivated = false;
    cameraDistance = 6.0f;
}

void GameManager::init() {
    groundTexture = loadTexture("C:\\Users\\omar3\\Documents\\sections graphics\\start\\grassfield.jpg");
    skyTexture = loadTexture("C:\\Users\\omar3\\Documents\\sections graphics\\start\\brickwall.jpg");

    treePositions[0][0] = -20.0f; treePositions[0][1] = -20.0f;
    treePositions[1][0] =  15.0f; treePositions[1][1] = -25.0f;
    treePositions[2][0] = -30.0f; treePositions[2][1] =  15.0f;
    treePositions[3][0] =  25.0f; treePositions[3][1] =  20.0f;
    treePositions[4][0] =   0.0f; treePositions[4][1] = -35.0f;
    treePositions[5][0] = -10.0f; treePositions[5][1] =  30.0f;
}

void GameManager::handleMenuInput(unsigned char key) {
    if (key == 27) {
        if (currentState == LEVEL_1 || currentState == LEVEL_2) {
            currentState = MENU;
            mciSendString("stop boomMusic", NULL, 0, NULL);
        }
        else if (currentState == MENU) {
            exit(0);
        }
        else {
            currentState = MENU;
        }
        return;
    }

    if (currentState == MENU) {
        if (key == '1') currentState = LEVEL_SELECT;
        if (key == '2') currentState = SETTINGS;
        if (key == '3') exit(0);
    }
    else if (currentState == LEVEL_SELECT) {
        if (key == '1') {
            score = 0;
            timeLeft = 30.0f;
            initLevel1();
            currentState = LEVEL_1;
        }
        if (key == '2') {
            score = 0;
            timeLeft = 30.0f;
            initLevel2();
            currentState = LEVEL_2;
        }
        if (key == '3' || key == 'b' || key == 'B') {
            currentState = MENU;
        }
    }
    else if (currentState == SETTINGS) {
        if (key == 'b' || key == 'B') currentState = MENU;
    }
    else if (currentState == LEVEL_1_COMPLETE) {
        if (key == 13) {
            score = 0;
            timeLeft = 30.0f;
            initLevel2();
            currentState = LEVEL_2;
        }
    }
    else if (currentState == GAME_OVER || currentState == LEVEL_2_COMPLETE) {
        if (key == 13 || key == 'b' || key == 'B') currentState = MENU;
    }
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

        float time = glutGet(GLUT_ELAPSED_TIME) * 0.003f;

        for (int i = 0; i < MAX_TARGETS; i++) {
            if (targets[i].active) {
                targets[i].y += sin(time + i) * 0.02f;
                float slideSpeed = 0.6f + (i % 3) * 0.25f;
                float slideRange = 4.0f;
                targets[i].x = targets[i].spawnX + sin(time * slideSpeed + i * 1.3f) * slideRange;
            }
        }

        if (beamTimer > 0) {
            beamTimer--;
        } else {
            isShooting = false;
        }

        timeLeft -= 16.0f / 1000.0f;
        if (timeLeft <= 0.0f) {
            currentState = GAME_OVER;
            mciSendString("stop boomMusic", NULL, 0, NULL);
        }

        if (currentState == LEVEL_1 && score >= 100) {
            currentState = LEVEL_1_COMPLETE;
            mciSendString("stop boomMusic", NULL, 0, NULL);
        }
        else if (currentState == LEVEL_2 && score >= 100) {
            currentState = LEVEL_2_COMPLETE;
            mciSendString("stop boomMusic", NULL, 0, NULL);
        }

        if (cheatActivated) {
            static bool musicStarted = false;
            if (!musicStarted) {
                mciSendString("stop boomMusic", NULL, 0, NULL);
                mciSendString("close boomMusic", NULL, 0, NULL);
                mciSendString("open \"ayay.wav\" alias boomMusic wait", NULL, 0, NULL);
                mciSendString("play boomMusic repeat", NULL, 0, NULL);
                musicStarted = true;
            }

            for (int i = 0; i < MAX_TARGETS; i++) {
                targets[i].y += 0.5f;
                targets[i].x += (rand() % 10 - 5) * 0.1f;
                targets[i].z += (rand() % 10 - 5) * 0.1f;
            }
        }
    }
}

void GameManager::shoot() {
    PlaySound(TEXT("boxing_bell.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);

    float rad = player.rotation * (3.14159f / 180.0f);
    float dirX = sin(rad);
    float dirZ = cos(rad);

    // Perpendicular (right side)
    float perpX =  cos(rad);
    float perpZ = -sin(rad);

    // AK muzzle position matches exactly what's in Player::draw()
    // Gun base: (0.08 right, 0.88 up, 0.26 forward)
    // Muzzle tip adds another ~0.68 forward in local Z
    float muzzleRight   = 0.08f;  // gun is slightly right of center
    float muzzleHeight  = 0.90f;  // y=0.88 + slight tilt
    float muzzleForward = 0.94f;  // 0.26 base + 0.68 barrel length

    beamStartX = player.x + perpX * muzzleRight + dirX * muzzleForward;
    beamStartY = player.y + muzzleHeight;
    beamStartZ = player.z + perpZ * muzzleRight + dirZ * muzzleForward;

    beamEndX = beamStartX + dirX * 50.0f;
    beamEndY = beamStartY;
    beamEndZ = beamStartZ + dirZ * 50.0f;

    isShooting = true;
    beamTimer = 10;

    for (float d = 0; d < 50.0f; d += 0.5f) {
        float checkX = beamStartX + dirX * d;
        float checkY = beamStartY;
        float checkZ = beamStartZ + dirZ * d;

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
    if (cheatActivated) {
        glDisable(GL_TEXTURE_2D);
        float r = (rand() % 100) / 100.0f;
        float g = (rand() % 100) / 100.0f;
        float b = (rand() % 100) / 100.0f;
        glColor3f(r, g, b);
    } else {
        glColor3f(1.0f, 1.0f, 1.0f);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, groundTexture);
    }

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f,  0.0f);  glVertex3f(-50.0f, 0.0f, -50.0f);
        glTexCoord2f(0.0f,  10.0f); glVertex3f(-50.0f, 0.0f,  50.0f);
        glTexCoord2f(10.0f, 10.0f); glVertex3f( 50.0f, 0.0f,  50.0f);
        glTexCoord2f(10.0f, 0.0f);  glVertex3f( 50.0f, 0.0f, -50.0f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void GameManager::drawSky() {
    if (!cheatActivated) {
        glColor3f(1.0f, 1.0f, 1.0f);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, skyTexture);
    }

    float s = 50.0f;
    float t = 3.0f;

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-s,  0.0f, -s);
        glTexCoord2f(t,    0.0f); glVertex3f( s,  0.0f, -s);
        glTexCoord2f(t,    t);    glVertex3f( s,   s,   -s);
        glTexCoord2f(0.0f, t);    glVertex3f(-s,   s,   -s);

        glTexCoord2f(0.0f, 0.0f); glVertex3f( s,  0.0f,  s);
        glTexCoord2f(t,    0.0f); glVertex3f(-s,  0.0f,  s);
        glTexCoord2f(t,    t);    glVertex3f(-s,   s,    s);
        glTexCoord2f(0.0f, t);    glVertex3f( s,   s,    s);

        glTexCoord2f(0.0f, 0.0f); glVertex3f(-s,  0.0f,  s);
        glTexCoord2f(t,    0.0f); glVertex3f(-s,  0.0f, -s);
        glTexCoord2f(t,    t);    glVertex3f(-s,   s,   -s);
        glTexCoord2f(0.0f, t);    glVertex3f(-s,   s,    s);

        glTexCoord2f(0.0f, 0.0f); glVertex3f( s,  0.0f, -s);
        glTexCoord2f(t,    0.0f); glVertex3f( s,  0.0f,  s);
        glTexCoord2f(t,    t);    glVertex3f( s,   s,    s);
        glTexCoord2f(0.0f, t);    glVertex3f( s,   s,   -s);

        glTexCoord2f(0.0f, 0.0f); glVertex3f(-s,   s,   -s);
        glTexCoord2f(t,    0.0f); glVertex3f( s,   s,   -s);
        glTexCoord2f(t,    t);    glVertex3f( s,   s,    s);
        glTexCoord2f(0.0f, t);    glVertex3f(-s,   s,    s);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void GameManager::drawTree(float x, float z) {
    glPushMatrix();
        glTranslatef(x, 0.0f, z);
        glDisable(GL_TEXTURE_2D);

        glColor3f(0.4f, 0.2f, 0.0f);
        glPushMatrix();
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            glutSolidCylinder(0.8f, 2.0f, 15, 15);
        glPopMatrix();

        glColor3f(0.0f, 0.5f, 0.1f);
        glPushMatrix();
            glTranslatef(0.0f, 2.0f, 0.0f);
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            glutSolidCone(2.5f, 4.0f, 15, 15);
        glPopMatrix();

        glColor3f(1.0f, 1.0f, 1.0f);
    glPopMatrix();
}

GLuint GameManager::loadTexture(const char* filename) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    } else {
        printf("Failed to load texture: %s\n", filename);
    }

    stbi_image_free(data);
    return textureID;
}

void GameManager::draw() {
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    if (currentState == MENU) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        drawText(w / 2.0f - 90.0f, h / 2.0f + 60.0f, "BEAM OF MADNESS");
        drawText(w / 2.0f - 60.0f, h / 2.0f + 10.0f, "1. Start Game");
        drawText(w / 2.0f - 60.0f, h / 2.0f - 20.0f, "2. Settings & Rules");
        drawText(w / 2.0f - 60.0f, h / 2.0f - 50.0f, "3. Exit Game");
    }
    else if (currentState == LEVEL_SELECT) {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        drawText(w / 2.0f - 70.0f, h / 2.0f + 60.0f, "SELECT LEVEL");
        drawText(w / 2.0f - 60.0f, h / 2.0f + 10.0f, "1. Level 1 (Easy)");
        drawText(w / 2.0f - 60.0f, h / 2.0f - 20.0f, "2. Level 2 (Hard)");
        drawText(w / 2.0f - 60.0f, h / 2.0f - 50.0f, "3. Back to Menu");
    }
    else if (currentState == SETTINGS) {
        glClearColor(0.2f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        drawText(w / 2.0f - 70.0f, h / 2.0f + 80.0f, "HOW TO PLAY:");
        drawText(w / 2.0f - 120.0f, h / 2.0f + 40.0f, "- Use W, A, S, D to move, Space to Jump");
        drawText(w / 2.0f - 120.0f, h / 2.0f + 10.0f, "- Mouse aim, Left Click to shoot.");
        drawText(w / 2.0f - 120.0f, h / 2.0f - 20.0f, "- Destroy all targets before time runs out!");
        drawText(w / 2.0f - 120.0f, h / 2.0f - 50.0f, "- Secret: Type 'boom' for MADNESS MODE.");
        drawText(w / 2.0f - 70.0f, h / 2.0f - 100.0f, "Press B to go Back");
    }
    else if (currentState == LEVEL_1 || currentState == LEVEL_2) {
        glClearColor(0.5f, 0.8f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();


        float cameraDistance = this->cameraDistance;
        float cameraHeight = 3.0f;
        float rad = player.rotation * (3.14159f / 180.0f);
        float camX = player.x - sin(rad) * cameraDistance;
        float camZ = player.z - cos(rad) * cameraDistance;
        float camY = player.y + cameraHeight;
        float lookX = player.x + sin(rad);
        float lookZ = player.z + cos(rad);

        gluLookAt(camX, camY, camZ, lookX, player.y, lookZ, 0.0f, 1.0f, 0.0f);

        drawGround();
        drawSky();

        for (int i = 0; i < 6; i++) drawTree(treePositions[i][0], treePositions[i][1]);
        for (int i = 0; i < MAX_TARGETS; i++) targets[i].draw();
        player.draw();

        if (isShooting) {
            glColor3f(1.0f, 1.0f, 0.0f);
            glLineWidth(3.0f);
            glBegin(GL_LINES);
                glVertex3f(beamStartX, beamStartY, beamStartZ);
                glVertex3f(beamEndX,   beamEndY,   beamEndZ);
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
        drawText(w / 2.0f - 80.0f, h / 2.0f + 20.0f, "LEVEL 1 COMPLETE!");
        drawText(w / 2.0f - 110.0f, h / 2.0f - 20.0f, "Press ENTER to Start Level 2");
    }
    else if (currentState == LEVEL_2_COMPLETE) {
        glClearColor(0.8f, 0.6f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        drawText(w / 2.0f - 100.0f, h / 2.0f + 20.0f, "YOU BEAT LEVEL 2!");
        drawText(w / 2.0f - 100.0f, h / 2.0f - 20.0f, "Press ESC to Exit");
    }
    else if (currentState == GAME_OVER) {
        glClearColor(0.8f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        char finalScoreText[50];
        snprintf(finalScoreText, sizeof(finalScoreText), "FINAL SCORE: %d", score);
        drawText(w / 2.0f - 50.0f, h / 2.0f + 20.0f, "TIME'S UP!");
        drawText(w / 2.0f - 70.0f, h / 2.0f - 20.0f, finalScoreText);
    }
}
