//
// Created by Víctor Jiménez Rugama on 12/27/21.
//
#include "../include/ControllerManager.h"
#include "../include/utils/logger.h"
#include "../include/view/mainView.h"

bool ControllerManager::controllerActive;
ControllerManager::ControllerManager(class tabView *tabView, subView* subViews[VIEWS]) : subViews() {
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

    for(int i =0; i<VIEWS; i ++)
        this->subViews[i] = subViews[i];

    switchCurrentView(tabView->getCurrentView());
}
void ControllerManager::switchCurrentView(tabSelected tabSelected){
    switch (tabSelected) {
        case HOME:
            currentSubView = subViews[0];
            break;
        case SOURCES:
            currentSubView = subViews[1];
            break;
        case INSTALLED:
            currentSubView = subViews[2];
            break;
        case SEARCH:
            currentSubView = subViews[3];
            break;
    }
}
subView * ControllerManager::getCurrentView(){
    return currentSubView;
}

void ControllerManager::initController() {
    mutex.lock();
    controllerActive = true;
    while(controllerActive)
        updateController();

    mutex.unlock();
}

void ControllerManager::updateRearUpperButtons() {
    bool L1Pressed = controller->L1Pressed();
    bool R1Pressed = controller->R1Pressed();
    if(L1Pressed && !R1Pressed && !L1Waspressed){
        switchCurrentView(tabView->tabLeft());
    } else if(!L1Pressed && R1Pressed && !R1Waspressed){
        switchCurrentView(tabView->tabRight());
    }
    L1Waspressed = L1Pressed;
    R1Waspressed = R1Pressed;

}

void ControllerManager::updateArrowVertical() {
    bool ArrowUpPressed = controller->DpadUpPressed();
    bool ArrowDownPressed = controller->DpadDownPressed();
    if(ArrowUpPressed && !ArrowDownPressed && !ArrowUpWasPressed){
        currentSubView->arrowUp();
    } else if(!ArrowUpPressed && ArrowDownPressed && !ArrowDownWasPressed){
        currentSubView->arrowDown();
    }
    ArrowUpWasPressed = ArrowUpPressed;
    ArrowDownWasPressed = ArrowDownPressed;
}

void ControllerManager::updateArrowHorizontal() {
    bool ArrowRightPressed = controller->DpadRightPressed();
    bool ArrowLeftPressed = controller->DpadLeftPressed();
    if(ArrowRightPressed && !ArrowLeftPressed && !ArrowRightWasPressed){
        currentSubView->arrowRight();
    } else if(!ArrowRightPressed && ArrowLeftPressed && !ArrowLeftWasPressed){
        currentSubView->arrowLeft();
    }
    ArrowRightWasPressed = ArrowRightPressed;
    ArrowLeftWasPressed = ArrowLeftPressed;

}

void ControllerManager::updateXCircle() {
    bool XPressed = controller->XPressed();
    bool CirclePressed = controller->CirclePressed();
    if(XPressed && !CirclePressed && !XWasPressed){
        currentSubView->pressX();
    } else if(!XPressed && CirclePressed && !CircleWasPressed){
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
}

void ControllerManager::stopController() {
    controllerActive = false;
}

ControllerManager::~ControllerManager() {
    stopController();
    delete controller;
}