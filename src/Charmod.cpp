#include "Charmod.h"
#include <GL/freeglut.h>
#include <cmath>

Charmod::Charmod() {
    x = 0.0f;
    y = 2.0f; // Start slightly in the air to test gravity
    z = 0.0f;
    rotation = 0.0f;
    
    // Physics variables
    velocityY = 0.0f;
    gravity = -0.015f;
    jumpForce = 0.3f;
    isGrounded = false;

    // Movement variables
    speed = 0.2f;
    turnSpeed = 3.0f;

    // Animation
    walkTimer = 0.0f;
    isWalking = false;
}

void Charmod::update(bool keys[]) {
    // 1. Handle Turning
    if (keys['a'] || keys['A']) rotation += turnSpeed;
    if (keys['d'] || keys['D']) rotation -= turnSpeed;

    float rad = rotation * (3.14159f / 180.0f);
    isWalking = false;

    // 2. Handle Walking
    if (keys['w'] || keys['W']) {
        x += sin(rad) * speed;
        z += cos(rad) * speed;
        isWalking = true;
    }
    if (keys['s'] || keys['S']) {
        x -= sin(rad) * speed;
        z -= cos(rad) * speed;
        isWalking = true;
    }

    // Update Animation Timer
    if (isWalking) {
        walkTimer += 0.2f; // Speed of the limb swing
    } else {
        // Smoothly return to idle standing position
        walkTimer = 0.0f; 
    }

    // 3. Handle Jumping Input (Assuming Spacebar is mapped to key index 32)
    if (keys[32] && isGrounded) {
        jump();
    }

    // 4. Apply Gravity & Physics
    if (!isGrounded) {
        velocityY += gravity;
    }
    
    y += velocityY;

    // Basic Floor Collision (Assuming ground is at y = 1.0 for the character's feet)
    if (y <= 1.0f) {
        y = 1.0f;
        velocityY = 0.0f;
        isGrounded = true;
    } else {
        isGrounded = false;
    }
}

void Charmod::jump() {
    velocityY = jumpForce;
    isGrounded = false;
}

void Charmod::interact() {
    // Here you would check distance to a door or button
    // similar to how GameManager::shoot() checks target distances.
}

void Charmod::draw() {
    glPushMatrix();
    
    // Move the entire character to their world position
    glTranslatef(x, y, z);
    glRotatef(rotation, 0.0f, 1.0f, 0.0f);

    // Calculate arm and leg swing angles
    float swingAngle = isWalking ? sin(walkTimer) * 30.0f : 0.0f;

    // --- DRAW TORSO ---
    glPushMatrix();
        glColor3f(0.0f, 0.0f, 0.8f); // Blue shirt
        glTranslatef(0.0f, 0.5f, 0.0f);
        glScalef(1.0f, 1.5f, 0.5f);
        glutSolidCube(1.0f);
    glPopMatrix();

    // --- DRAW HEAD ---
    glPushMatrix();
        glColor3f(0.9f, 0.7f, 0.6f); // Skin tone
        glTranslatef(0.0f, 1.5f, 0.0f); // Sit on top of torso
        glutSolidCube(0.6f);
        
        // Draw eyes to show direction
        glColor3f(0.0f, 0.0f, 0.0f);
        glTranslatef(-0.15f, 0.1f, 0.31f);
        glutSolidCube(0.1f);
        glTranslatef(0.30f, 0.0f, 0.0f);
        glutSolidCube(0.1f);
    glPopMatrix();

    // --- DRAW LEFT ARM ---
    glPushMatrix();
        glColor3f(0.0f, 0.0f, 0.8f);
        // Move to shoulder joint
        glTranslatef(-0.7f, 1.0f, 0.0f); 
        // Swing from the shoulder
        glRotatef(swingAngle, 1.0f, 0.0f, 0.0f); 
        // Move down so it hangs from the joint, not the center
        glTranslatef(0.0f, -0.6f, 0.0f); 
        glScalef(0.3f, 1.2f, 0.3f);
        glutSolidCube(1.0f);
    glPopMatrix();

    // --- DRAW RIGHT ARM ---
    glPushMatrix();
        glColor3f(0.0f, 0.0f, 0.8f);
        glTranslatef(0.7f, 1.0f, 0.0f); 
        glRotatef(-swingAngle, 1.0f, 0.0f, 0.0f); // Opposite swing
        glTranslatef(0.0f, -0.6f, 0.0f);
        glScalef(0.3f, 1.2f, 0.3f);
        glutSolidCube(1.0f);
    glPopMatrix();

    // --- DRAW LEFT LEG ---
    glPushMatrix();
        glColor3f(0.2f, 0.2f, 0.2f); // Dark pants
        glTranslatef(-0.3f, -0.2f, 0.0f); // Hip joint
        glRotatef(-swingAngle, 1.0f, 0.0f, 0.0f); // Opposite of left arm
        glTranslatef(0.0f, -0.6f, 0.0f);
        glScalef(0.4f, 1.2f, 0.4f);
        glutSolidCube(1.0f);
    glPopMatrix();

    // --- DRAW RIGHT LEG ---
    glPushMatrix();
        glColor3f(0.2f, 0.2f, 0.2f);
        glTranslatef(0.3f, -0.2f, 0.0f); // Hip joint
        glRotatef(swingAngle, 1.0f, 0.0f, 0.0f); // Opposite of right arm
        glTranslatef(0.0f, -0.6f, 0.0f);
        glScalef(0.4f, 1.2f, 0.4f);
        glutSolidCube(1.0f);
    glPopMatrix();

    glPopMatrix(); // End character transformations
}
