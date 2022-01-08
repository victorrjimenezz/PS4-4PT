//
// Created by Víctor Jiménez Rugama on 12/24/21.
//
#ifndef CYDI4_MAINVIEW_H
#define CYDI4_MAINVIEW_H

#ifndef GRAPHICS_USES_FONT
#define GRAPHICS_USES_FONT 1
#endif

//Common Headers
#include "../../_common/graphics.h"
#include "../utils/PNG.h"
#include "../../_common/notifi.h"
#include "../ControllerManager.h"
#include "subView.h"
#include "tabView.h"
#include "../base.h"

#include <thread>

class mainView {
private:
    Scene2D * mainScene;
    tabView * tabView;
    subView * currentSubView = nullptr;
    ControllerManager * controllerManager;
    const static int views = VIEWS;
    subView* subViews[views];
    Color bgColor{};



    int frameID;

    // Font faces
    FT_Face fontLarge{};
    FT_Face fontMediumLarge{};
    FT_Face fontMedium{};
    FT_Face fontSmall{};
public:
    static bool updatingView;
    const static int frameWidth = FRAME_WIDTH;
    const static int frameHeight = FRAME_HEIGHT;
    const static int frameDepth = FRAME_DEPTH;

    mainView(bool isFirstRun);
    int updateView();
    ~mainView();
};
#endif //CYDI4_MAINVIEW_H
