#include "Player.h"
#include <GL/freeglut.h>
#include <cmath>

Player::Player() {
    x = 0.0f;
    y = 0.5f;
    z = 0.0f;
    rotation = 0.0f;
    speed = 0.2f;
    turnSpeed = 3.0f;
}

void Player::update(bool keys[]) {
    if (keys['a'] || keys['A']) rotation += turnSpeed;
    if (keys['d'] || keys['D']) rotation -= turnSpeed;

    float rad = rotation * (3.14159f / 180.0f);

    if (keys['w'] || keys['W']) {
        x += sin(rad) * speed;
        z += cos(rad) * speed;
    }
    if (keys['s'] || keys['S']) {
        x -= sin(rad) * speed;
        z -= cos(rad) * speed;
    }
}

void Player::draw() {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotation, 0.0f, 1.0f, 0.0f);

    glColor3f(0.0f, 0.0f, 1.0f);
    glutSolidCube(1.0f);

    glTranslatef(0.0f, 0.0f, 0.6f);
    glColor3f(1.0f, 1.0f, 0.0f);
    glutSolidCube(0.3f);

    glPopMatrix();
}
