#include "Target.h"
#include <GL/freeglut.h>
#include <cstdlib>
#include <cmath>

Target::Target() {
    active = false;
    size = 1.5f;
}

void Target::spawn() {
    x = (rand() % 60) - 30.0f;
    y = 1.0f;
    z = (rand() % 60) - 30.0f;

    if (std::abs(x) < 5.0f) x += 10.0f;
    if (std::abs(z) < 5.0f) z += 10.0f;

    active = true;
}

void Target::draw() {
    if (active) {
        glPushMatrix();
        glTranslatef(x, y, z);
        glColor3f(1.0f, 0.0f, 0.0f);
        glutSolidCube(size);
        glPopMatrix();
    }
}
