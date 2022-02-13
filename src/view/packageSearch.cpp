//
// Created by Víctor Jiménez Rugama on 12/24/21.
//
#include "../../include/view/packageSearch.h"
#include "../../include/view/keyboardInput.h"
#include "../../include/repository/package.h"
#include "../../include/repository/repository.h"
#include "../../include/utils/utils.h"
#include "../../include/file/download.h"
#include "../../include/utils/PNG.h"
#include "../../include/utils/LANG.h"
#include "../../include/view/downloadView.h"
#include "../../include/view/repositoryView.h"
#include "../../include/view/filterView.h"
#include "../../include/utils/settings.h"

#include <vector>
#include <iterator>

filterView::sort packageSearch::currentSort = filterView::ALPHABET;
filterView::sortOrder packageSearch::currentSortOrder = filterView::ASCENDANT;
packageSearch * packageSearch::mainPackageSearch = nullptr;
packageSearch::packageSearch(Scene2D * mainScene, FT_Face fontLarge, FT_Face fontMedium, FT_Face fontSmall, int frameWidth, int frameHeight) : currPackages(), packageRectangles(), viewWidth(frameWidth), viewHeight(frameHeight * TOPVIEWSIZE), updatePKGSMutex(), filterPKGSMutex(), fillPageMutex() {
    this->mainScene = mainScene;
    this->repositoryList = repositoryView::mainRepositoryView->getRepositoryList();

    rectangleBaseHeight = ((frameHeight-frameHeight*TABVIEWSIZE-viewHeight) / packagesPerPage);
    rectangleDivisorHeight = (rectangleBaseHeight*RECTANGLEDIVISORHEIGHT);

    bgColor = {255,255,255};
    selectedColor = {0,0,0};
    textColor = {180, 180, 180};
    updateTextColor = {255,0,0};

    this->fontLarge = fontLarge;
    this->fontMedium = fontMedium;
    this->fontSmall = fontSmall;

    keyboardInput = new class keyboardInput(mainScene, fontSmall, viewWidth * KEYBOARD_X_POS, viewHeight / 2, frameWidth * (1 - KEYBOARD_X_POS * 2), viewHeight/2,LANG::mainLang->SEARCH.c_str(), "");
    filterView = new class filterView(mainScene, fontSmall, viewWidth * KEYBOARD_X_POS, 3*viewHeight / 4, frameWidth * (1 - KEYBOARD_X_POS * 2), viewHeight/2);


    currPage = 0;
    selected = 0;

    int repoX = static_cast<int>(frameWidth*REPO_X_POS);
    this->repoIconX = static_cast<int>(REPO_ICON_POS*repoX);
    for(int i =0; i < packagesPerPage; i++)
        packageRectangles[i] = {repoX, viewHeight + i * rectangleBaseHeight + rectangleBaseHeight / 2, frameWidth, rectangleBaseHeight};
    this->packageTypeX=repoX+ (packageRectangles[0].width - repoX) * PKGLIST_TYPE_POS;
    packageSearch::mainPackageSearch = this;
}


void packageSearch::fillPage() {
    std::unique_lock<std::mutex> lock(fillPageMutex);

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
    std::unique_lock<std::mutex> lock(updatePKGSMutex);

    displayPackageList.clear();
    packageList.clear();

    for(auto repository : *repositoryList){
        for(const auto& package : *(repository->getPackageList())){
            packageList.push_back(package);
        }
    }
    filterPackages(keyboardInput->readText().c_str());
}

void packageSearch::filterPackages(const char * name) {
    std::unique_lock<std::mutex> lock(filterPKGSMutex);

    displayPackageList.clear();

        currPage = 0;
        selected = 0;
        for (auto &package: packageList) {
            std::string hayStack = package->getName();
            std::string needle = name;
            std::transform(hayStack.begin(), hayStack.end(), hayStack.begin(), ::toupper);
            std::transform(needle.begin(), needle.end(), needle.begin(), ::toupper);

            if ((strcasecmp(name,"")==0 || hayStack.find(needle) != std::string::npos) && meetsCriteria(package)) {
                displayPackageList.emplace_back(package);
            }
        }


    currentSort = filterView->getCurrentSort();
    currentSortOrder = filterView->getCurrentSortOrder();
    std::sort(displayPackageList.begin(), displayPackageList.end(), &packageSearch::packageCompare);
    fillPage();
}

