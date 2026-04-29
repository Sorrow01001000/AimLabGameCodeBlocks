#include <GL/freeglut.h>
#include <cstdlib>
#include <ctime>
#include "GameManager.h"

GameManager game;
bool keys[256];

// --- NEW: Cheat Code Tracker ---
char recentKeys[5] = "    "; // Stores 4 blank spaces to start

void reshape(int w, int h) {
    if (h == 0) h = 1;
    float ratio = (float)w / h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, ratio, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void display() {
    game.draw();
    glutSwapBuffers();
}

void update(int value) {
    game.update(keys);
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void keyboardDown(unsigned char key, int x, int y) {
    keys[key] = true;

    // --- NEW: Track the keys for the cheat! ---
    recentKeys[0] = recentKeys[1];
    recentKeys[1] = recentKeys[2];
    recentKeys[2] = recentKeys[3];
    recentKeys[3] = key;

    if (recentKeys[0] == 'b' && recentKeys[1] == 'o' && recentKeys[2] == 'o' && recentKeys[3] == 'm') {
        game.cheatActivated = true; // Tell the game manager to start the party
    }

    if (key == 27) {
        exit(0);
    }

    if (game.currentState == MENU && key == 13) {
        game.initLevel1();
        game.currentState = LEVEL_1;
    }
    else if (game.currentState == LEVEL_1_COMPLETE && key == 13) {
        game.score = 0;
        game.timeLeft = 30.0f;
        game.initLevel2();
        game.currentState = LEVEL_2;
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    keys[key] = false;
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (game.currentState == LEVEL_1 || game.currentState == LEVEL_2) {
            game.shoot();
        }
    }
}

int main(int argc, char** argv) {
    srand(static_cast<unsigned int>(time(0)));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Academic Aim Trainer");

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutMouseFunc(mouse);

    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}
