#pragma once

class Target {
public:
    float x, y, z;
    float size;
    bool active;

    Target(); // Constructor
    void spawn();
    void draw();
};
