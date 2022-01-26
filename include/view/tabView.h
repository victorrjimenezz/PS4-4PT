//
// Created by Víctor Jiménez Rugama on 12/24/21.
//

#ifndef CYDI4_TABVIEW_H
#define CYDI4_TABVIEW_H

#ifndef GRAPHICS_USES_FONT
#define GRAPHICS_USES_FONT 1
#endif

//Common Headers
#include "../utils/AppGraphics.h"
#include "tabSelected.h"

class PNG;
class tabView {
    struct tabRectange{
        int x;
        int y;
        int w;
        int h;
        Color color;
    };
    struct pos{
        int x;
        int y;
    };
private:
    tabSelected tabSelected;
    Color tabColor;
    Scene2D * mainScene;

    tabRectange tabRectange;

    pos homePos{};
    PNG * homeIcon;
    PNG * homeSelectedIcon;

    pos sourcesPos{};
    PNG * sourcesIcon;
    PNG * sourcesSelectedIcon;

    /*pos downloadPos{};
    PNG * downloadIcon;
    PNG * downloadSelectedIcon;*/

    pos installedPos{};
    PNG * installedIcon;
    PNG * installedSelectedIcon;

    pos searchPos;
    PNG * searchIcon;
    PNG * searchSelectedIcon;

    int frameWidth = 0;
    int frameHeight = 0;
public:
    enum tabSelected getCurrentView();
    enum tabSelected tabRight();
    enum tabSelected tabLeft();
    explicit tabView(Scene2D * scene, int frameWidth, int frameHeight);
    int updateTab();
    ~tabView();
};
#endif //CYDI4_TABVIEW_H
