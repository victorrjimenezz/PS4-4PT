//
// Created by Víctor Jiménez Rugama on 2/6/22.
//
#include "../../include/utils/GIF.h"
#include "../../include/file/fileManager.h"
#include "../../include/utils/AppGraphics.h"
#include "../../include/utils/logger.h"
#include <stb/stb_image.h>
#include <stb/stb_image_resize.h>

void GIF::loadGIF(uint8_t *data, uint32_t len, int desiredWidth, int desiredHeight) {
    int *delays = NULL;
    uint8_t * imageData;

    imageData = (uint8_t *) stbi_load_gif_from_memory(data, len, &delays, &width, &height, &frameCount, &channels, 0);
    frameSize = width*height*channels;

    uint8_t * newImg;
    auto * currentFrameBytes = (unsigned char *) malloc(frameSize);
    for(int i = 0; i<frameCount;i++){
        newImg = (unsigned char *) malloc(desiredWidth*desiredHeight*channels);
        memcpy(currentFrameBytes,imageData,frameSize);
        stbir_resize_uint8(currentFrameBytes, this->width, this->height, 0, newImg, desiredWidth, desiredHeight, 0, channels);
        frames.emplace_back(newImg);
        imageData+=frameSize;
    }
    stbi_image_free((uint32_t *)currentFrameBytes);

    imageData -= frameSize*frameCount;
    if(imageData!= nullptr)
        stbi_image_free((uint32_t *)imageData);

    this->width = desiredWidth;
    this->height = desiredHeight;
    this->frameSize = desiredWidth*desiredHeight*channels;
}

GIF::~GIF() {
    std::unique_lock<std::mutex> lock(frameMutex);
    for(auto frame : frames)
        if (frame != nullptr)
            free(frame);
}

GIF::GIF(const char * path, int width, int height) : PNG(path,width,height), frameMutex() {
    FILE * imageFile = fopen(path,"rb");
    uint32_t imageSize;
    uint8_t * imageDataBytes;
    currentFrame = 0;

    if(imageFile== nullptr)
        goto fail;

    fseek(imageFile, 0L, SEEK_END);
    imageSize = ftell(imageFile);
    rewind(imageFile);

    imageDataBytes = (uint8_t *) malloc(sizeof(uint8_t) * imageSize);
    fread(imageDataBytes,1,imageSize,imageFile);

    loadGIF(imageDataBytes,imageSize,width,height);

    free(imageDataBytes);

    return;
    fail:
    this->currentFrame=0;
    this->width = 0;
    this->height = 0;
    this->frameCount = 0;
    this->channels = 0;
    this->frameSize = 0;
}

void GIF::Draw(Scene2D *scene, int startX, int startY) {

    std::unique_lock<std::mutex> lock(frameMutex);
    // Don't draw non-existant images
    if(frames.size() <= currentFrame) {
        currentFrame = 0;
        return;
    }

    uint8_t * currentFrameBytes = frames.at(currentFrame);
    if(currentFrameBytes == nullptr)
        return;

    // Iterate the bitmap and draw the pixels
    for (int yPos = 0; yPos < this->height; yPos++)
    {
        for (int xPos = 0; xPos < this->width; xPos++)
        {
            // Do some bounds checking to make sure the pixel is actually inside the frame buffer
            if (xPos < 0 || yPos < 0 || xPos >= this->width || yPos >= this->height)
                continue;
            int x = startX + xPos;
            int y = startY + yPos;

            auto* pixelOffset = currentFrameBytes + (xPos + yPos * width) * channels;
            if(pixelOffset == nullptr)
                continue;

            uint8_t r = pixelOffset[0];
            uint8_t g = pixelOffset[1];
            uint8_t b = pixelOffset[2];
            uint8_t a = channels >= 4 ? pixelOffset[3] : 0xff;

            Color color = { r, g, b, a};

            scene->DrawPixel(x, y, color);
        }
    }
    currentFrame++;
    currentFrame%=frameCount;
}
