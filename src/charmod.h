#ifndef CHARMOD_H
#define CHARMOD_H

class Charmod {
public:
    // Position and Physics
    float x, y, z;
    float rotation;
    float velocityY;
    float gravity;
    float jumpForce;
    bool isGrounded;

    // Movement speeds
    float speed;
    float turnSpeed;

    // Animation variables
    float walkTimer;
    bool isWalking;

    Charmod();

    // Core functions
    void update(bool keys[]);
    void draw();

    // Actions
    void jump();
    void interact(); // Placeholder for environment interaction
};

#endif
