#include "Player.h"
#include <GL/freeglut.h>
#include <cmath>

// -------------------------------------------------------
//  CONSTRUCTOR  — same values, untouched
// -------------------------------------------------------
Player::Player() {
    x = 0.0f;
    y = 0.5f;
    z = 0.0f;
    rotation  = 0.0f;
    speed     = 0.2f;
    turnSpeed = 3.0f;
    walkTimer = 0.0f;
    isWalking = false;
    velocityY = 0.0f;
    gravity = -0.015f;
    jumpForce = 0.3f;
    isGrounded = true;
    y = 0.0f; // start on ground
}

// -------------------------------------------------------
//  UPDATE  — original movement logic, untouched
// -------------------------------------------------------
void Player::update(bool keys[]) {
    if (keys['a'] || keys['A']) rotation += turnSpeed;
    if (keys['d'] || keys['D']) rotation -= turnSpeed;

    float rad = rotation * (3.14159f / 180.0f);
    isWalking = false;

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

    if (isWalking) walkTimer += 0.18f;
    else           walkTimer  = 0.0f;

    // JUMP
    if (keys[32] && isGrounded) {
        velocityY = jumpForce;
        isGrounded = false;
    }

    velocityY += gravity;
    y += velocityY;


    if (y <= 0.0f) {
        y = 0.0f;
        velocityY = 0.0f;
        isGrounded = true;
    }


    // -------------------------------------------------------
    //  INVISIBLE WALLS — clamp player inside the world boundary.
    //  The ground plane spans -50 to +50; we stop at ±47 so
    //  the player never touches or clips through the sky walls.
    // -------------------------------------------------------
    const float WALL = 47.0f;
    if (x >  WALL) x =  WALL;
    if (x < -WALL) x = -WALL;
    if (z >  WALL) z =  WALL;
    if (z < -WALL) z = -WALL;
}

// -------------------------------------------------------
//  HELPER — draw a box centred at current matrix origin
// -------------------------------------------------------
static void box(float sx, float sy, float sz) {
    glPushMatrix();
    glScalef(sx, sy, sz);
    glutSolidCube(1.0f);
    glPopMatrix();
}

