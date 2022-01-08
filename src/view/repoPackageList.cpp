//
// Created by Víctor Jiménez Rugama on 12/24/21.
//
#include "../../include/view/repoPackageList.h"
#include "../../_common/notifi.h"
#include "../../include/utils/logger.h"
#include "../../include/utils/utils.h"
#include "../../include/view/downloadView.h"
#include <vector>
#include <iterator>


repoPackageList::repoPackageList(Scene2D * mainScene, FT_Face fontLarge, FT_Face fontMedium, FT_Face fontSmall, int frameWidth, int frameHeight, repository * repository, subView * parent) : currPackages(), packageRectangles(), viewWidth(frameWidth), viewHeight(frameHeight * TOPVIEWSIZE) {
    this->mainScene = mainScene;
    this->parent = parent;
    this->child = nullptr;

    rectangleBaseHeight = ((frameHeight-frameHeight*TABVIEWSIZE-viewHeight) / packagesPerPage);
    rectangleDivisorHeight = (rectangleBaseHeight*RECTANGLEDIVISORHEIGHT);

    bgColor = {255,255,255};
    selectedColor = {0,0,0};
    textColor = {180, 180, 180};

    this->isOnKeyboard = false;
    this->active = true;
    this->fontLarge = fontLarge;
    this->fontMedium = fontMedium;
    this->fontSmall = fontSmall;

    keyboardInput = new class keyboardInput(mainScene, fontSmall, viewWidth * KEYBOARD_X_POS, viewHeight / 2, frameWidth * (1 - KEYBOARD_X_POS * 2), viewHeight, "SEARCH","",isOnKeyboard, false);

    currPage = 0;
    selected = 0;

    for(const std::shared_ptr<package>& package : *repository->getPackageList()) {
        displayPackageList.emplace_back(package);
        packageList.emplace_back(package);
    }

    int repoX = static_cast<int>(frameWidth*REPO_X_POS);
    this->repoIconX = static_cast<int>(REPO_ICON_POS*repoX);
    for(int i =0; i < packagesPerPage; i++)
        packageRectangles[i] = {repoX, viewHeight + i * rectangleBaseHeight + rectangleBaseHeight / 2, frameWidth, rectangleBaseHeight};

    this->packageTypeX=repoX+ (packageRectangles[0].width - repoX) * PKGLIST_TYPE_POS;

    repoPackageList::fillPage();

}

void repoPackageList::fillPage() {
    int j;
    size_t repoListSize = displayPackageList.size();
    for(int i =0; i < packagesPerPage; i++){
        j = currPage * packagesPerPage + i;
        if(repoListSize<=j)
            currPackages[i] = nullptr;
        else
            currPackages[i] = displayPackageList.at(j);
    }
    if(repoListSize <= selected)
        selected = repoListSize-1;
    if(selected < 0)
        selected =0;
}

void repoPackageList::filterPackages(const char * name) {
    displayPackageList.clear();
    for(auto & package : packageList){
        std::string hayStack =  package->getName();
        std::string needle = name;
        if (findStringIC(hayStack,needle)) {
            displayPackageList.emplace_back(package);
        }
    }
    fillPage();
}

