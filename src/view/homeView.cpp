//
// Created by Víctor Jiménez Rugama on 12/24/21.
//

#include "../../include/view/homeView.h"
#include "../../include/view/downloadView.h"
#include "../../include/view/keyboardInput.h"
#include "../../include/utils/logger.h"
#include "../../include/utils/notifi.h"
#include "../../include/utils/dialog.h"
#include "../../include/file/fileDownloadRequest.h"
#include "../../include/file/download.h"
#include "../../include/utils/PNG.h"

#include <iterator>
#include <orbis/UserService.h>

homeView::homeView(Scene2D * mainScene, FT_Face fontLarge, FT_Face fontMedium, FT_Face fontSmall, int frameWidth, int frameHeight) : frameWidth(frameWidth),frameHeight(frameHeight){
    this->mainScene = mainScene;

    bgColor = {255,255,255};
    textColor = {0, 0, 0};

    this->fontLarge = fontLarge;
    this->fontMedium = fontMedium;
    this->fontSmall = fontSmall;

    keyboardX = HOMEVIEW_KEYBOARD_X*frameWidth;
    keyboardInput = new class keyboardInput(mainScene, fontSmall, keyboardX, HOMEVIEW_KEYBOARD_Y*frameHeight, frameWidth * (1 - HOMEVIEW_KEYBOARD_X * 2), HOMEVIEW_KEYBOARD_HEIGHT*frameHeight,"DOWNLOAD", "https://",true);

    logoWidth = HOMEVIEW_LOGO_WIDTH*frameWidth;
    logoX = HOMEVIEW_LOGO_X*frameWidth-logoWidth/2;
    logoY = HOMEVIEW_LOGO_Y*frameHeight;
    logoHeight = logoWidth;
    logo = new PNG(LOGO_PATH,logoWidth,logoHeight);
    int UID;
    sceUserServiceGetInitialUser(&UID);
    char usernameChar[20];
    sceUserServiceGetUserName(UID, usernameChar, 20);
    std::string username = std::string(usernameChar);
    std::ostringstream out;
    out.precision(2);
    out << std::fixed << APP_VERSION;

    welcomeText = "Welcome " + username + " to 4PT\n Version: " + out.str();
    welcomeSubText = "Developed by @victorrjimenezz";
    pkgLink = "PKG Direct Download:";

}

void homeView::updateView() {
    logo->Draw(mainScene,logoX,logoY);
    mainScene->DrawText((char*) welcomeText.c_str(),fontLarge,100,400,bgColor,textColor);
    mainScene->DrawText((char*) welcomeSubText.c_str(),fontMedium,100,600,bgColor,textColor);;
    mainScene->DrawText((char*) pkgLink.c_str(),fontMedium,keyboardX,700,bgColor,textColor);
    keyboardInput->updateView();
    if(keyboardInput->hasEntered())
        hasEntered();
}
void homeView::hasEntered() {
    std::string inputText = keyboardInput->readText();

    if(!fileDownloadRequest::verifyURL(inputText.c_str())) {
        popDialog("INVALID URL");
        return;
    }

    bool initFailed;


    auto * download = new class download(inputText.c_str(),&initFailed);
    if(initFailed) {
        notifi(NULL,std::string("COULD NOT DOWNLOAD PKG FROM\n"+inputText).c_str());
        return;
    }
    downloadView::downloadManager->addDownload( download);
}
void homeView::pressX(){
    keyboardInput->pressKey();
}

void homeView::pressCircle(){

}
void homeView::pressTriangle(){

}
void homeView::pressSquare(){
    keyboardInput->deleteChar();
}
void homeView::arrowUp(){
    keyboardInput->setUpperRow();
}
void homeView::arrowDown(){
    keyboardInput->setLowerRow();
}
void homeView::arrowRight() {
    keyboardInput->nextKey();

}
void homeView::arrowLeft() {
    keyboardInput->previousKey();

}
bool homeView::isActive() {
    return true;
}
homeView::~homeView() {
    delete keyboardInput;
    delete logo;
}

subView *homeView::getParent() {
    return nullptr;
}

subView *homeView::getChild() {
    return nullptr;
}
