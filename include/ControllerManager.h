//
// Created by Víctor Jiménez Rugama on 12/27/21.
//

#ifndef CYDI4_CONTROLLERMANAGER_H
#define CYDI4_CONTROLLERMANAGER_H

#include "utils/controller.h"
#include "view/subView.h"
#include "view/tabView.h"
#include "base.h"
#include <mutex>

class ControllerManager {
private:
    std::mutex mutex;

    bool L1Waspressed{};
    bool R1Waspressed{};
    bool ArrowUpWasPressed{};
    bool ArrowDownWasPressed{};
    bool ArrowRightWasPressed{};
    bool ArrowLeftWasPressed{};
    bool XWasPressed{};
    bool CircleWasPressed{};
    bool triangleWasPressed{};
    bool squareWasPressed{};

    static bool controllerActive;

    Controller *controller = nullptr;
    subView * currentSubView = nullptr;
    tabView * tabView = nullptr;
    subView* subViews[VIEWS];
    void switchCurrentView(tabSelected tabSelected);
    void updateRearUpperButtons();
    void updateArrowVertical();
    void updateArrowHorizontal();
    void updateXCircle();
    void updateTriangle();
    void updateSquare();
    void updateController();
public:
    ControllerManager(class tabView * tabView, subView* subViews[VIEWS]);
    void initController();
    void stopController();
    ~ControllerManager();
    subView *getCurrentView();
};
#endif //CYDI4_CONTROLLERMANAGER_H
