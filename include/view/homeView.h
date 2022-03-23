//
// Created by Víctor Jiménez Rugama on 12/24/21.
//
#ifndef CYDI4_HOMEVIEW_H
#define CYDI4_HOMEVIEW_H
#ifndef GRAPHICS_USES_FONT
#define GRAPHICS_USES_FONT 1
#endif

#include "subView.h"
#include "../utils/AppGraphics.h"

#include <vector>
#include <string>

class PNG;
class keyboardInput;
class homeView : public subView {
private:
    PNG * logo;
    std::string welcomeText;
    std::string welcomeSubText;
    std::string pkgLink;
    int logoX;
    int logoY;
    int logoWidth;
    int logoHeight;
    int keyboardX;
    Scene2D * mainScene;

    Color bgColor{};
    Color textColor{};
    FT_Face fontLarge{};
    FT_Face fontMedium{};
    FT_Face fontSmall{};

    keyboardInput * keyboardInput;

    const int frameWidth = 0;
    const int frameHeight = 0;
    void hasEntered();
    void loadWelcomeText();
public:
    homeView(Scene2D* mainScene, FT_Face fontLarge, FT_Face fontMedium, FT_Face fontSmall, int frameWidth, int frameHeight);
    void updateView();
    void pressX();
    void pressCircle();
    void pressTriangle();
    void pressSquare();
    void arrowUp();
    void arrowDown();
    void arrowRight();
    void arrowLeft();
    void deleteChild(){};
    subView * getParent();
    subView * getChild();
    bool isActive();
    void langChanged();
    ~homeView();
};
#endif //CYDI4_HOMEVIEW_H
