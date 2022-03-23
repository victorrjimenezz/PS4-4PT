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
#include "../../include/utils/settings.h"
#include "../../include/main.h"

#include <vector>
#include <iterator>

filterView::sort repoPackageList::currentSort = filterView::ALPHABET;
filterView::sortOrder repoPackageList::currentSortOrder = filterView::ASCENDANT;

repoPackageList::repoPackageList(Scene2D * mainScene, FT_Face fontLarge, FT_Face fontMedium, FT_Face fontSmall, int frameWidth, int frameHeight, repository * repository, subView * parent) : filterMtx(),displayPackageList(),currPackages(), packageRectangles(), viewWidth(frameWidth), viewHeight(frameHeight * TOPVIEWSIZE) {
    this->mainScene = mainScene;
    this->parent = parent;
    this->child = nullptr;

    rectangleBaseHeight = ((frameHeight-frameHeight*TABVIEWSIZE-viewHeight) / packagesPerPage);
    rectangleDivisorHeight = (rectangleBaseHeight*RECTANGLEDIVISORHEIGHT);

    bgColor = {255,255,255};
    selectedColor = {0,0,0};
    textColor = {180, 180, 180};
    updateTextColor = {255, 0, 0};

    this->active = false;
    this->fontLarge = fontLarge;
    this->fontMedium = fontMedium;
    this->fontSmall = fontSmall;

    keyboardInput = new class keyboardInput(mainScene, fontSmall, viewWidth * KEYBOARD_X_POS, viewHeight / 2, frameWidth * (1 - KEYBOARD_X_POS * 2), viewHeight/2, getMainLang()->SEARCH.c_str(),"");
    filterView = new class filterView(mainScene, fontSmall, viewWidth * KEYBOARD_X_POS, 3*viewHeight / 4, frameWidth * (1 - KEYBOARD_X_POS * 2), viewHeight/2);

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
    int j, offset = currPage * packagesPerPage;
    size_t repoListSize = displayPackageList.size();
    auto it = displayPackageList.begin();
    if(repoListSize > offset)
        std::advance(it, offset);
    for(int i =0; i < packagesPerPage; i++){
        j = offset + i;
        if(j < repoListSize) {
            currPackages[i] = *it;
            it++;
        } else
            currPackages[i] = nullptr;

    }
    if(repoListSize <= selected)
        selected = repoListSize-1;
    if(selected < 0)
        selected =0;
}

void repoPackageList::filterPackages(const char * name) {
    std::unique_lock<std::mutex> lock(filterMtx);

    //TODO IF NAME IS SMALLER DO RESET EVERYTHING, IF NAME IS LONGER ONLY FILTER THOSE ALREADY FILTERED
    displayPackageList.clear();

    currPage = 0;
    selected = 0;
    std::string needle = name;
    std::transform(needle.begin(), needle.end(), needle.begin(), ::toupper);

    for (auto &package: *packageList) {
        std::string hayStack = package->getName();
        std::transform(hayStack.begin(), hayStack.end(), hayStack.begin(), ::toupper);

        if ((strcasecmp(name,"")==0 || hayStack.find(needle) != std::string::npos) && meetsCriteria(package)) {
            displayPackageList.emplace_back(package);
        }
    }

    currentSort = filterView->getCurrentSort();
    currentSortOrder = filterView->getCurrentSortOrder();
    std::sort(displayPackageList.begin(), displayPackageList.end(), &repoPackageList::packageCompare);
    fillPage();
}

