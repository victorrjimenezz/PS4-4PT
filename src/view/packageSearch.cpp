//
// Created by Víctor Jiménez Rugama on 12/24/21.
//
#include "../../include/view/packageSearch.h"
#include "../../include/view/keyboardInput.h"
#include "../../include/repository/package.h"
#include "../../include/repository/repository.h"
#include "../../include/utils/utils.h"
#include "../../include/utils/notifi.h"
#include "../../include/file/download.h"
#include "../../include/utils/PNG.h"
#include "../../include/view/downloadView.h"
#include "../../include/view/repositoryView.h"

#include <vector>
#include <iterator>

packageSearch * packageSearch::mainPackageSearch;
packageSearch::packageSearch(Scene2D * mainScene, FT_Face fontLarge, FT_Face fontMedium, FT_Face fontSmall, int frameWidth, int frameHeight) : currPackages(), packageRectangles(), viewWidth(frameWidth), viewHeight(frameHeight * TOPVIEWSIZE) {
    this->mainScene = mainScene;
    packageSearch::mainPackageSearch = this;
    this->repositoryList = repositoryView::mainRepositoryView->getRepositoryList();

    rectangleBaseHeight = ((frameHeight-frameHeight*TABVIEWSIZE-viewHeight) / packagesPerPage);
    rectangleDivisorHeight = (rectangleBaseHeight*RECTANGLEDIVISORHEIGHT);

    bgColor = {255,255,255};
    selectedColor = {0,0,0};
    textColor = {90, 90, 90};
    updateTextColor = {255,0,0};

    this->isOnKeyboard = false;
    this->fontLarge = fontLarge;
    this->fontMedium = fontMedium;
    this->fontSmall = fontSmall;

    keyboardInput = new class keyboardInput(mainScene, fontSmall, viewWidth * KEYBOARD_X_POS, viewHeight / 2, frameWidth * (1 - KEYBOARD_X_POS * 2), viewHeight,"SEARCH", "",isOnKeyboard, false);

    updatePackages();

    currPage = 0;
    selected = 0;

    int repoX = static_cast<int>(frameWidth*REPO_X_POS);
    this->repoIconX = static_cast<int>(REPO_ICON_POS*repoX);
    for(int i =0; i < packagesPerPage; i++)
        packageRectangles[i] = {repoX, viewHeight + i * rectangleBaseHeight + rectangleBaseHeight / 2, frameWidth, rectangleBaseHeight};
    this->packageTypeX=repoX+ (packageRectangles[0].width - repoX) * PKGLIST_TYPE_POS;
    packageSearch::fillPage();

}


void packageSearch::fillPage() {
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

void packageSearch::updatePackages() {
    for(auto & package : displayPackageList)
        package.reset();
    displayPackageList.clear();
    for(auto & package : packageList)
        package.reset();
    packageList.clear();
    for(auto repository : *repositoryList){
        for(const auto& package : *(repository->getPackageList())){
            packageList.emplace_back(package);
        }
    }
    filterPackages(keyboardInput->readText());
}

void packageSearch::filterPackages(const char * name) {
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

void packageSearch::updateView() {
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
                mainScene->DrawText((char *) currPackage->getName(), fontMedium, packageRectangle.x, packageRectangle.y,
                                    textColor, textColor);
                printStr = "Version: ";
                printStr += currPackage->getVersionStr();
                printStr += " | ";
                printStr += currPackage->getTitleID();
                printStr += " | ";
                printStr += currPackage->getRepoName();
                mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageRectangle.x, packageRectangle.y + 3 * packageRectangle.height / 8,
                                    textColor, textColor);
                printStr = "Type: ";
                printStr += TypeStr[currPackage->getPackageType()];
                mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX,
                                    packageRectangle.y - 1 * packageRectangle.height / 8,
                                    textColor, textColor);
                currPackage->getIcon()->Draw(mainScene, repoIconX, packageRectangle.y - 3 * packageRectangle.height / 8);
                if(currPackage->isInstalled()) {
                    if(currPackage->getVersion()>currPackage->getCurrVer()) {
                        printStr = "Update Available ";
                        mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX, packageRectangle.y+ 1 * packageRectangle.height / 8,
                                            updateTextColor, updateTextColor);
                    } else {
                        printStr = "Installed";
                        mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX, packageRectangle.y+ 1 * packageRectangle.height / 8,
                                            textColor, textColor);
                    }
                }
                printStr = "Size: ";
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
            mainScene->DrawText((char *) currPackage->getName(), fontMedium, packageRectangle.x, packageRectangle.y,
                                selectedColor, selectedColor);
            printStr = "Version: ";
            printStr += currPackage->getVersionStr();
            printStr += " | ";
            printStr += currPackage->getTitleID();
            printStr += " | ";
            printStr += currPackage->getRepoName();
            mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageRectangle.x, packageRectangle.y + 3 * packageRectangle.height / 8,
                                selectedColor, selectedColor);
            printStr = "Type: ";
            printStr += TypeStr[currPackage->getPackageType()];
            mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX,
                                packageRectangle.y - 1 * packageRectangle.height / 8,
                                selectedColor, selectedColor);
            currPackage->getIcon()->Draw(mainScene, repoIconX, packageRectangle.y - 3 * packageRectangle.height / 8);
            if(currPackage->isInstalled()) {
                if(currPackage->getVersion()>currPackage->getCurrVer()) {
                    printStr = "Update Available ";
                    mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX, packageRectangle.y+ 1 * packageRectangle.height / 8,
                                        updateTextColor, updateTextColor);
                } else {
                    printStr = "Installed";
                    mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX, packageRectangle.y+ 1 * packageRectangle.height / 8,
                                        selectedColor, selectedColor);
                }
            }
            printStr = "Size: ";
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

void packageSearch::pressX(){
    if(isOnKeyboard)
        keyboardInput->pressKey();
    else {
        std::shared_ptr<package> currpkg = currPackages[selected];
        if(currPackages[selected] == nullptr)
            return;
        downloadView::downloadManager->addDownload(new download(currpkg));
    }
}
void packageSearch::deleteChild(){

}

subView* packageSearch::getParent(){
    return nullptr;
}

subView* packageSearch::getChild(){
    return nullptr;
}
void packageSearch::pressCircle(){
    if(isOnKeyboard) {
        isOnKeyboard = false;
        keyboardInput->unSelectKeyboard();
    }
}
void packageSearch::pressTriangle(){
    isOnKeyboard = !isOnKeyboard;
    if(isOnKeyboard)
        keyboardInput->selectKeyboard();
    else
        keyboardInput->unSelectKeyboard();
}
void packageSearch::pressSquare(){
    if(isOnKeyboard)
        keyboardInput->deleteChar();
}
void packageSearch::arrowUp(){
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
void packageSearch::arrowDown(){
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
void packageSearch::arrowRight() {
    if(isOnKeyboard)
        keyboardInput->nextKey();

}
void packageSearch::arrowLeft() {
    if(isOnKeyboard)
        keyboardInput->previousKey();

}
packageSearch::~packageSearch() {
    for(auto repository : packageList)
        repository.reset();
    repositoryList.reset();
    delete keyboardInput;
}

bool packageSearch::isActive() {
    return true;
}
