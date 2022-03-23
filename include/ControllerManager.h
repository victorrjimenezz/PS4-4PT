//
// Created by Víctor Jiménez Rugama on 12/27/21.
//

#ifndef CYDI4_CONTROLLERMANAGER_H
#define CYDI4_CONTROLLERMANAGER_H

#include "view/tabView.h"
#include "base.h"
#include "main.h"
#include <mutex>

class drwav_int16;
class Controller;
class subView;
class tabView;

class ControllerManager {
private:
    std::mutex mutex;

    int exit;

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

    //Audio Files
    drwav_int16  * tabChangeWav;
    size_t tabChangeWavCount;

    drwav_int16  * pressXWav;
    size_t pressXWavCount;

    drwav_int16  * escapeWav;
    size_t escapeWavCount;

    drwav_int16  * arrowWav;
    size_t arrowWavCount;


    static bool controllerActive;

    Controller *controller = nullptr;
    subView * currentSubView = nullptr;
    tabView * tabView = nullptr;
    void updateRearUpperButtons();
    void updateArrowVertical();
    void updateArrowHorizontal();
    void updateXCircle();
    void updateTriangle();
    void updateSquare();
    void updateController();
public:
    ControllerManager(class tabView * tabView);
    void initController();
    void stopController();
    ~ControllerManager();

    int getExit();

    subView *getCurrentSubView();
};
#endif //CYDI4_CONTROLLERMANAGER_H
