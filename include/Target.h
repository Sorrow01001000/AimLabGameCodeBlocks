#pragma once

class Target {
public:
    float x, y, z;
    float spawnX;   // remembered X origin for left-right slide
    float size;
    bool active;

    Target(); // Constructor
    void spawn();
    void draw();
};
