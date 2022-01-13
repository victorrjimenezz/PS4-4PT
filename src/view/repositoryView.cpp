//
// Created by Víctor Jiménez Rugama on 12/24/21.
//
#include "../../_common/notifi.h"

#include "../../include/view/repositoryView.h"
#include "../../include/repository/repoFetcher.h"
#include "../../include/utils/dialog.h"

#include <string>
#include <vector>
#include <iterator>
#include <thread>

repositoryView * repositoryView::mainRepositoryView;
repositoryView::repositoryView(Scene2D * mainScene, FT_Face fontLarge, FT_Face fontMedium, FT_Face fontSmall, int frameWidth, int frameHeight, bool isFirstRun) : currRepos(),frameWidth(frameWidth),frameHeight(frameHeight), viewWidth(frameWidth), viewHeight(frameHeight*TOPVIEWSIZE) {
    this->mainScene = mainScene;
    mainRepositoryView = this;

    rectangleBaseHeight = ((frameHeight-frameHeight*TABVIEWSIZE-viewHeight)/reposPerPage);
    rectangleDivisorHeight = (rectangleBaseHeight*RECTANGLEDIVISORHEIGHT);

    bgColor = {255,255,255};
    selectedColor = {0,0,0};
    textColor = {180, 180, 180};

    this->isOnKeyboard = false;
    this->fontLarge = fontLarge;
    this->fontMedium = fontMedium;
    this->fontSmall = fontSmall;

    keyboardInput = new class keyboardInput(mainScene, fontSmall, viewWidth * KEYBOARD_X_POS, viewHeight / 2, frameWidth * (1 - KEYBOARD_X_POS * 2), viewHeight, "ADD REPO","http://",isOnKeyboard);
    child = nullptr;

    currPage = 0;
    selected = 0;
    repositoryList = std::shared_ptr<std::vector<repository*>>(new std::vector<repository*>);
    if(isFirstRun){
        repository * repo = fetchRepo("http://4pt-project.com/");
        if(repo != nullptr)
            addRepository(repo);

    } else {
        loadSavedRepos(this);
    }

    int repoX = static_cast<int>(frameWidth*REPO_X_POS);
    this->repoIconX = static_cast<int>(REPO_ICON_POS*repoX);
    for(int i =0; i< reposPerPage; i++)
        repoRectangles[i] = {repoX,viewHeight+i * rectangleBaseHeight+rectangleBaseHeight/2,frameWidth,rectangleBaseHeight};
    repositoryView::fillPage();

    selectedOption = OPEN;
    std::string iconPath = DATA_PATH;
    iconPath+="assets/images/";

    this->openIconX=repoX+(repoRectangles[0].width-repoX)*OPEN_ICON_POS;
    this->openIcon= new PNG((iconPath+"open.png").c_str());
    this->openIconSelected= new PNG((iconPath+"openSelected.png").c_str());


    this->updateIconX=repoX+(repoRectangles[0].width-repoX)*UPDATE_ICON_POS;
    this->updateIcon = new AnimatedPNG((iconPath+"/loadingGIF/").c_str(), mainScene, ICON_DEFAULT_WIDTH, ICON_DEFAULT_HEIGHT);
    this->updateIconSelected= new PNG((iconPath+"updateSelected.png").c_str());

    this->deleteIconX=repoX+(repoRectangles[0].width-repoX)*DELETE_ICON_POS;
    this->deleteIcon= new PNG((iconPath+"delete.png").c_str());
    this->deleteIconSelected= new PNG((iconPath+"deleteSelected.png").c_str());




}

void repositoryView::fillPage() {
    int j;
    size_t repoListSize = repositoryList->size();
    for(int i =0; i< reposPerPage; i++){
        j = currPage*reposPerPage+i;
        if(repoListSize<=j)
            currRepos[i] = nullptr;
        else
            currRepos[i] = repositoryList->at(j);
    }
}

void repositoryView::updateRepositories() {
    for(auto & repository : *repositoryList)
        repository->updateRepository();
}