void repoPackageList::updateView() {
    if(repo->isUpdating()) {
        active = false;
        return;
    }
    int selectedTemp;
    bool empty;
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
    empty = displayPackageList.empty();
    {
        std::unique_lock<std::mutex> lock(updateMtx);
        for (int i = 0; i < packagesPerPage; i++) {
            std::shared_ptr<package> currPackage = currPackages[i];
            packageRectangle packageRectangle = packageRectangles[i];
            if (selectedTemp != i || keyboardInput->active() || filterView->active()) {
                mainScene->DrawRectangle(0, packageRectangle.y - packageRectangle.height / 2, packageRectangle.width,
                                         rectangleDivisorHeight, textColor);
                mainScene->DrawRectangle(0,
                                         packageRectangle.y + packageRectangle.height / 2 - rectangleDivisorHeight / 2,
                                         packageRectangle.width, rectangleDivisorHeight, textColor);
                if (currPackage != nullptr) {
                    mainScene->DrawText(
                            (char *) std::string(currPackage->getName()).substr(0, PKGLIST_CHARACTER_LIMIT).c_str(),
                            fontMedium, packageRectangle.x, packageRectangle.y,
                            textColor, textColor);
                    printStr = getMainLang()->TYPE;
                    printStr += TypeStr[currPackage->getPackageType()];
                    mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX,
                                        packageRectangle.y - 1 * packageRectangle.height / 8,
                                        textColor, textColor);
                    printStr = getMainLang()->VERSION;
                    printStr += ": ";
                    printStr += currPackage->getVersionStr();
                    printStr += " | ";
                    printStr += currPackage->getTitleID();

                    if (currPackage->getSystemVersion() > 0) {
                        printStr += " | " + getMainLang()->FOR_FW;
                        printStr += currPackage->getSystemVersionStr();
                        printStr += "+";
                    }
                    mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageRectangle.x,
                                        packageRectangle.y + 3 * packageRectangle.height / 8,
                                        textColor, textColor);
                    if (currPackage->isInstalled()) {
                        if (currPackage->updateAvailable()) {
                            printStr = getMainLang()->UPDATE_AVAILABLE;
                            printStr += currPackage->getSFOType();
                            mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX,
                                                packageRectangle.y + 1 * packageRectangle.height / 8,
                                                updateTextColor, updateTextColor);
                        } else {
                            printStr = getMainLang()->INSTALLED;
                            printStr += currPackage->getSFOType();
                            mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX,
                                                packageRectangle.y + 1 * packageRectangle.height / 8,
                                                textColor, textColor);
                        }
                    } else if (strlen(currPackage->getSFOType()) > 1) {
                        printStr = currPackage->getSFOType();
                        mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX,
                                            packageRectangle.y + 1 * packageRectangle.height / 8,
                                            textColor, textColor);
                    }
                    currPackage->getIcon()->Draw(mainScene, repoIconX,
                                                 packageRectangle.y - 3 * packageRectangle.height / 8);
                    printStr = getMainLang()->SIZE;
                    printStr += ": ";
                    printStr += currPackage->getPkgSizeMB();
                    printStr += "MB";
                    mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX,
                                        packageRectangle.y + 3 * packageRectangle.height / 8,
                                        textColor, textColor);
                }
            }
        }
        if (!keyboardInput->active() && !filterView->active()) {
            packageRectangle packageRectangle = packageRectangles[selectedTemp];
            if (!empty) {
                mainScene->DrawRectangle(0, packageRectangle.y - packageRectangle.height / 2, packageRectangle.width,
                                         rectangleDivisorHeight, selectedColor);
                mainScene->DrawRectangle(0,
                                         packageRectangle.y + packageRectangle.height / 2 - rectangleDivisorHeight / 2,
                                         packageRectangle.width, rectangleDivisorHeight, selectedColor);
                std::shared_ptr<package> currPackage = currPackages[selectedTemp];
                mainScene->DrawText(
                        (char *) std::string(currPackage->getName()).substr(0, PKGLIST_CHARACTER_LIMIT).c_str(),
                        fontMedium, packageRectangle.x, packageRectangle.y,
                        selectedColor, selectedColor);
                printStr = getMainLang()->TYPE;
                printStr += TypeStr[currPackage->getPackageType()];
                mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX,
                                    packageRectangle.y - 1 * packageRectangle.height / 8,
                                    selectedColor, selectedColor);
                printStr = getMainLang()->VERSION;
                printStr += ": ";
                printStr += currPackage->getVersionStr();
                printStr += " | ";
                printStr += currPackage->getTitleID();

                if (currPackage->getSystemVersion() > 0) {
                    printStr += " | " + getMainLang()->FOR_FW;;
                    printStr += currPackage->getSystemVersionStr();
                    printStr += "+";
                }
                mainScene->DrawText((char *) printStr.c_str(), fontSmall,
                                    packageRectangle.x, packageRectangle.y + 3 * packageRectangle.height / 8,
                                    selectedColor, selectedColor);
                currPackage->getIcon()->Draw(mainScene, repoIconX,
                                             packageRectangle.y - 3 * packageRectangle.height / 8);
                if (currPackage->isInstalled()) {
                    if (currPackage->updateAvailable()) {
                        printStr = getMainLang()->UPDATE_AVAILABLE;
                        printStr += currPackage->getSFOType();
                        mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX,
                                            packageRectangle.y + 1 * packageRectangle.height / 8,
                                            updateTextColor, updateTextColor);
                    } else {
                        printStr = getMainLang()->INSTALLED;
                        printStr += currPackage->getSFOType();
                        mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX,
                                            packageRectangle.y + 1 * packageRectangle.height / 8,
                                            selectedColor, selectedColor);
                    }
                } else if (strlen(currPackage->getSFOType()) > 1) {
                    printStr = currPackage->getSFOType();
                    mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX,
                                        packageRectangle.y + 1 * packageRectangle.height / 8,
                                        selectedColor, selectedColor);
                }
                printStr = getMainLang()->SIZE;
                printStr += ": ";
                printStr += currPackage->getPkgSizeMB();
                printStr += "MB";
                mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX,
                                    packageRectangle.y + 3 * packageRectangle.height / 8,
                                    selectedColor, selectedColor);
            } else {
                mainScene->DrawRectangle(0, packageRectangle.y - packageRectangle.height / 2, packageRectangle.width,
                                         rectangleDivisorHeight, textColor);
                mainScene->DrawRectangle(0,
                                         packageRectangle.y + packageRectangle.height / 2 - rectangleDivisorHeight / 2,
                                         packageRectangle.width, rectangleDivisorHeight, textColor);
            }
        }
        keyboardInput->updateView();
        filterView->updateView();
    }
}

