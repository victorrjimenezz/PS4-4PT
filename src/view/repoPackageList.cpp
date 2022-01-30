//
// Created by Víctor Jiménez Rugama on 12/24/21.
//
#include "../../include/view/repoPackageList.h"
#include "../../include/view/keyboardInput.h"
#include "../../include/utils/notifi.h"
#include "../../include/utils/utils.h"
#include "../../include/utils/PNG.h"
#include "../../include/utils/LANG.h"
#include "../../include/repository/repository.h"
#include "../../include/view/downloadView.h"
#include "../../include/file/download.h"
#include "../../include/utils/logger.h"

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
    updateTextColor = {255, 0, 0};

    this->isOnKeyboard = false;
    this->active = false;
    this->fontLarge = fontLarge;
    this->fontMedium = fontMedium;
    this->fontSmall = fontSmall;

    keyboardInput = new class keyboardInput(mainScene, fontSmall, viewWidth * KEYBOARD_X_POS, viewHeight / 2, frameWidth * (1 - KEYBOARD_X_POS * 2), viewHeight, LANG::mainLang->SEARCH.c_str(),"",isOnKeyboard, false);

    currPage = 0;
    selected = 0;

    repo = repository;
    this->packageList = repository->getPackageList();
    filterPackages("");

    int repoX = static_cast<int>(frameWidth*REPO_X_POS);
    this->repoIconX = static_cast<int>(REPO_ICON_POS*repoX);
    for(int i =0; i < packagesPerPage; i++)
        packageRectangles[i] = {repoX, viewHeight + i * rectangleBaseHeight + rectangleBaseHeight / 2, frameWidth, rectangleBaseHeight};

    this->packageTypeX=repoX+ (packageRectangles[0].width - repoX) * PKGLIST_TYPE_POS;

}

void repoPackageList::fillPage() {
    int j;
    size_t repoListSize = displayPackageList.size();
    for(int i =0; i < packagesPerPage; i++){
        j = currPage * packagesPerPage + i;
        if(j < repoListSize)
            currPackages[i] = displayPackageList.at(j);
        else
            currPackages[i] = nullptr;
    }
    if(repoListSize <= selected)
        selected = repoListSize-1;
    if(selected < 0)
        selected =0;
}

void repoPackageList::filterPackages(const char * name) {
    //TODO IF NAME IS SMALLER DO RESET EVERYTHING, IF NAME IS LONGER ONLY FILTER THOSE ALREADY FILTERED
    for(auto pkg : displayPackageList)
        pkg.reset();
    displayPackageList.clear();
    if(strcasecmp(name,"")==0) {
        for (auto &package: *packageList) {
            displayPackageList.emplace_back(package);
        }
    }else {

        currPage = 0;
        selected = 0;
        std::string needle = name;
        std::transform(needle.begin(), needle.end(), needle.begin(), ::toupper);

        for (auto &package: *packageList) {
            std::string hayStack = package->getName();
            std::transform(hayStack.begin(), hayStack.end(), hayStack.begin(), ::toupper);

            if (hayStack.find(needle) != std::string::npos) {

                displayPackageList.emplace_back(package);
            }
        }
    }


    fillPage();
}

