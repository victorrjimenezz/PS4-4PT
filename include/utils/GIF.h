//
// Created by Víctor Jiménez Rugama on 2/6/22.
//

#ifndef INC_4PT_GIF_H
#define INC_4PT_GIF_H
#include "PNG.h"
#include <vector>
#include <mutex>

class GIF : public PNG {
private:
    std::mutex frameMutex;
    int width, height, frameCount, channels, frameSize, currentFrame;
    std::vector<uint8_t*> frames;
    void loadGIF(uint8_t *data, uint32_t len, int desiredWidth, int desiredHeight);
public:
    explicit GIF(const char * path, int width, int height);
    void Draw(Scene2D *scene, int startX, int startY) override;
    ~GIF() override;
};
#endif //INC_4PT_GIF_H
