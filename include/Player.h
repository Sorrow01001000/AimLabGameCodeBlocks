#pragma once // Prevents this file from being included multiple times

class Player {
public:
    float x, y, z;
    float rotation;
    float speed;
    float turnSpeed;



    // Walk animation (set by update, read by draw — no logic change)
    float walkTimer;
    bool  isWalking;

    float velocityY;
    float gravity;
    float jumpForce;
    bool isGrounded;


    Player(); // Constructor
    void update(bool keys[]);
    void draw();

private:
    // Helper: draw a solid sphere approximated with glutSolidSphere
    void drawLimb(float sx, float sy, float sz) const;
};