void repoPackageList::pressX(){
    if(filterView->active())
        filterView->pressX();
    else {
        std::shared_ptr<package> currpkg = currPackages[selected];
        if(currPackages[selected] == nullptr)
            return;
        if(getMainSettings()->shouldInstallDirectlyPS4())
            currpkg->install();
        else
            getDownloadManager()->addDownload(new download(currpkg));

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
    if(filterView->active())
        filterView->disableFilterView();
    else
        active = false;

}
void repoPackageList::setActive() {
    if(repo->hasUpdated())
        filterPackages(std::string(keyboardInput->readText()).c_str());
    active = true;
}
void repoPackageList::pressTriangle(){
    keyboardInput->enableKeyboard();
}
void repoPackageList::pressSquare(){
    if(!filterView->active())
        filterView->enableFilterView();
    else
        filterView->disableFilterView();
}
void repoPackageList::arrowUp(){
    if(filterView->active())
        filterView->arrowUP();
    else {
        std::unique_lock<std::mutex> lock(updateMtx);
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
void repoPackageList::arrowDown(){
    if(filterView->active())
        filterView->arrowDown();
    else {
        std::unique_lock<std::mutex> lock(updateMtx);
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
void repoPackageList::arrowRight() {
    if(filterView->active())
        filterView->nextOption();

}
void repoPackageList::arrowLeft() {
    if(filterView->active())
        filterView->prevOption();
}
repoPackageList::~repoPackageList() {
    std::unique_lock<std::mutex> lock(updateMtx);
    packageList->clear();
    delete packageList;

    displayPackageList.clear();

    delete filterView;
    delete keyboardInput;
}

bool repoPackageList::isActive() {
    return active;
}

void repoPackageList::langChanged() {
        delete keyboardInput;
        keyboardInput = new class keyboardInput(mainScene, fontSmall, viewWidth * KEYBOARD_X_POS, viewHeight / 2, viewWidth * (1 - KEYBOARD_X_POS * 2), viewHeight/2, getMainLang()->SEARCH.c_str(),"");
        delete filterView;
        filterView = new class filterView(mainScene, fontSmall, viewWidth * KEYBOARD_X_POS, 3*viewHeight / 4, viewWidth * (1 - KEYBOARD_X_POS * 2), viewHeight/2);

}

bool repoPackageList::meetsCriteria(std::shared_ptr<package> &sharedPtr) {
    if(!filterView->isEnabled(sharedPtr->getPackageType()))
        return false;
    if(filterView->onlyUpdates())
        return sharedPtr->updateAvailable() && sharedPtr->isInstalled();
    return true;
}

bool repoPackageList::packageCompare(const std::shared_ptr<package> &lhs, const std::shared_ptr<package> &rhs) {
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