void repositoryView::updateView() {
    for(int i =0; i< reposPerPage; i++){
        repository * currRepo = currRepos[i];
        repoRectangle repoRectangle = repoRectangles[i];
        if(selected != i || isOnKeyboard){
            mainScene->DrawRectangle(0, repoRectangle.y-repoRectangle.height/2, repoRectangle.width, rectangleDivisorHeight, textColor);
            mainScene->DrawRectangle(0, repoRectangle.y+repoRectangle.height/2-rectangleDivisorHeight/2, repoRectangle.width, rectangleDivisorHeight, textColor);
            if(currRepo != nullptr){
                currRepo->getIcon()->Draw(mainScene,repoIconX,repoRectangle.y-3*repoRectangle.height/8);
                mainScene->DrawText((char *) currRepo->getName(), fontMedium, repoRectangle.x, repoRectangle.y,
                                    selectedColor, textColor);
                mainScene->DrawText((char *) currRepo->getRepoURL(), fontSmall, repoRectangle.x, repoRectangle.y+3*repoRectangle.height/8,
                                    selectedColor, textColor);
            }
        }
    }
    if(!isOnKeyboard && !repositoryList->empty()) {
        repository * currRepo = currRepos[selected];
        repoRectangle repoRectangle = repoRectangles[selected];
        mainScene->DrawRectangle(0, repoRectangle.y - repoRectangle.height / 2, repoRectangle.width,
                                 rectangleDivisorHeight, selectedColor);
        mainScene->DrawRectangle(0, repoRectangle.y + repoRectangle.height / 2 - rectangleDivisorHeight / 2,
                                 repoRectangle.width, rectangleDivisorHeight, selectedColor);
        mainScene->DrawText((char *) currRepo->getName(), fontMedium, repoRectangle.x, repoRectangle.y,
                            selectedColor, selectedColor);
        mainScene->DrawText((char *) currRepo->getRepoURL(), fontSmall, repoRectangle.x, repoRectangle.y+3*repoRectangle.height/8,
                            selectedColor, textColor);
        currRepo->getIcon()->Draw(mainScene,repoIconX,repoRectangle.y-3*repoRectangle.height/8);
            switch (selectedOption) {
                case OPEN:
                    openIconSelected->Draw(mainScene,openIconX,repoRectangle.y-3*repoRectangle.height/8);
                    updateIcon->Draw(updateIconX,repoRectangle.y-3*repoRectangle.height/8);
                    deleteIcon->Draw(mainScene,deleteIconX,repoRectangle.y-3*repoRectangle.height/8);
                    break;
                case UPDATE:
                    openIcon->Draw(mainScene,openIconX,repoRectangle.y-3*repoRectangle.height/8);
                    if(currRepo->isUpdating())
                        updateIcon->Draw(updateIconX, repoRectangle.y - 3 * repoRectangle.height / 8);
                    else
                        updateIconSelected->Draw(mainScene,updateIconX,repoRectangle.y-3*repoRectangle.height/8);
                    deleteIcon->Draw(mainScene,deleteIconX,repoRectangle.y-3*repoRectangle.height/8);
                    break;
                case DELETE:
                    openIcon->Draw(mainScene,openIconX,repoRectangle.y-3*repoRectangle.height/8);
                    updateIcon->Draw(updateIconX,repoRectangle.y-3*repoRectangle.height/8);
                    deleteIconSelected->Draw(mainScene,deleteIconX,repoRectangle.y-3*repoRectangle.height/8);
                    break;
            }

    }
    keyboardInput->updateView();
    if(keyboardInput->hasEntered())
        hasEntered();
}

void repositoryView::hasEntered(){
    std::string repoURLTEMP = keyboardInput->readText();
    if(repoURLTEMP.back() != '/')
        repoURLTEMP+='/';
    const char * repoURL = repoURLTEMP.c_str();
    if(strcasecmp(repoURLTEMP.substr(0,8).c_str(),"https://") == 0) {
        popDialog("SSL (HTTPS) NOT SUPPORTED YET");
        return;
    } else if(strcasecmp(repoURLTEMP.substr(0,7).c_str(),"http://") != 0) {
        popDialog("INVALID URL");
        return;
    }
    for(repository *repository : *repositoryList){
        if(strcasecmp(repository->getRepoURL(),repoURL) == 0) {
            popDialog("Repository already loaded!");
            return;
        }
    }
    repository *repo = fetchRepo(repoURL);
    if(repo != nullptr) {
        addRepository(repo);
        fillPage();
    } else {
        std::string errmessage = "Repo not found at: ";
        errmessage+=repoURL;
        popDialog(errmessage.c_str());
    }
}