void packageSearch::updateView() {
    int selectedTemp;
    if(keyboardInput->hasEntered() || filterView->hasChanged())
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
    selectedTemp = selected;
    for(int i =0; i < packagesPerPage; i++){
        std::shared_ptr<package> currPackage = currPackages[i];
        packageRectangle packageRectangle = packageRectangles[i];
        if(selectedTemp != i || keyboardInput->active() || filterView->active()){
            mainScene->DrawRectangle(0, packageRectangle.y - packageRectangle.height / 2, packageRectangle.width, rectangleDivisorHeight, textColor);
            mainScene->DrawRectangle(0, packageRectangle.y + packageRectangle.height / 2 - rectangleDivisorHeight / 2, packageRectangle.width, rectangleDivisorHeight, textColor);
            if(currPackage != nullptr){
                mainScene->DrawText((char *) std::string(currPackage->getName()).substr(0,PKGLIST_CHARACTER_LIMIT).c_str(), fontMedium, packageRectangle.x, packageRectangle.y,
                                    textColor, textColor);
                printStr = LANG::mainLang->VERSION;
                printStr += ": ";
                printStr += currPackage->getVersionStr();
                printStr += " | ";
                printStr += currPackage->getTitleID();
                printStr += " | ";
                printStr += currPackage->getRepoName();
                if(currPackage->getSystemVersion() > 0) {
                    printStr += " | "+LANG::mainLang->FOR_FW;;
                    printStr +=currPackage->getSystemVersionStr();
                    printStr+="+";
                }
                mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageRectangle.x, packageRectangle.y + 3 * packageRectangle.height / 8,
                                    textColor, textColor);
                printStr = LANG::mainLang->TYPE;
                printStr += TypeStr[currPackage->getPackageType()];
                mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX,
                                    packageRectangle.y - 1 * packageRectangle.height / 8,
                                    textColor, textColor);
                currPackage->getIcon()->Draw(mainScene, repoIconX, packageRectangle.y - 3 * packageRectangle.height / 8);
                if(currPackage->isInstalled()) {
                    if(currPackage->updateAvailable()) {
                        printStr = LANG::mainLang->UPDATE_AVAILABLE;
                        mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX, packageRectangle.y+ 1 * packageRectangle.height / 8,
                                            updateTextColor, updateTextColor);
                    } else {
                        printStr = LANG::mainLang->INSTALLED;
                        mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX, packageRectangle.y+ 1 * packageRectangle.height / 8,
                                            textColor, textColor);
                    }
                }
                printStr = LANG::mainLang->SIZE;
                printStr += ": ";
                printStr+= currPackage->getPkgSizeMB();
                printStr += "MB";
                mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX,
                                    packageRectangle.y + 3 * packageRectangle.height / 8,
                                    textColor, textColor);
            }
        }
    }
    bool empty = displayPackageList.empty();
    if(!keyboardInput->active() && !filterView->active()) {
        packageRectangle packageRectangle = packageRectangles[selectedTemp];
        if(!empty) {
        mainScene->DrawRectangle(0, packageRectangle.y - packageRectangle.height / 2, packageRectangle.width,
                                 rectangleDivisorHeight, selectedColor);
        mainScene->DrawRectangle(0, packageRectangle.y + packageRectangle.height / 2 - rectangleDivisorHeight / 2,
                                 packageRectangle.width, rectangleDivisorHeight, selectedColor);
            std::shared_ptr<package> currPackage = currPackages[selectedTemp];
            mainScene->DrawText((char *) std::string(currPackage->getName()).substr(0,PKGLIST_CHARACTER_LIMIT).c_str(), fontMedium, packageRectangle.x, packageRectangle.y,
                                selectedColor, selectedColor);
            printStr = LANG::mainLang->VERSION;
            printStr += ": ";
            printStr += currPackage->getVersionStr();
            printStr += " | ";
            printStr += currPackage->getTitleID();
            printStr += " | ";
            printStr += currPackage->getRepoName();
            if(currPackage->getSystemVersion() > 0) {
                printStr += " | "+LANG::mainLang->FOR_FW;;
                printStr +=currPackage->getSystemVersionStr();
                printStr+="+";
            }
            mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageRectangle.x, packageRectangle.y + 3 * packageRectangle.height / 8,
                                selectedColor, selectedColor);
            printStr = LANG::mainLang->TYPE;
            printStr += TypeStr[currPackage->getPackageType()];
            mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX,
                                packageRectangle.y - 1 * packageRectangle.height / 8,
                                selectedColor, selectedColor);
            currPackage->getIcon()->Draw(mainScene, repoIconX, packageRectangle.y - 3 * packageRectangle.height / 8);
            if(currPackage->isInstalled()) {
                if(currPackage->updateAvailable()) {
                    printStr = LANG::mainLang->UPDATE_AVAILABLE;
                    mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX, packageRectangle.y+ 1 * packageRectangle.height / 8,
                                        updateTextColor, updateTextColor);
                } else {
                    printStr = LANG::mainLang->INSTALLED;
                    mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX, packageRectangle.y+ 1 * packageRectangle.height / 8,
                                        selectedColor, selectedColor);
                }
            }
            printStr = LANG::mainLang->SIZE;
            printStr += ": ";
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
    filterView->updateView();
}

