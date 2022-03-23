//
// Created by Víctor Jiménez Rugama on 12/27/21.
//
#include "../include/ControllerManager.h"
#include "../include/utils/controller.h"
#include "../include/view/mainView.h"
#include "../include/view/subView.h"
#include "../include/utils/AudioManager.h"



bool ControllerManager::controllerActive;

ControllerManager::ControllerManager(class tabView *tabView) {
    this->exit = 0;
    this->tabView = tabView;
    controller = new Controller();
    controller->Init(-1);

    L1Waspressed = false;
    R1Waspressed = false;
    ArrowUpWasPressed = false;
    ArrowDownWasPressed = false;
    ArrowRightWasPressed = false;
    ArrowLeftWasPressed = false;
    XWasPressed = false;
    triangleWasPressed = false;
    CircleWasPressed = false;
    squareWasPressed = false;
    controllerActive = false;

}

void ControllerManager::initController() {
    std::unique_lock<std::mutex> lock(mutex);

    //LOAD tabChangeWav
    std::string audioPath = DATA_PATH;
    audioPath+="assets/audio/tabChange.wav";
    tabChangeWav = (drwav_int16 *)AudioManager::loadAudioFile(audioPath.c_str(), &tabChangeWavCount);

    //LOAD pressXWav
    audioPath = DATA_PATH;
    audioPath+="assets/audio/pressX.wav";
    pressXWav = (drwav_int16 *)AudioManager::loadAudioFile(audioPath.c_str(), &pressXWavCount);

    //LOAD escapeWav
    audioPath = DATA_PATH;
    audioPath+="assets/audio/escape.wav";
    escapeWav = (drwav_int16 *)AudioManager::loadAudioFile(audioPath.c_str(), &escapeWavCount);

    //LOAD arrowWav
    audioPath = DATA_PATH;
    audioPath+="assets/audio/arrow.wav";
    arrowWav = (drwav_int16 *)AudioManager::loadAudioFile(audioPath.c_str(), &arrowWavCount);

    currentSubView = getSubViewAt(tabView->getCurrentView());
    controllerActive = true;

    while(controllerActive)
        updateController();

}

void ControllerManager::updateRearUpperButtons() {
    bool L1Pressed = controller->L1Pressed();
    bool R1Pressed = controller->R1Pressed();
    if(L1Pressed && !R1Pressed && !L1Waspressed){
        getMainAudioManager()->playAudio(tabChangeWav,tabChangeWavCount);
        currentSubView = getSubViewAt(tabView->tabLeft());
    } else if(!L1Pressed && R1Pressed && !R1Waspressed){
        getMainAudioManager()->playAudio(tabChangeWav,tabChangeWavCount);
        currentSubView = getSubViewAt(tabView->tabRight());
    }
    L1Waspressed = L1Pressed;
    R1Waspressed = R1Pressed;

}

void ControllerManager::updateArrowVertical() {
    bool ArrowUpPressed = controller->DpadUpPressed();
    bool ArrowDownPressed = controller->DpadDownPressed();
    if(ArrowUpPressed && !ArrowDownPressed && !ArrowUpWasPressed){
        getMainAudioManager()->playAudio(arrowWav,arrowWavCount);
        currentSubView->arrowUp();
    } else if(!ArrowUpPressed && ArrowDownPressed && !ArrowDownWasPressed){
        getMainAudioManager()->playAudio(arrowWav,arrowWavCount);
        currentSubView->arrowDown();
    }
    ArrowUpWasPressed = ArrowUpPressed;
    ArrowDownWasPressed = ArrowDownPressed;
}

void ControllerManager::updateArrowHorizontal() {
    bool ArrowRightPressed = controller->DpadRightPressed();
    bool ArrowLeftPressed = controller->DpadLeftPressed();
    if(ArrowRightPressed && !ArrowLeftPressed && !ArrowRightWasPressed){
        getMainAudioManager()->playAudio(arrowWav,arrowWavCount);
        currentSubView->arrowRight();
    } else if(!ArrowRightPressed && ArrowLeftPressed && !ArrowLeftWasPressed){
        getMainAudioManager()->playAudio(arrowWav,arrowWavCount);
        currentSubView->arrowLeft();
    }
    ArrowRightWasPressed = ArrowRightPressed;
    ArrowLeftWasPressed = ArrowLeftPressed;

}

void ControllerManager::updateXCircle() {
    bool XPressed = controller->XPressed();
    bool CirclePressed = controller->CirclePressed();
    if(XPressed && !CirclePressed && !XWasPressed){
        getMainAudioManager()->playAudio(pressXWav,pressXWavCount);
        currentSubView->pressX();
    } else if(!XPressed && CirclePressed && !CircleWasPressed){
        getMainAudioManager()->playAudio(escapeWav,escapeWavCount);
        currentSubView->pressCircle();
    }
    XWasPressed = XPressed;
    CircleWasPressed = CirclePressed;

}

void ControllerManager::updateTriangle() {
    bool trianglePressed = controller->TrianglePressed();
    if(trianglePressed && !triangleWasPressed){
        currentSubView->pressTriangle();
    }
    triangleWasPressed = trianglePressed;
}

void ControllerManager::updateSquare() {
    bool squarePressed = controller->SquarePressed();
    if(squarePressed && !squareWasPressed){
        currentSubView->pressSquare();
    }
    squareWasPressed = squarePressed;
}

void ControllerManager::updateController() {

    while(!currentSubView->isActive()) {
        while(mainView::updatingView)
            continue;
        currentSubView = currentSubView->getParent();
        currentSubView->deleteChild();
    }

    while(currentSubView->getChild() != nullptr)
        currentSubView = currentSubView->getChild();


    updateRearUpperButtons();
    updateArrowVertical();
    updateArrowHorizontal();
    updateXCircle();
    updateTriangle();
    updateSquare();
    exit = controller->OptionsPressed() ? -1 : 0;
}

void ControllerManager::stopController() {
    controllerActive = false;
}

ControllerManager::~ControllerManager() {
    controllerActive = false;

    std::unique_lock<std::mutex> lock(mutex);

    if(tabChangeWav!= nullptr)
        free(tabChangeWav);
    if(pressXWav!= nullptr)
        free(pressXWav);
    if(escapeWav!= nullptr)
        free(escapeWav);
    if(arrowWav!= nullptr)
        free(arrowWav);

    stopController();
    delete controller;
}
subView * ControllerManager::getCurrentSubView(){
    return currentSubView;
}
int ControllerManager::getExit() {
    return exit;
}
