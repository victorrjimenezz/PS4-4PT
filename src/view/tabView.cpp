//
// Created by Víctor Jiménez Rugama on 12/24/21.
//
#include "../../include/view/tabView.h"
#include "../../include/base.h"
#include "../../include/utils/PNG.h"

#include <string>

tabView::tabView(Scene2D *scene, int frameWidth, int frameHeight) : tabRectange(), tabColor(), tabSelected(), searchPos() {
    mainScene = scene;
    this->frameWidth = frameWidth;
    this->frameHeight = frameHeight;
    tabColor = {110,110,110};
    tabSelected = HOME;
    int tabViewSize = static_cast<int>(frameHeight*(TABVIEWSIZE));
    tabRectange = {0, frameHeight-tabViewSize,frameWidth,tabViewSize,tabColor};

    homePos = {1*frameWidth/6, tabRectange.y};
    std::string PNGPath = std::string(DATA_PATH);
    homeIcon = new PNG((PNGPath+"assets/images/tabView/home.png").c_str());
    homeSelectedIcon = new PNG((PNGPath+"assets/images/tabView/homeSelected.png").c_str());

    sourcesPos = {2*frameWidth/6, tabRectange.y};
    sourcesIcon = new PNG((PNGPath+"assets/images/tabView/sources.png").c_str());
    sourcesSelectedIcon = new PNG((PNGPath+"assets/images/tabView/sourcesSelected.png").c_str());

    installedPos = {3*frameWidth/6, tabRectange.y};
    installedIcon = new PNG((PNGPath+"assets/images/tabView/installed.png").c_str());
    installedSelectedIcon = new PNG((PNGPath+"assets/images/tabView/installedSelected.png").c_str());

    searchPos = {4*frameWidth/6, tabRectange.y};
    searchIcon = new PNG((PNGPath+"assets/images/tabView/search.png").c_str());
    searchSelectedIcon = new PNG((PNGPath+"assets/images/tabView/searchSelected.png").c_str());

    settingsPos = {5*frameWidth/6, tabRectange.y};
    settingsIcon = new PNG((PNGPath+"assets/images/tabView/settings.png").c_str(),ICON_DEFAULT_WIDTH,ICON_DEFAULT_HEIGHT);
    settingsSelectedIcon = new PNG((PNGPath+"assets/images/tabView/settingsSelected.png").c_str(),ICON_DEFAULT_WIDTH,ICON_DEFAULT_HEIGHT);
}
tabSelected tabView::tabLeft() {
    switch (tabSelected) {
        case HOME:
        default:
            break;
        case SOURCES:
            tabSelected=HOME;
            break;
        case INSTALLED:
            tabSelected=SOURCES;
            break;
        case SEARCH:
            tabSelected=INSTALLED;
            break;
        case SETTINGS:
            tabSelected=SEARCH;
            break;
    }
    return tabSelected;
}
tabSelected tabView::tabRight() {
    switch (tabSelected) {
        case HOME:
            tabSelected=SOURCES;
            break;
        case SOURCES:
            tabSelected=INSTALLED;
            break;
        case INSTALLED:
            tabSelected=SEARCH;
            break;
        case SEARCH:
            tabSelected=SETTINGS;
            break;
        case SETTINGS:
        default:
            break;
    }
    return tabSelected;
}
int tabView::updateTab() {
    mainScene->DrawRectangle(tabRectange.x,tabRectange.y,tabRectange.w,tabRectange.h,tabRectange.color);
    switch (tabSelected) {
        case HOME:
        default:
            homeSelectedIcon->Draw(mainScene,homePos.x,homePos.y);
            sourcesIcon->Draw(mainScene,sourcesPos.x,sourcesPos.y);
            installedIcon->Draw(mainScene,installedPos.x,installedPos.y);
            searchIcon->Draw(mainScene,searchPos.x,searchPos.y);
            settingsIcon->Draw(mainScene,settingsPos.x,settingsPos.y);
            break;
        case SOURCES:
            homeIcon->Draw(mainScene,homePos.x,homePos.y);
            sourcesSelectedIcon->Draw(mainScene,sourcesPos.x,sourcesPos.y);
            installedIcon->Draw(mainScene,installedPos.x,installedPos.y);
            searchIcon->Draw(mainScene,searchPos.x,searchPos.y);
            settingsIcon->Draw(mainScene,settingsPos.x,settingsPos.y);
            break;
        case INSTALLED:
            homeIcon->Draw(mainScene,homePos.x,homePos.y);
            sourcesIcon->Draw(mainScene,sourcesPos.x,sourcesPos.y);
            installedSelectedIcon->Draw(mainScene,installedPos.x,installedPos.y);
            searchIcon->Draw(mainScene,searchPos.x,searchPos.y);
            settingsIcon->Draw(mainScene,settingsPos.x,settingsPos.y);
            break;
        case SEARCH:
            homeIcon->Draw(mainScene,homePos.x,homePos.y);
            sourcesIcon->Draw(mainScene,sourcesPos.x,sourcesPos.y);
            installedIcon->Draw(mainScene,installedPos.x,installedPos.y);
            searchSelectedIcon->Draw(mainScene,searchPos.x,searchPos.y);
            settingsIcon->Draw(mainScene,settingsPos.x,settingsPos.y);
            break;
        case SETTINGS:
            homeIcon->Draw(mainScene,homePos.x,homePos.y);
            sourcesIcon->Draw(mainScene,sourcesPos.x,sourcesPos.y);
            installedIcon->Draw(mainScene,installedPos.x,installedPos.y);
            searchIcon->Draw(mainScene,searchPos.x,searchPos.y);
            settingsSelectedIcon->Draw(mainScene,settingsPos.x,settingsPos.y);
            break;
        
    }

    return 0;
}

tabView::~tabView(){
    delete(homeIcon);
    delete(homeSelectedIcon);

    delete(sourcesIcon);
    delete(sourcesSelectedIcon);

    delete(installedIcon);
    delete(installedSelectedIcon);

    delete(searchIcon);
    delete(searchSelectedIcon);

    delete(settingsIcon);
    delete(settingsSelectedIcon);
}

tabSelected tabView::getCurrentView() {
    return tabSelected;
}
