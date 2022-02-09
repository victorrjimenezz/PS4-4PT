//
// Created by Víctor Jiménez Rugama on 2/8/22.
//

#ifndef INC_4PT_SETTINGSVIEW_H
#define INC_4PT_SETTINGSVIEW_H
#ifndef GRAPHICS_USES_FONT
#define GRAPHICS_USES_FONT 1
#endif

#include "subView.h"
#include "../utils/AppGraphics.h"

#include <vector>
#include <string>

class PNG;
class settingsView : public subView {
private:
    enum rows {LANGROW, NOTIFICATIONROW};
    rows currentRow;
    struct flag {
        PNG * flagIcon;
        std::string lang;
    };

    Color textColor;
    Color checkboxColor;
    Color checkboxEnabledColor;
    std::string langText;

    Scene2D * mainScene;
    int frameWidth, frameHeight;
    int flagTextPosX;
    int textPosY;
    int flagPosY;
    int flagBaseX;
    int flagSpacingX;
    int selected;

    int notificationsXText;
    int notificationsYText;
    int notificationsY;
    int notifications1X;
    int notifications2X;
    int notifications3X;

    FT_Face fontLarge;
    FT_Face fontMedium;
    FT_Face fontSmall;
    std::vector<flag> flags;

    PNG * selectedBorder;

    PNG * checkboxBorder;
    PNG * checkboxCross;
    PNG * checkboxSelectedCross;
public:
    explicit settingsView(Scene2D* mainScene, FT_Face fontLarge, FT_Face fontMedium, FT_Face fontSmall, int frameWidth, int frameHeight);

    void updateView();
    void pressX();
    void pressCircle();
    void pressTriangle();
    void pressSquare();
    void arrowUp();
    void arrowDown();
    void arrowRight();
    void arrowLeft();
    bool isActive();
    void setActive(){};
    void deleteChild(){};
    void langChanged();
    subView * getParent();
    subView * getChild();
    ~settingsView();
};
#endif //INC_4PT_SETTINGSVIEW_H
