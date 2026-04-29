#pragma once // Prevents this file from being included multiple times

class Player {
public:
    float x, y, z;
    float rotation;
    float speed;
    float turnSpeed;

    Player(); // Constructor
    void update(bool keys[]);
    void draw();
};