void repoPackageList::updateView() {
    if(keyboardInput->hasChanged())
        filterPackages(keyboardInput->readText());
    std::string printStr;
    for(int i =0; i < packagesPerPage; i++){
        std::shared_ptr<package> currPackage = currPackages[i];
        packageRectangle packageRectangle = packageRectangles[i];
        if(selected != i || isOnKeyboard){
            mainScene->DrawRectangle(0, packageRectangle.y - packageRectangle.height / 2, packageRectangle.width, rectangleDivisorHeight, textColor);
            mainScene->DrawRectangle(0, packageRectangle.y + packageRectangle.height / 2 - rectangleDivisorHeight / 2, packageRectangle.width, rectangleDivisorHeight, textColor);
            if(currPackage != nullptr){
                mainScene->DrawText((char *) std::string(currPackage->getName()).substr(0,PKGLIST_CHARACTER_LIMIT).c_str(), fontMedium, packageRectangle.x, packageRectangle.y,
                                    selectedColor, textColor);
                printStr = "Type: ";
                printStr+=TypeStr[currPackage->getPackageType()];
                mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX, packageRectangle.y- 1 * packageRectangle.height / 8,
                                    selectedColor, selectedColor);
                printStr = "Version: ";
                printStr += currPackage->getVersion();
                mainScene->DrawText((char *) printStr.substr(0,DOWNLOAD_CHARACTER_LIMIT).c_str(), fontSmall, packageRectangle.x, packageRectangle.y + 3 * packageRectangle.height / 8,
                                    selectedColor, textColor);
                currPackage->getIcon()->Draw(mainScene, repoIconX, packageRectangle.y - 3 * packageRectangle.height / 8);
            }
        }
    }
    bool empty = displayPackageList.empty();
    if(!isOnKeyboard) {
        packageRectangle packageRectangle = packageRectangles[selected];
        if(!empty) {
            mainScene->DrawRectangle(0, packageRectangle.y - packageRectangle.height / 2, packageRectangle.width,
                                     rectangleDivisorHeight, selectedColor);
            mainScene->DrawRectangle(0, packageRectangle.y + packageRectangle.height / 2 - rectangleDivisorHeight / 2,
                                     packageRectangle.width, rectangleDivisorHeight, selectedColor);
            std::shared_ptr<package> currPackage = currPackages[selected];
            mainScene->DrawText((char *) std::string(currPackage->getName()).substr(0, PKGLIST_CHARACTER_LIMIT).c_str(),
                                fontMedium, packageRectangle.x, packageRectangle.y,
                                selectedColor, selectedColor);
            printStr = "Type: ";
            printStr += TypeStr[currPackage->getPackageType()];
            mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX,
                                packageRectangle.y - 1 * packageRectangle.height / 8,
                                selectedColor, selectedColor);
            printStr = "Version: ";
            printStr += currPackage->getVersion();
            mainScene->DrawText((char *) printStr.substr(0, DOWNLOAD_CHARACTER_LIMIT).c_str(), fontSmall,
                                packageRectangle.x, packageRectangle.y + 3 * packageRectangle.height / 8,
                                selectedColor, textColor);
            currPackage->getIcon()->Draw(mainScene, repoIconX, packageRectangle.y - 3 * packageRectangle.height / 8);
        } else {
            mainScene->DrawRectangle(0, packageRectangle.y - packageRectangle.height / 2, packageRectangle.width, rectangleDivisorHeight, textColor);
            mainScene->DrawRectangle(0, packageRectangle.y + packageRectangle.height / 2 - rectangleDivisorHeight / 2, packageRectangle.width, rectangleDivisorHeight, textColor);
        }
    }
    keyboardInput->updateView();
}

void repoPackageList::pressX(){
    if(isOnKeyboard)
        keyboardInput->pressKey();
    else
        downloadView::downloadManager->addDownload( new download(currPackages[selected]));
}
void repoPackageList::deleteChild(){
    delete child;
    child = nullptr;
}

subView* repoPackageList::getParent(){
    return parent;
}

subView* repoPackageList::getChild(){
    return child;
}
void repoPackageList::pressCircle(){
    active=false;
}
void repoPackageList::pressTriangle(){
    isOnKeyboard = !isOnKeyboard;
    if(isOnKeyboard)
        keyboardInput->selectKeyboard();
    else
        keyboardInput->unSelectKeyboard();
}
void repoPackageList::pressSquare(){
    if(isOnKeyboard)
        keyboardInput->deleteChar();
}
void repoPackageList::arrowUp(){
    if(!isOnKeyboard) {
        selected--;
        if (selected + packagesPerPage * currPage < 0) {
            selected = 0;
        } else if (selected < 0) {
            selected = packagesPerPage - 1;
            currPage--;
            fillPage();
        }
    } else
        keyboardInput->setUpperRow();
}
void repoPackageList::arrowDown(){
    if(!isOnKeyboard) {
        selected++;
        int repoSize = (int) packageList.size();
        if (selected + packagesPerPage * currPage < repoSize) {
            if (selected >= packagesPerPage) {
                selected = 0;
                currPage++;
                fillPage();
            }
        } else {
            selected--;
        }
    } else
        keyboardInput->setLowerRow();
}
void repoPackageList::arrowRight() {
    if(isOnKeyboard)
        keyboardInput->nextKey();

}
void repoPackageList::arrowLeft() {
    if(isOnKeyboard)
        keyboardInput->previousKey();

}
repoPackageList::~repoPackageList() {
    for(auto repository : packageList)
        repository.reset();
    delete keyboardInput;
}

bool repoPackageList::isActive() {
    return active;
}
