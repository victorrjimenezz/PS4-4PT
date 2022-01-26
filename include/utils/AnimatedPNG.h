//
// Created by Víctor Jiménez Rugama on 1/5/22.
//
#ifndef CYDI4_ANIMATEDPNG_H
#define CYDI4_ANIMATEDPNG_H
#include "AppGraphics.h"
#include <vector>
class PNG;
class AnimatedPNG {
private:
    std::vector<PNG*> frames;
    Scene2D * mainScene;
    int currFrame, numFrames, width, height;
    bool playing;
public:
    AnimatedPNG(const char * PNGFolder, Scene2D * mainScene, int width = -1, int height = -1);
    int addFrame(const char * PNGPath);
    void play();
    void stop();
    void Draw(int startX, int startY);
    ~AnimatedPNG();
};
#endif //CYDI4_ANIMATEDPNG_H
