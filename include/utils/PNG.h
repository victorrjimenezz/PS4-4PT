//
// Created by Víctor Jiménez Rugama on 1/7/22.
//

#ifndef CYDI4_PNG_H
#define CYDI4_PNG_H
#include <string>
class Scene2D;
class PNG {

    int width;
    int height;
    int channels;
    unsigned char *img;
    std::string path;
public:
    PNG(PNG * png);
    PNG(const char *imagePath);
    PNG(const uint8_t *imageData, uint32_t dataLen);
    PNG(const char *imagePath, int width, int height);
    PNG(const uint8_t *imageData, uint32_t dataLen, int width, int height);
    int getWidth() const;
    int getHeight() const;
    std::string getPath();
    ~PNG();
    int getChannels() const;
    void Draw(Scene2D *scene, int startX, int startY);
    void Draw(uint32_t *frameBuffer, int startX, int startY, int screenWidth, int screenHeight);
};

#endif //CYDI4_PNG_H
