//
// Created by Víctor Jiménez Rugama on 12/24/21.
//

#include "../../include/view/repositoryView.h"
#include "../../include/view/keyboardInput.h"
#include "../../include/view/repoPackageList.h"
#include "../../include/view/packageSearch.h"
#include "../../include/repository/repository.h"
#include "../../include/utils/dialog.h"
#include "../../include/utils/PNG.h"
#include "../../include/utils/AnimatedPNG.h"
#include "../../include/utils/notifi.h"
#include "../../include/file/fileDownloadRequest.h"
#include "../../include/utils/AudioManager.h"
#include "../../include/utils/logger.h"
#include "../../include/file/fileManager.h"
#include "../../include/utils/LANG.h"

#include <string>
#include <vector>
#include <iterator>
#include <thread>
#include <yaml-cpp/yaml.h>

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

    keyboardInput = new class keyboardInput(mainScene, fontSmall, viewWidth * KEYBOARD_X_POS, viewHeight / 2, frameWidth * (1 - KEYBOARD_X_POS * 2), viewHeight, LANG::mainLang->ADD_REPO.c_str(),"https://",isOnKeyboard);
    child = nullptr;

    currPage = 0;
    selected = 0;
    repositoryList = std::shared_ptr<std::vector<repository*>>(new std::vector<repository*>);
    if(isFirstRun){
        repository * repo = repository::fetchRepo(MAIN_URL);
        if(repo != nullptr)
            addRepository(repo);

    } else {
        loadSavedRepos();
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

    //LOAD deleteWav
    std::string audioPath = DATA_PATH;
    audioPath+="assets/audio/delete.wav";
    deleteWav = (drwav_int16 *)AudioManager::loadAudioFile(audioPath.c_str(), &deleteWavCount);


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
    std::string printStr;
    int totalPages = ceil((double)repositoryList->size()/reposPerPage);
    if(totalPages ==0) totalPages++;
    if(currPage+1<10)
        printStr+="0";
    printStr+= std::to_string(currPage+1) + "/";
    if(totalPages<10)
        printStr+="0";
    printStr+= std::to_string(totalPages);
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
                            selectedColor, selectedColor);
        currRepo->getIcon()->Draw(mainScene,repoIconX,repoRectangle.y-3*repoRectangle.height/8);
            switch (selectedOption) {
                case OPEN:
                    openIconSelected->Draw(mainScene,openIconX,repoRectangle.y-3*repoRectangle.height/8);
                    if(currRepo->isUpdating())
                        updateIcon->Play(updateIconX,repoRectangle.y-3*repoRectangle.height/8);
                    else
                        updateIcon->Draw(updateIconX,repoRectangle.y-3*repoRectangle.height/8);
                    deleteIcon->Draw(mainScene,deleteIconX,repoRectangle.y-3*repoRectangle.height/8);
                    break;
                case UPDATE:
                    openIcon->Draw(mainScene,openIconX,repoRectangle.y-3*repoRectangle.height/8);
                    if(currRepo->isUpdating())
                        updateIcon->Play(updateIconX, repoRectangle.y - 3 * repoRectangle.height / 8);
                    else
                        updateIconSelected->Draw(mainScene,updateIconX,repoRectangle.y-3*repoRectangle.height/8);
                    deleteIcon->Draw(mainScene,deleteIconX,repoRectangle.y-3*repoRectangle.height/8);
                    break;
                case DELETE:
                    openIcon->Draw(mainScene,openIconX,repoRectangle.y-3*repoRectangle.height/8);
                    if(currRepo->isUpdating())
                        updateIcon->Play(updateIconX,repoRectangle.y-3*repoRectangle.height/8);
                    else
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
    if(!fileDownloadRequest::verifyURL(repoURL)) {
        popDialog((LANG::mainLang->INVALID_URL+repoURLTEMP).c_str());
        return;
    }

    for(repository *repository : *repositoryList){
        if(strcasecmp(repository->getRepoURL(),repoURL) == 0) {
            popDialog(LANG::mainLang->REPO_ALREADY_LOADED.c_str());
            return;
        }
    }
    repository *repo = repository::fetchRepo(repoURL);
    if(repo != nullptr) {
        addRepository(repo);
        fillPage();
    } else {
        std::string errmessage = LANG::mainLang->NO_REPO_FOUND_AT;
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
                if(!currRepo->isUpdating()) {
                    repoPackageListView->setActive();
                    child = repoPackageListView;
                }
                break;
            case UPDATE:
                if(!currRepo->isUpdating())
                    std::thread([&capture0 = *currRepo, updateIcon = std::ref(updateIcon)] { capture0.updateRepository(); }).detach();
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
    packageSearch::mainPackageSearch->updatePackages();
    AudioManager::mainAudioManager->playAudio(deleteWav,deleteWavCount);
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

int repositoryView::loadSavedRepos() {
        std::string originalRepoIcon = DATA_PATH;
        originalRepoIcon+="assets/images/repository/repoDefaultIcon.png";
        std::string repoPath = STORED_PATH;
        repoPath+=REPO_PATH;

        YAML::Node repoYAML;

        std::string loadingRepoFolder;
        std::string loadingRepoPath;
        std::string loadingRepoPathTEMP;
        std::string repoURL;
        for(const std::string& file : lsDir(repoPath.c_str())) {
            loadingRepoFolder=repoPath+file+'/';
            loadingRepoPath = loadingRepoFolder;
            loadingRepoPath+="repo.yml";
            loadingRepoPathTEMP = loadingRepoFolder;
            loadingRepoPathTEMP+="repoTEMP.yml";
            if(fileExists(loadingRepoPath.c_str())){
                removeFile(loadingRepoPathTEMP.c_str());
                moveFile(loadingRepoPath.c_str(),loadingRepoPathTEMP.c_str());
                try {
                    repoYAML = YAML::LoadFile(loadingRepoPathTEMP);
                } catch(const YAML::ParserException& ex) {
                    LOG << ex.what();
                    if(folderExists(loadingRepoFolder.c_str()))
                        removeFile(loadingRepoFolder.c_str());
                    continue;
                }
                if(repoYAML["repoURL"]) {
                    repoURL = repoYAML["repoURL"].as<std::string>();
                    std::string repoYML = repoURL+"repo.yml";
                    fileDownloadRequest repoUpdateRequest(repoYML.c_str(),loadingRepoPath.c_str());
                    if(repoUpdateRequest.initDownload() < 0){
                        LOG << "Could not download " << repoYML << " to "<<loadingRepoPath;
                        LOG << "ERROR WHEN UPDATING REPO " << repoURL;
                        removeFile(loadingRepoPath.c_str());
                        moveFile(loadingRepoPathTEMP.c_str(),loadingRepoPath.c_str());

                        std::string repoName = repoYAML["name"].as<std::string>();
                        std::string localIconDir = loadingRepoFolder+"icon.png";
                        if(!fileExists(localIconDir.c_str()))
                            copyFile(originalRepoIcon.c_str(),localIconDir.c_str());
                        addRepository(new repository(file.c_str(), repoName.c_str(), repoURL.c_str(), loadingRepoFolder.c_str(), localIconDir.c_str()));
                    } else {
                        try {
                            repoYAML = YAML::LoadFile(loadingRepoPath);
                        } catch(const YAML::ParserException& ex) {
                            LOG << ex.what();
                            if(folderExists(loadingRepoFolder.c_str()))
                                removeFile(loadingRepoFolder.c_str());
                            continue;
                        }

                        repoYAML["repoURL"] = repoURL;
                        std::ofstream fout(loadingRepoPath);
                        fout << repoYAML;
                        fout.close();
                        removeFile(loadingRepoPathTEMP.c_str());
                        std::string iconDefaultPath = DATA_PATH;
                        iconDefaultPath+="assets/images/repository/repoDefaultIcon.png";
                        const char * iconDefaultPathChar = iconDefaultPath.c_str();

                        std::string repoIconPath;
                        if(repoYAML["iconPath"]) {
                            repoIconPath = repoYAML["iconPath"].as<std::string>();
                            LOG << "iconPath: " << repoIconPath.c_str();
                        }
                        std::string localDownloadPath;
                        std::string downloadURL = repoURL;
                        int ret = -1;
                        if(!repoIconPath.empty()){
                            localDownloadPath = loadingRepoFolder+"icon"+repoIconPath.substr(repoIconPath.find_last_of('.'));
                            if(fileExists(localDownloadPath.c_str()))
                                removeFile(localDownloadPath.c_str());
                            downloadURL += repoIconPath;
                            fileDownloadRequest iconDownloadRequest(downloadURL.c_str(),localDownloadPath.c_str());
                            ret = iconDownloadRequest.initDownload();
                        }

                        if(ret < 0) {

                            LOG << "Error when downloading icon from " << downloadURL<< " to "<< localDownloadPath;
                            localDownloadPath = loadingRepoFolder + "icon.png";
                            if(fileExists(localDownloadPath.c_str()))
                                removeFile(localDownloadPath.c_str());
                            copyFile(iconDefaultPathChar, localDownloadPath.c_str());
                        }

                        std::string repoName = repoYAML["name"].as<std::string>();
                        addRepository(new repository(file.c_str(), repoName.c_str(), repoURL.c_str(), loadingRepoFolder.c_str(), localDownloadPath.c_str()));
                    }
                } else {
                    LOG << "RepoURL NOT found. Removing Directory" << loadingRepoFolder;
                    removeDir(loadingRepoFolder.c_str());
                }
            } else {
                LOG << "repo.yml NOT found. Removing Directory" << loadingRepoFolder;
                removeDir(loadingRepoFolder.c_str());
            }
        }
        return 0;
}
void repositoryView::langChanged() {
    delete keyboardInput;
    keyboardInput = new class keyboardInput(mainScene, fontSmall, viewWidth * KEYBOARD_X_POS, viewHeight / 2, viewWidth * (1 - KEYBOARD_X_POS * 2), viewHeight, LANG::mainLang->ADD_REPO.c_str(),"https://",isOnKeyboard);
}