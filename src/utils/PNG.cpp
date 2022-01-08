//
// Created by Víctor Jiménez Rugama on 1/7/22.
//
#define STBI_ASSERT(x)
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stb/stb_image_resize.h>
#include "../../include/utils/PNG.h"
#include "../../include/utils/logger.h"

PNG::PNG(const char *imagePath)
{
    this->img = (unsigned char *)stbi_load(imagePath, &this->width, &this->height, &this->channels, STBI_rgb_alpha);

    if (this->img == NULL)
    {
        LOG << "Failed to load image '" << imagePath << "': " << stbi_failure_reason();
        return;
    }
}
int PNG::getChannels() const {
    return channels;
}
PNG::PNG(const char *imagePath, int width, int height)
{
    const unsigned char * tempIMG = ( unsigned char *)stbi_load(imagePath, &this->width, &this->height, &this->channels, STBI_default);

    if (tempIMG == NULL)
    {
        LOG << "Failed to load image '" << imagePath << "': " << stbi_failure_reason();
        return;
    }
    this->img = (unsigned char *) malloc(width*height*channels);
    stbir_resize_uint8(tempIMG, this->width, this->height, 0, img, width, height, 0, channels);
    stbi_image_free((uint32_t *)tempIMG);
    if (this->img == NULL)
    {
        LOG << "Failed to load image '" << imagePath << "': " << stbi_failure_reason();
        return;
    }
    this->width = width;
    this->height = height;
}

PNG::~PNG()
{
    if(this->img != NULL)
        stbi_image_free(this->img);
}

void PNG::Draw(Scene2D *scene, int startX, int startY)
{
    // Don't draw non-existant images
    if(this->img == NULL)
        return;

    // Iterate the bitmap and draw the pixels
    for (int yPos = 0; yPos < this->height; yPos++)
    {
        for (int xPos = 0; xPos < this->width; xPos++)
        {
            int x = startX + xPos;
            int y = startY + yPos;

            auto* pixelOffset = this->img + (xPos + yPos * height) * channels;
            uint8_t r = pixelOffset[0];
            uint8_t g = pixelOffset[1];
            uint8_t b = pixelOffset[2];
            uint8_t a = channels >= 4 ? pixelOffset[3] : 0xff;

            Color color = { r, g, b };

            // Do some bounds checking to make sure the pixel is actually inside the frame buffer
            if (xPos < 0 || yPos < 0 || xPos >= this->width || yPos >= this->height)
                continue;

            if(a > 100)
                scene->DrawPixel(x, y, color);
        }
    }
}


