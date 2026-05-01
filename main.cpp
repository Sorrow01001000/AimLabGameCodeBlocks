#include <GL/freeglut.h>
#include "GameManager.h"
#include <string>

// Our global game object
GameManager game;

// Array to keep track of which keys are being held down (for W, A, S, D movement)
bool keys[256] = { false };

// Variables to track the "boom" cheat code
std::string cheatBuffer = "";

// --------------------------------------------------------
// THIS IS THE FUNCTION WE NEEDED TO UPDATE
// --------------------------------------------------------
void keyboard(unsigned char key, int x, int y) {
    // 1. Mark the key as pressed down (for movement)
    keys[key] = true;

    // 2. SEND THE KEY TO OUR NEW MENU SYSTEM!
    game.handleMenuInput(key);

    // 3. The "boom" cheat code logic
    cheatBuffer += key;
    if (cheatBuffer.length() > 4) {
        cheatBuffer.erase(0, 1); // Keep only the last 4 letters typed
    }
    if (cheatBuffer == "boom") {
        game.cheatActivated = true;
    }
}
// --------------------------------------------------------

void keyboardUp(unsigned char key, int x, int y) {
    // Mark the key as released (so the player stops moving)
    keys[key] = false;
}

void mouse(int button, int state, int x, int y) {
    // If we are actually playing a level and we left-click, shoot!
    if ((game.currentState == LEVEL_1 || game.currentState == LEVEL_2) &&
         button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        game.shoot();
    }
}

void display() {
    // This tells the GameManager to draw whatever screen we are currently on
    game.draw();
    glutSwapBuffers();
}

void update(int value) {
    // This runs 60 times a second to update the game logic
    game.update(keys);

    glutPostRedisplay();
    glutTimerFunc(16, update, 0); // 16 milliseconds = ~60 frames per second
}

void reshape(int w, int h) {
    if (h == 0) h = 1; // Prevent a divide-by-zero error
    float ratio = w * 1.0f / h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);

    // Set the 3D camera perspective
    gluPerspective(45.0f, ratio, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Beam of Madness");

    game.init();
    // Enable 3D depth testing
    glEnable(GL_DEPTH_TEST);

    // Register all our input and draw functions
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutMouseFunc(mouse);

    // Start the game loop
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}