void repositoryView::pressX(){
    if(isOnKeyboard)
        keyboardInput->pressKey();
    else {
        repository *currRepo = currRepos[selected];
        if(currRepo == nullptr)
            return;
        repoPackageList * repoPackageListView = repoPackageViewList.at(selected+currPage*reposPerPage);
        switch (selectedOption) {
            case OPEN:
                repoPackageListView->setActive();
                child = repoPackageListView;
                break;
            case UPDATE:
                std::thread([&capture0 = *currRepo, updateIcon = std::ref(updateIcon)] { capture0.updateRepository(updateIcon); }).detach();
                break;
            case DELETE:
                deleteRepo(currRepo->getID());
                break;
        }
    }
}

int repositoryView::deleteRepo(const char * id){
    repoPackageViewList.erase(repoPackageViewList.begin() + selected+currPage*reposPerPage);
    repositoryList->erase(std::remove_if(repositoryList->begin(), repositoryList->end(), [&id](repository* repo){bool found = strcasecmp(repo->getID(), id) == 0; if(found) repo->deleteRepository(); return found;}), repositoryList->end());

    if(!(selected==0 && currPage==0)) {
        selected--;
        if(selected < 0) {
            selected = reposPerPage-1;
            currPage--;
        }
    }
    fillPage();
    return 0;
}
void repositoryView::pressCircle(){
    if(isOnKeyboard) {
        isOnKeyboard = false;
        keyboardInput->unSelectKeyboard();
    }
}
void repositoryView::pressTriangle(){
    isOnKeyboard = !isOnKeyboard;
    if(isOnKeyboard)
        keyboardInput->selectKeyboard();
    else
        keyboardInput->unSelectKeyboard();
}
void repositoryView::pressSquare(){
    if(isOnKeyboard)
        keyboardInput->deleteChar();
}
void repositoryView::arrowUp(){
    if(!isOnKeyboard) {
        selected--;
        if (selected + reposPerPage * currPage < 0) {
            selected = 0;
        } else if (selected < 0) {
            selected = reposPerPage - 1;
            currPage--;
            fillPage();
        }
    } else
        keyboardInput->setUpperRow();
}
void repositoryView::arrowDown(){
    if(!isOnKeyboard) {
        selected++;
        int repoSize = (int) repositoryList->size();
        if (selected + reposPerPage * currPage < repoSize) {
            if (selected >= reposPerPage) {
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
void repositoryView::arrowRight() {
    if(!isOnKeyboard) {
        switch(selectedOption){
            case OPEN:
                selectedOption = UPDATE;
                break;
            case UPDATE:
                selectedOption = DELETE;
                break;
            case DELETE:
            default:
                break;
        }
    } else
        keyboardInput->nextKey();

}
void repositoryView::arrowLeft() {
    if(!isOnKeyboard) {
        switch(selectedOption){
            case OPEN:
                break;
            case UPDATE:
                selectedOption = OPEN;
                break;
            case DELETE:
                selectedOption = UPDATE;
            default:
                break;
        }
    } else
        keyboardInput->previousKey();

}
bool repositoryView::isActive() {
    return true;
}
repositoryView::~repositoryView() {
    for(auto repository : *repositoryList)
        delete repository;
    for(auto repoPackageView : repoPackageViewList)
        delete repoPackageView;

    repositoryList.reset();
    delete child;
    delete keyboardInput;
    delete deleteIcon;
    delete deleteIconSelected;
}
void repositoryView::deleteChild() {
    child = nullptr;
}
subView *repositoryView::getParent() {
    return nullptr;
}

subView *repositoryView::getChild() {
    return child;
}

std::shared_ptr<std::vector<repository *>> repositoryView::getRepositoryList() {
    return repositoryList;
}

void repositoryView::addRepository(repository * repository) {
    repositoryList->emplace_back(repository);
    repoPackageViewList.emplace_back(new repoPackageList(mainScene, fontLarge, fontMedium, fontSmall, frameWidth, frameHeight,
                                                         repository, this));
}