void repoPackageList::updateView() {
    if(keyboardInput->hasChanged())
        filterPackages(keyboardInput->readText().c_str());

    std::string printStr;
    int totalPages = ceil((double)displayPackageList.size()/packagesPerPage);
    if(totalPages ==0) totalPages++;
    if(currPage+1<10)
        printStr+="0";
    printStr+= std::to_string(currPage+1) + "/";
    if(totalPages<10)
        printStr+="0";
    printStr+= std::to_string(totalPages);
        mainScene->DrawText((char *) printStr.c_str(), fontSmall, viewWidth*CURR_PAGE_X, viewHeight*CURR_PAGE_Y,
                            selectedColor, selectedColor);
    for(int i =0; i < packagesPerPage; i++){
        std::shared_ptr<package> currPackage = currPackages[i];
        packageRectangle packageRectangle = packageRectangles[i];
        if(selected != i || isOnKeyboard){
            mainScene->DrawRectangle(0, packageRectangle.y - packageRectangle.height / 2, packageRectangle.width, rectangleDivisorHeight, textColor);
            mainScene->DrawRectangle(0, packageRectangle.y + packageRectangle.height / 2 - rectangleDivisorHeight / 2, packageRectangle.width, rectangleDivisorHeight, textColor);
            if(currPackage != nullptr){
                mainScene->DrawText((char *) std::string(currPackage->getName()).substr(0,PKGLIST_CHARACTER_LIMIT).c_str(), fontMedium, packageRectangle.x, packageRectangle.y,
                                    textColor, textColor);
                printStr = LANG::mainLang->TYPE;
                printStr+=TypeStr[currPackage->getPackageType()];
                mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX, packageRectangle.y- 1 * packageRectangle.height / 8,
                                    textColor, textColor);
                printStr = LANG::mainLang->VERSION;
                printStr += currPackage->getVersionStr();
                printStr += " | ";
                printStr += currPackage->getTitleID();
                mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageRectangle.x, packageRectangle.y + 3 * packageRectangle.height / 8,
                                    textColor, textColor);
                if(currPackage->isInstalled()) {
                    if(currPackage->getVersion()>currPackage->getCurrVer()) {
                        printStr = LANG::mainLang->UPDATE_AVAILABLE;
                        printStr += currPackage->getSFOType();
                        mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX, packageRectangle.y+ 1 * packageRectangle.height / 8,
                                            updateTextColor, updateTextColor);
                    } else {
                        printStr = LANG::mainLang->INSTALLED;
                        printStr += currPackage->getSFOType();
                        mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX, packageRectangle.y+ 1 * packageRectangle.height / 8,
                                            textColor, textColor);
                    }
                } else if(strlen(currPackage->getSFOType())>1) {
                    printStr = currPackage->getSFOType();
                    mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX, packageRectangle.y+ 1 * packageRectangle.height / 8,
                                        textColor, textColor);
                }
                currPackage->getIcon()->Draw(mainScene, repoIconX, packageRectangle.y - 3 * packageRectangle.height / 8);
                printStr = LANG::mainLang->SIZE;
                printStr+= currPackage->getPkgSizeMB();
                printStr += "MB";
                mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX,
                                    packageRectangle.y + 3 * packageRectangle.height / 8,
                                    textColor, textColor);
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
            printStr = LANG::mainLang->TYPE;
            printStr += TypeStr[currPackage->getPackageType()];
            mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX,
                                packageRectangle.y - 1 * packageRectangle.height / 8,
                                selectedColor, selectedColor);
            printStr = LANG::mainLang->VERSION;
            printStr += currPackage->getVersionStr();
            printStr += " | ";
            printStr += currPackage->getTitleID();
            mainScene->DrawText((char *) printStr.c_str(), fontSmall,
                                packageRectangle.x, packageRectangle.y + 3 * packageRectangle.height / 8,
                                selectedColor, selectedColor);
            currPackage->getIcon()->Draw(mainScene, repoIconX, packageRectangle.y - 3 * packageRectangle.height / 8);
            if(currPackage->isInstalled()) {
                if(currPackage->getVersion()>currPackage->getCurrVer()) {
                    printStr = LANG::mainLang->UPDATE_AVAILABLE;
                    printStr += currPackage->getSFOType();
                    mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX, packageRectangle.y+ 1 * packageRectangle.height / 8,
                                        updateTextColor, updateTextColor);
                } else {
                    printStr = LANG::mainLang->INSTALLED;
                    printStr += currPackage->getSFOType();
                    mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX, packageRectangle.y+ 1 * packageRectangle.height / 8,
                                        selectedColor, selectedColor);
                }
            } else if(strlen(currPackage->getSFOType())>1) {
                printStr = currPackage->getSFOType();
                mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX, packageRectangle.y+ 1 * packageRectangle.height / 8,
                                    selectedColor, selectedColor);
            }
            printStr = LANG::mainLang->SIZE;
            printStr+= currPackage->getPkgSizeMB();
            printStr += "MB";
            mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX,
                                packageRectangle.y + 3 * packageRectangle.height / 8,
                                selectedColor, selectedColor);
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
    else {
        std::shared_ptr<package> currpkg = currPackages[selected];
        if(currPackages[selected] == nullptr)
            return;
        downloadView::downloadManager->addDownload(new download(currpkg));
    }
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

    if(isOnKeyboard) {
        isOnKeyboard = !isOnKeyboard;
        keyboardInput->unSelectKeyboard();
    } else {
        active = false;
    }
}
void repoPackageList::setActive() {
    if(repo->hasUpdated())
        filterPackages(std::string(keyboardInput->readText()).c_str());
    active = true;
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
        int repoSize = (int) displayPackageList.size();
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
    for(auto pkg : displayPackageList)
        pkg.reset();
    displayPackageList.clear();
    delete keyboardInput;
}

bool repoPackageList::isActive() {
    return active;
}

void repoPackageList::langChanged() {
        delete keyboardInput;
        keyboardInput = new class keyboardInput(mainScene, fontSmall, viewWidth * KEYBOARD_X_POS, viewHeight / 2, viewWidth * (1 - KEYBOARD_X_POS * 2), viewHeight, LANG::mainLang->SEARCH.c_str(),"",isOnKeyboard, false);
}
