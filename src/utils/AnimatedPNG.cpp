//
// Created by Víctor Jiménez Rugama on 1/5/22.
//
#include "../../include/utils/AnimatedPNG.h"
#include "../../include/utils/PNG.h"
#include "../../include/utils/logger.h"
#include "../../include/file/fileManager.h"

AnimatedPNG::AnimatedPNG(const char *PNGFolder, Scene2D * mainScene, int width, int height) : frames(){
    numFrames = 0;
    currFrame = 0;
    this->width = width;
    this->height = height;
    this->mainScene = mainScene;
    for(const std::string& frame : lsDir(PNGFolder))
        addFrame((PNGFolder + frame).c_str());
}

int AnimatedPNG::addFrame(const char *PNGPath) {
    if(!fileExists(PNGPath))
        return -1;
    if(width <= 0 || height <= 0)
        frames.emplace_back(new PNG(PNGPath));
    else
        frames.emplace_back(new PNG(PNGPath, width, height));
    return ++numFrames;
}
void AnimatedPNG::Play(int startX, int startY) {

    PNG * currFramePNG = frames.at(currFrame);
    if(currFramePNG == nullptr){
        LOG << "NULL Frame at AnimatedPNG";
        return;
    }
    currFramePNG->Draw(mainScene,startX,startY);
    currFrame++;
    currFrame %= numFrames;
}

void AnimatedPNG::Draw(int startX, int startY) {
    currFrame = 0;
    if(frames.at(currFrame) == nullptr)
        return;
    frames.at(currFrame)->Draw(mainScene,startX,startY);
}

AnimatedPNG::~AnimatedPNG() {
    for(PNG * frame : frames)
        delete frame;
}