void packageSearch::pressX(){
    if(filterView->active())
        filterView->pressX();
    else {
        std::shared_ptr<package> currpkg = currPackages[selected];
        if(currPackages[selected] == nullptr)
            return;
        if(settings::getMainSettings()->shouldInstallDirectlyPS4())
            currpkg->install();
        else
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

}
void packageSearch::pressTriangle(){
    keyboardInput->enableKeyboard();
}
void packageSearch::pressSquare(){
    if(!filterView->active())
        filterView->enableFilterView();
    else
        filterView->disableFilterView();
}
void packageSearch::arrowUp(){
    if(filterView->active())
        filterView->arrowUP();
    else {
        selected--;
        if (selected + packagesPerPage * currPage < 0) {
            selected = 0;
        } else if (selected < 0) {
            selected = packagesPerPage - 1;
            currPage--;
            fillPage();
        }
    }
}
void packageSearch::arrowDown(){
    if(filterView->active())
        filterView->arrowDown();
    else  {
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
    }
}
void packageSearch::arrowRight() {
    if(filterView->active())
        filterView->nextOption();

}
void packageSearch::arrowLeft() {
    if(filterView->active())
        filterView->prevOption();

}
packageSearch::~packageSearch() {
    for(auto repository : packageList)
        repository.reset();
    repositoryList.reset();
    delete filterView;
    delete keyboardInput;
}

bool packageSearch::isActive() {
    return true;
}

void packageSearch::langChanged() {
        delete keyboardInput;
        keyboardInput =  new class keyboardInput(mainScene, fontSmall, viewWidth * KEYBOARD_X_POS, viewHeight / 2, viewWidth * (1 - KEYBOARD_X_POS * 2), viewHeight/2,LANG::mainLang->SEARCH.c_str(), "");
        delete filterView;
        filterView = new class filterView(mainScene, fontSmall, viewWidth * KEYBOARD_X_POS, 3*viewHeight / 4, viewWidth * (1 - KEYBOARD_X_POS * 2), viewHeight/2);

}
bool packageSearch::meetsCriteria(std::shared_ptr<package> &sharedPtr) {
    if(!filterView->isEnabled(sharedPtr->getPackageType()))
        return false;
    if(filterView->onlyUpdates())
        return sharedPtr->updateAvailable() && sharedPtr->isInstalled();
    return true;
}

bool packageSearch::packageCompare(const std::shared_ptr<package> &lhs, const std::shared_ptr<package> &rhs) {
    std::string lhsName= lhs->getName();
    std::string rhsName = rhs->getName();

    bool value;
    if(currentSortOrder == filterView::ASCENDANT) {
        switch (currentSort) {
            case filterView::SIZE:
                value = lhs->getPkgSize() > rhs->getPkgSize();
                break;
            case filterView::VERSION:
                value = lhs->getVersion() > rhs->getVersion();
                break;
            case filterView::ALPHABET:
            default:
                std::transform(lhsName.begin(), lhsName.end(), lhsName.begin(), ::toupper);
                std::transform(rhsName.begin(), rhsName.end(), rhsName.begin(), ::toupper);
                value = lhsName.compare(rhsName) < 0;
                break;
        }
    } else {
        switch (currentSort) {
            case filterView::SIZE:
                value = lhs->getPkgSize() < rhs->getPkgSize();
                break;
            case filterView::VERSION:
                value = lhs->getVersion() < rhs->getVersion();
                break;
            case filterView::ALPHABET:
            default:
                std::transform(lhsName.begin(), lhsName.end(), lhsName.begin(), ::toupper);
                std::transform(rhsName.begin(), rhsName.end(), rhsName.begin(), ::toupper);
                value = lhsName.compare(rhsName) > 0;
                break;
        }
    }

    return value;
}