// -------------------------------------------------------
//  DRAW — pure-cube Roblox-style character + AK-47
//
//  Layout (Y=0 is player origin on ground):
//   feet/shoes : 0.00-0.20
//   legs       : 0.20-1.00
//   torso      : 1.00-1.80
//   head       : 1.80-2.40
//  Arms are posed OUT horizontally (weapon-hold stance).
// -------------------------------------------------------
void Player::draw() {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotation, 0.0f, 1.0f, 0.0f);

    // Walk swing — legs only, arms locked in gun-hold pose
    float legSwing = isWalking ? sinf(walkTimer) * 28.0f : 0.0f;
    float bob      = isWalking ? sinf(walkTimer * 2.0f) * 0.02f : 0.0f;

    // === ROBLOX COLOUR PALETTE ===
    auto skin  = []{ glColor3f(0.98f, 0.85f, 0.55f); }; // classic Roblox yellow
    auto shirt = []{ glColor3f(0.15f, 0.40f, 0.80f); }; // blue shirt
    auto pants = []{ glColor3f(0.15f, 0.15f, 0.40f); }; // dark blue pants
    auto shoes = []{ glColor3f(0.10f, 0.08f, 0.06f); }; // near-black shoes
    auto hair  = []{ glColor3f(0.20f, 0.12f, 0.04f); }; // dark brown hair
    auto hips  = []{ glColor3f(0.18f, 0.18f, 0.45f); }; // slightly lighter pants for hips

    // -------------------------------------------------------
    // Scale: character is ~1.6 units tall (shorter / squatter)
    // Y layout:
    //   0.00-0.15  shoes
    //   0.15-0.55  legs
    //   0.55-0.70  HIPS block
    //   0.70-1.15  torso
    //   1.15-1.60  head
    // -------------------------------------------------------

    // ================================================
    // SHOES
    // ================================================
    glPushMatrix();
        glTranslatef(-0.18f, 0.08f + bob, 0.03f);
        glRotatef(-legSwing * 0.3f, 1,0,0);
        shoes(); box(0.26f, 0.16f, 0.34f);
    glPopMatrix();
    glPushMatrix();
        glTranslatef( 0.18f, 0.08f + bob, 0.03f);
        glRotatef( legSwing * 0.3f, 1,0,0);
        shoes(); box(0.26f, 0.16f, 0.34f);
    glPopMatrix();

    // ================================================
    // LEFT LEG
    // ================================================
    glPushMatrix();
        glTranslatef(-0.18f, 0.35f + bob, 0.0f);
        glRotatef(-legSwing, 1,0,0);
        pants();
        glPushMatrix(); glTranslatef(0,-0.20f,0); box(0.28f, 0.40f, 0.28f); glPopMatrix();
    glPopMatrix();

    // ================================================
    // RIGHT LEG
    // ================================================
    glPushMatrix();
        glTranslatef( 0.18f, 0.35f + bob, 0.0f);
        glRotatef( legSwing, 1,0,0);
        pants();
        glPushMatrix(); glTranslatef(0,-0.20f,0); box(0.28f, 0.40f, 0.28f); glPopMatrix();
    glPopMatrix();

    // ================================================
    // HIPS — wide block connecting legs to torso
    //        This is the key Roblox silhouette feature
    // ================================================
    glPushMatrix();
        glTranslatef(0.0f, 0.60f + bob, 0.0f);
        hips();
        // Wide and shallow — spans both legs + a bit extra
        box(0.64f, 0.18f, 0.32f);
    glPopMatrix();

    // ================================================
    // TORSO
    // ================================================
    glPushMatrix();
        glTranslatef(0.0f, 0.93f + bob, 0.0f);
        shirt();
        // Slightly narrower than hips so hips are visible
        box(0.58f, 0.48f, 0.30f);
    glPopMatrix();

    // ================================================
    // LEFT ARM — locked ~65° forward for gun hold
    // ================================================
    glPushMatrix();
        glTranslatef(-0.42f, 1.05f + bob, 0.0f);
        glRotatef(-65.0f, 1,0,0);
        shirt();
        glPushMatrix(); glTranslatef(0,-0.18f,0); box(0.22f, 0.36f, 0.22f); glPopMatrix();
        skin();
        glPushMatrix(); glTranslatef(0,-0.38f,0); box(0.20f, 0.16f, 0.20f); glPopMatrix();
    glPopMatrix();

    // ================================================
    // RIGHT ARM — locked ~55° forward (trigger hand)
    // ================================================
    glPushMatrix();
        glTranslatef( 0.42f, 1.05f + bob, 0.0f);
        glRotatef(-55.0f, 1,0,0);
        shirt();
        glPushMatrix(); glTranslatef(0,-0.18f,0); box(0.22f, 0.36f, 0.22f); glPopMatrix();
        skin();
        glPushMatrix(); glTranslatef(0,-0.38f,0); box(0.20f, 0.16f, 0.20f); glPopMatrix();
    glPopMatrix();

    // ================================================
    // HEAD — big Roblox cube head, shorter neck
    // ================================================
    glPushMatrix();
        glTranslatef(0.0f, 1.38f + bob, 0.0f);

        // Main head (square, Roblox-proportioned)
        skin(); box(0.54f, 0.50f, 0.54f);

        // Hair — thick top slab + side curtains
        hair();
        glPushMatrix(); glTranslatef(0,  0.28f, -0.01f); box(0.56f, 0.12f, 0.56f); glPopMatrix();
        glPushMatrix(); glTranslatef(-0.28f, 0.14f, -0.01f); box(0.02f, 0.22f, 0.56f); glPopMatrix();
        glPushMatrix(); glTranslatef( 0.28f, 0.14f, -0.01f); box(0.02f, 0.22f, 0.56f); glPopMatrix();

        // Eyes (black flat squares)
        glColor3f(0.05f, 0.05f, 0.05f);
        glPushMatrix(); glTranslatef(-0.12f,  0.05f, 0.28f); box(0.11f, 0.09f, 0.02f); glPopMatrix();
        glPushMatrix(); glTranslatef( 0.12f,  0.05f, 0.28f); box(0.11f, 0.09f, 0.02f); glPopMatrix();

        // Smile
        glColor3f(0.40f, 0.15f, 0.10f);
        glPushMatrix(); glTranslatef(0.0f, -0.10f, 0.28f); box(0.18f, 0.035f, 0.02f); glPopMatrix();

    glPopMatrix(); // end head

    // ================================================
    // AK-47 — chest height, pointing forward (+Z)
    // ================================================
    glPushMatrix();
        glTranslatef(0.08f, 0.70f + bob, 0.35f);
        glRotatef(-8.0f, 1,0,0);

        auto gunMetal  = []{ glColor3f(0.22f, 0.22f, 0.22f); };
        auto gunWood   = []{ glColor3f(0.45f, 0.25f, 0.08f); };
        auto gunBarrel = []{ glColor3f(0.30f, 0.30f, 0.30f); };

        // RECEIVER
        gunMetal();
        glPushMatrix(); glTranslatef(0, 0, 0.08f); box(0.09f, 0.10f, 0.46f); glPopMatrix();

        // BARREL
        gunBarrel();
        glPushMatrix(); glTranslatef(0, 0.01f, 0.46f); box(0.04f, 0.04f, 0.42f); glPopMatrix();

        // MUZZLE BRAKE
        gunMetal();
        glPushMatrix(); glTranslatef(0, 0.01f, 0.68f); box(0.07f, 0.07f, 0.05f); glPopMatrix();

        // GAS TUBE
        glPushMatrix(); glTranslatef(0, 0.08f, 0.38f); box(0.03f, 0.03f, 0.28f); glPopMatrix();

        // FRONT HANDGUARD (wood)
        gunWood();
        glPushMatrix(); glTranslatef(0, 0.0f, 0.37f); box(0.08f, 0.08f, 0.18f); glPopMatrix();

        // BANANA MAGAZINE
        gunMetal();
        glPushMatrix(); glTranslatef(0, -0.11f, 0.06f); box(0.07f, 0.12f, 0.09f); glPopMatrix();
        glPushMatrix();
            glTranslatef(0, -0.20f, 0.10f);
            glRotatef(18.0f, 1,0,0);
            box(0.07f, 0.15f, 0.08f);
        glPopMatrix();

        // PISTOL GRIP
        gunWood();
        glPushMatrix();
            glTranslatef(0.0f, -0.10f, -0.03f);
            glRotatef(-25.0f, 1,0,0);
            box(0.08f, 0.17f, 0.08f);
        glPopMatrix();

        // STOCK
        gunWood();
        glPushMatrix(); glTranslatef(0, 0.02f, -0.18f); box(0.07f, 0.09f, 0.25f); glPopMatrix();
        glPushMatrix(); glTranslatef(0, 0.07f, -0.25f); box(0.07f, 0.03f, 0.14f); glPopMatrix();

        // SIGHTS
        gunMetal();
        glPushMatrix(); glTranslatef(0, 0.07f, -0.05f); box(0.05f, 0.04f, 0.03f); glPopMatrix();
        glPushMatrix(); glTranslatef(0, 0.07f, 0.46f);  box(0.02f, 0.05f, 0.02f); glPopMatrix();

    glPopMatrix(); // end AK-47

    glPopMatrix(); // end character
}
