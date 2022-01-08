//
// Created by Víctor Jiménez Rugama on 1/7/22.
//

#ifndef CYDI4_PNG_H
#define CYDI4_PNG_H
#include "../../_common/graphics.h"
#include <string>
class PNG {

    int width;
    int height;
    int channels;
    unsigned char *img;
    std::string path;
public:
    PNG(const char *imagePath);
    PNG(const char *imagePath, int width, int height);
    int getWidth() const;
    int getHeight() const;
    std::string getPath();
    ~PNG();
    int getChannels() const;
    void Draw(Scene2D *scene, int startX, int startY);
};

#endif //CYDI4_PNG_H
