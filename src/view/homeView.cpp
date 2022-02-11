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
#include "../../include/utils/LANG.h"

#include <iterator>
#include <codecvt>
#include <orbis/UserService.h>
#include <orbis/ImeDialog.h>

homeView::homeView(Scene2D * mainScene, FT_Face fontLarge, FT_Face fontMedium, FT_Face fontSmall, int frameWidth, int frameHeight) : frameWidth(frameWidth),frameHeight(frameHeight){
    this->mainScene = mainScene;

    bgColor = {255,255,255};
    textColor = {0, 0, 0};

    this->fontLarge = fontLarge;
    this->fontMedium = fontMedium;
    this->fontSmall = fontSmall;

    keyboardX = HOMEVIEW_KEYBOARD_X*frameWidth;

    keyboardInput = new class keyboardInput(mainScene, fontSmall, keyboardX, HOMEVIEW_KEYBOARD_Y*frameHeight, frameWidth * (1 - HOMEVIEW_KEYBOARD_X * 2), HOMEVIEW_KEYBOARD_HEIGHT*frameHeight, LANG::mainLang->PKG_DIRECT_DOWNLOAD.c_str(), "https://",ORBIS_TYPE_TYPE_URL,ORBIS_BUTTON_LABEL_GO);

    logoWidth = HOMEVIEW_LOGO_WIDTH*frameWidth;
    logoX = HOMEVIEW_LOGO_X*frameWidth-logoWidth/2;
    logoY = HOMEVIEW_LOGO_Y*frameHeight;
    logoHeight = logoWidth;
    logo = new PNG(LOGO_PATH,logoWidth,logoHeight);
    loadWelcomeText();
}
void homeView::loadWelcomeText(){
    int UID;
    sceUserServiceGetInitialUser(&UID);
    char usernameChar[20];
    sceUserServiceGetUserName(UID, usernameChar, 20);
    std::string username = std::string(usernameChar);
    std::ostringstream out;
    out.precision(2);
    out << std::fixed << APP_VERSION;
    std::string welcomeString(LANG::mainLang->WELCOME_MESSAGE);
    welcomeString.replace(welcomeString.find(".user."),6,username);
    welcomeText = welcomeString + ": " + out.str();
    welcomeSubText = std::string(LANG::mainLang->DEVELOPED_BY) + "@victorrjimenezz";
    pkgLink = LANG::mainLang->PKG_DIRECT_DOWNLOAD;
}
void homeView::langChanged() {
    loadWelcomeText();
    delete keyboardInput;
    keyboardInput = new class keyboardInput(mainScene, fontSmall, keyboardX, HOMEVIEW_KEYBOARD_Y*frameHeight, frameWidth * (1 - HOMEVIEW_KEYBOARD_X * 2), HOMEVIEW_KEYBOARD_HEIGHT*frameHeight,LANG::mainLang->PKG_DIRECT_DOWNLOAD.c_str(), "https://",ORBIS_TYPE_TYPE_URL,ORBIS_BUTTON_LABEL_GO);
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
        popDialog((LANG::mainLang->INVALID_URL + inputText).c_str());
        return;
    }

    bool initFailed;


    auto * download = new class download(inputText.c_str(),&initFailed);
    if(initFailed) {
        notifi(NULL,(LANG::mainLang->FAILED_TO_DOWNLOAD_PKG_FROM+inputText).c_str());
        return;
    }
    downloadView::downloadManager->addDownload( download);
}
void homeView::pressX(){

}

void homeView::pressCircle(){

}

void homeView::pressTriangle(){
    keyboardInput->enableKeyboard();
}
void homeView::pressSquare(){

}
void homeView::arrowUp(){

}
void homeView::arrowDown(){

}
void homeView::arrowRight() {


}
void homeView::arrowLeft() {
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
