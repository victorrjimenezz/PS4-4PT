//
// Created by Víctor Jiménez Rugama on 12/24/21.
//

#include "../../include/view/downloadView.h"
#include "../../include/base.h"
#include "../../include/ControllerManager.h"
#include "../../include/utils/logger.h"
#include "../../include/utils/notifi.h"
#include "../../include/utils/PNG.h"
#include "../../include/utils/utils.h"
#include "../../include/file/fileManager.h"
#include "../../include/file/download.h"
#include "../../include/file/fileDownloadRequest.h"
#include "../../include/utils/AudioManager.h"
#include "../../include/utils/LANG.h"

#include <string>
#include <thread>
#include <ostream>
#include <vector>
#include <iterator>
//TODO Investigate Crash when removing download
downloadView* downloadView::downloadManager;
downloadView::downloadView(Scene2D * mainScene, FT_Face fontLarge, FT_Face fontMedium, FT_Face fontSmall, int frameWidth, int frameHeight) : downloadList(),currDownloads(), frameWidth(frameWidth), frameHeight(frameHeight), viewWidth(frameWidth), viewHeight(frameHeight * TOPVIEWSIZE) {
    this->mainScene = mainScene;
    downloadManager = this;

    rectangleBaseHeight = ((frameHeight-frameHeight*TABVIEWSIZE-viewHeight) / downloadsPerPage);
    rectangleDivisorHeight = (rectangleBaseHeight*RECTANGLEDIVISORHEIGHT);

    bgColor = {255,255,255};
    selectedColor = {0,0,0};
    textColor = {180, 180, 180};

    this->fontLarge = fontLarge;
    this->fontMedium = fontMedium;
    this->fontSmall = fontSmall;

    child = nullptr;

    currPage = 0;
    selected = 0;

    loadDownloadList();

    int repoX = static_cast<int>(frameWidth*REPO_X_POS);
    this->repoIconX = static_cast<int>(REPO_ICON_POS*repoX);
    for(int i =0; i < downloadsPerPage; i++)
        downloadRectangles[i] = {repoX, viewHeight + i * rectangleBaseHeight + rectangleBaseHeight / 2, frameWidth, rectangleBaseHeight,viewHeight + i * rectangleBaseHeight + rectangleBaseHeight / 2 -1 * rectangleBaseHeight / 16} ;

    this->downloadDateX=repoX+ (downloadRectangles[0].width - repoX) * DOWNLOAD_DATE_POS;
    this->packageTypeX=repoX+ (downloadRectangles[0].width - repoX) * PACKAGE_TYPE_POS;

    option = SelectedOption::REMOVE;
    std::string iconPath;

    this->installIconX=repoX+ (downloadRectangles[0].width - repoX) * INSTALL_ICON_POS;
    iconPath = DATA_PATH;
    iconPath+="assets/images/download/";
    this->installIcon= new PNG((iconPath+"install.png").c_str(),DOWNLOAD_OPTION_ICON_WIDTH,DOWNLOAD_OPTION_ICON_HEIGHT);
    this->installIconSelected= new PNG((iconPath+"installSelected.png").c_str(),DOWNLOAD_OPTION_ICON_WIDTH,DOWNLOAD_OPTION_ICON_HEIGHT);
    this->downloadIcon= new PNG((iconPath+"download.png").c_str(),DOWNLOAD_OPTION_ICON_WIDTH,DOWNLOAD_OPTION_ICON_HEIGHT);
    this->downloadIconSelected= new PNG((iconPath+"downloadSelected.png").c_str(),DOWNLOAD_OPTION_ICON_WIDTH,DOWNLOAD_OPTION_ICON_HEIGHT);

    this->uninstallIconX=repoX+ (downloadRectangles[0].width - repoX) * UNINSTALL_ICON_POS;
    this->uninstallIcon= new PNG((iconPath+"uninstall.png").c_str(),DOWNLOAD_OPTION_ICON_WIDTH,DOWNLOAD_OPTION_ICON_HEIGHT);
    this->uninstallIconSelected= new PNG((iconPath+"uninstallSelected.png").c_str(),DOWNLOAD_OPTION_ICON_WIDTH,DOWNLOAD_OPTION_ICON_HEIGHT);

    this->pauseIcon= new PNG((iconPath+"pause.png").c_str(),DOWNLOAD_OPTION_ICON_WIDTH,DOWNLOAD_OPTION_ICON_HEIGHT);
    this->pauseIconSelected= new PNG((iconPath+"pauseSelected.png").c_str(),DOWNLOAD_OPTION_ICON_WIDTH,DOWNLOAD_OPTION_ICON_HEIGHT);


    this->deleteIconX=repoX+ (downloadRectangles[0].width - repoX) * DOWNLOAD_DELETE_ICON_POS;
    std::string deleteIconPath = DATA_PATH;
    deleteIconPath+="assets/images/";
    this->deleteIcon= new PNG((deleteIconPath+"delete.png").c_str(),DOWNLOAD_OPTION_ICON_WIDTH,DOWNLOAD_OPTION_ICON_HEIGHT);
    this->deleteIconSelected= new PNG((deleteIconPath+"deleteSelected.png").c_str(),DOWNLOAD_OPTION_ICON_WIDTH,DOWNLOAD_OPTION_ICON_HEIGHT);

    //LOAD deleteWav
    std::string audioPath = DATA_PATH;
    audioPath+="assets/audio/delete.wav";
    deleteWav = (drwav_int16 *)AudioManager::loadAudioFile(audioPath.c_str(), &deleteWavCount);

    this->isUpdating = false;
    this->fillPage();
}

void downloadView::fillPage() {
    int j;
    size_t repoListSize = downloadList.size();
    for(int i =0; i < downloadsPerPage; i++){
        j = currPage * downloadsPerPage + i;
        if(repoListSize<=j)
            currDownloads[i] = nullptr;
        else
            currDownloads[i] = downloadList.at(j);
    }
    if(currDownloads[selected] == nullptr)
        return;
    int installed = currDownloads[selected]->isInstalled();
    int finished = currDownloads[selected]->getRequest()->hasFinished();
    if((!installed && option == UNINSTALL )|| (!finished && option == INSTALL))
        option = INSTALL;
}
int downloadView::loadDownloadList(){
    std::vector<std::string> downloadsLocations;
    bool failedInit;
    if(fileExists(DOWNLOADS_PATH))  {
        try {
            downloadsYAML = YAML::LoadFile(DOWNLOADS_PATH);
            for(YAML::const_iterator it=downloadsYAML.begin(); it!=downloadsYAML.end(); ++it){
                if(it->second){
                    if((it->second).IsMap()) {
                        const std::string &key = it->first.as<std::string>();
                        if (strcasecmp(key.c_str(), "name") == 0 || strcasecmp(key.c_str(), "repoURL") == 0)
                            continue;
                        std::string id = key;

                        YAML::Node attributes = it->second;
                        if (!attributes["date"] || !attributes["path"] || !attributes["url"] || !attributes["repoName"] || !attributes["type"])
                            continue;

                        std::string date = attributes["date"].as<std::string>("");
                        std::string path = attributes["path"].as<std::string>("");
                        std::string url = attributes["url"].as<std::string>("");
                        std::string repoName = attributes["repoName"].as<std::string>("");
                        std::string type = attributes["type"].as<std::string>("");

                        auto *dld = new download(id.c_str(), date.c_str(), path.c_str(),&failedInit, url.c_str(),type.c_str(),repoName.c_str());
                        if (failedInit) {
                            delete dld;
                            continue;
                        }
                        if(!path.empty())
                            downloadsLocations.emplace_back(path);
                        downloadList.emplace_back(dld);
                    }
                }
            }
        } catch(const YAML::ParserException& ex) {
            LOG << ex.what();
            removeFile(DOWNLOADS_PATH);
        }
    }

    for(const std::string& file : lsDir(INSTALL_PATH)) {
        std::string filename = file.substr(0,file.find_last_of('.'));
        std::string extension = file.substr(file.size() - 4);
        if(file.find_last_of('.') == std::string::npos || strcasecmp(extension.c_str(),PKG_EXTENSION) != 0)
            continue;
        std::string path = INSTALL_PATH;
        path+= file;
        if (std::find(downloadsLocations.begin(), downloadsLocations.end(), path) != downloadsLocations.end())
            continue;
        auto *dld = new download(filename.c_str(), genDate().c_str(), path.c_str(), &failedInit);
        if (failedInit || !dld->hasFinished()) {
            delete dld;
            removeFile(path.c_str());
            continue;
        }

        downloadsYAML[dld->getID()];
        downloadsYAML[dld->getID()]["date"] = dld->getDate();
        downloadsYAML[dld->getID()]["path"] = dld->getPath();
        downloadsYAML[dld->getID()]["url"] = dld->getURL();
        downloadsYAML[dld->getID()]["repoName"] = dld->getRepoName();
        downloadsYAML[dld->getID()]["type"] = TypeStr[dld->getPackageType()];
        downloadList.emplace_back(dld);
    }

    std::ofstream downloadsFile(DOWNLOADS_PATH, std::ofstream::out | std::ofstream::trunc);
    downloadsFile << downloadsYAML;
    downloadsFile.flush();
    downloadsFile.close();
    return 0;
}

void downloadView::addDownload(download * newDownload) {
    std::ofstream downloadsFile(DOWNLOADS_PATH, std::ofstream::out | std::ofstream::trunc);
    std::thread(&download::initDownload,std::ref(*newDownload)).detach();
    std::string notification = newDownload->getName();
    notification += "\nAdded to downloads";
    notifi(NULL,notification.c_str());
    downloadList.emplace_back(newDownload);
    downloadsYAML[newDownload->getID()];
    downloadsYAML[newDownload->getID()]["date"] = newDownload->getDate();
    downloadsYAML[newDownload->getID()]["path"] = newDownload->getPath();
    downloadsYAML[newDownload->getID()]["url"] = newDownload->getURL();
    downloadsYAML[newDownload->getID()]["repoName"] = newDownload->getRepoName();
    downloadsYAML[newDownload->getID()]["type"] = TypeStr[newDownload->getPackageType()];
    downloadsFile << downloadsYAML;
    downloadsFile.flush();
    downloadsFile.close();
    fillPage();
}
void downloadView::updateView() {
    this->isUpdating = true;
    std::string printStr;
    std::stringstream printStringStream;
    printStringStream.precision(2);
    printStringStream << std::fixed;
    for(int i =0; i < downloadsPerPage; i++){
        download * currDownload = currDownloads[i];
        downloadRectangle repoRectangle = downloadRectangles[i];
        if(selected != i || downloadList.empty()){
            mainScene->DrawRectangle(0, repoRectangle.y-repoRectangle.height/2, repoRectangle.width, rectangleDivisorHeight, textColor);
            mainScene->DrawRectangle(0, repoRectangle.y+repoRectangle.height/2-rectangleDivisorHeight/2, repoRectangle.width, rectangleDivisorHeight, textColor);
            if(currDownload != nullptr){
                printStringStream.str(std::string());
                std::shared_ptr<fileDownloadRequest> downloadRequest = currDownload->getRequest();
                if(currDownload->hasFailed()) {
                    printStringStream << LANG::mainLang->HAS_FAILED;
                    printStringStream << LANG::mainLang->DOWNLOADED << downloadRequest->getDownloadedInMb();
                    printStringStream << "MBs / " << downloadRequest->getTotalSizeInMb() << "MBs";
                } else if(!currDownload->stored())
                    printStringStream <<  LANG::mainLang->PENDING_DOWNLOAD;
                else if(currDownload->hasFinished() && !currDownload->isInstalled()) {
                    printStringStream << LANG::mainLang->HAS_FINISHED;
                    printStringStream << " | " << LANG::mainLang->SIZE << ": " << downloadRequest->getTotalSizeInMb() << "MBs";
                } else if(currDownload->hasFinished() && currDownload->isInstalled())
                    printStringStream << LANG::mainLang->INSTALLED;
                else {
                    printStringStream << LANG::mainLang->DOWNLOADED << downloadRequest->getDownloadedInMb();
                    printStringStream << "MBs / " << downloadRequest->getTotalSizeInMb() << "MBs";
                }
                currDownload->getIcon()->Draw(mainScene, repoIconX, repoRectangle.y - 3 * repoRectangle.height / 8);
                mainScene->DrawText((char *) std::string(currDownload->getName()).substr(0,DOWNLOAD_NAME_CHARACTER_LIMIT).c_str(), fontMedium, repoRectangle.x, repoRectangle.y - 1 * repoRectangle.height/8,
                                    textColor, textColor);
                mainScene->DrawText((char *) currDownload->getTitleID(), fontSmall, repoRectangle.x, repoRectangle.y + 1 * repoRectangle.height/8,
                                    textColor, textColor);
                mainScene->DrawText((char *) printStringStream.str().c_str(), fontSmall, repoRectangle.x, repoRectangle.y + 3 * repoRectangle.height / 8,
                                    textColor, textColor);
                printStr = LANG::mainLang->TYPE;
                printStr+=TypeStr[currDownload->getPackageType()];
                mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX, repoRectangle.y- 1 * repoRectangle.height / 8,
                                    textColor, textColor);
                printStr = LANG::mainLang->SOURCE;
                printStr+=currDownload->getRepoName();
                mainScene->DrawText((char *) printStr.substr(0,DOWNLOAD_CHARACTER_LIMIT).c_str(), fontSmall, packageTypeX, repoRectangle.y+ 3 * repoRectangle.height / 8,
                                    textColor, textColor);
                printStr = LANG::mainLang->DOWNLOAD_DATE;
                printStr+=currDownload->getDate();
                mainScene->DrawText((char *) printStr.c_str(), fontSmall, downloadDateX, repoRectangle.y- 1 * repoRectangle.height / 8,
                                    textColor, textColor);
                printStr = LANG::mainLang->VERSION;
                printStr += ": ";
                printStr+=currDownload->getVersionStr();
                mainScene->DrawText((char *) printStr.substr(0,DOWNLOAD_CHARACTER_LIMIT).c_str(), fontSmall, downloadDateX, repoRectangle.y+ 3 * repoRectangle.height / 8,
                                    textColor, textColor);
            }
        }
    }
    if(!downloadList.empty()) {
        printStringStream.str(std::string());
        download *currDownload = currDownloads[selected];
        downloadRectangle repoRectangle = downloadRectangles[selected];
        mainScene->DrawRectangle(0, repoRectangle.y - repoRectangle.height / 2, repoRectangle.width,
                                 rectangleDivisorHeight, selectedColor);
        mainScene->DrawRectangle(0, repoRectangle.y + repoRectangle.height / 2 - rectangleDivisorHeight / 2,
                                 repoRectangle.width, rectangleDivisorHeight, selectedColor);
        mainScene->DrawText((char *) std::string(currDownload->getName()).substr(0,DOWNLOAD_NAME_CHARACTER_LIMIT).c_str(), fontMedium, repoRectangle.x, repoRectangle.y - 1 * repoRectangle.height/8,
                            selectedColor, selectedColor);
        mainScene->DrawText((char *) currDownload->getTitleID(), fontSmall, repoRectangle.x, repoRectangle.y + 1 * repoRectangle.height/8,
                            selectedColor, selectedColor);
        printStr = LANG::mainLang->TYPE;
        printStr+=TypeStr[currDownload->getPackageType()];
        mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX, repoRectangle.y- 1 * repoRectangle.height / 8,
                            selectedColor, selectedColor);
        printStr = LANG::mainLang->SOURCE;
        printStr+=currDownload->getRepoName();
        mainScene->DrawText((char *) printStr.substr(0,DOWNLOAD_CHARACTER_LIMIT).c_str(), fontSmall, packageTypeX, repoRectangle.y+ 3 * repoRectangle.height / 8,
                            selectedColor, selectedColor);
        printStr = LANG::mainLang->DOWNLOAD_DATE;
        printStr+=currDownload->getDate();
        mainScene->DrawText((char *) printStr.c_str(), fontSmall, downloadDateX, repoRectangle.y- 1 * repoRectangle.height / 8,
                            selectedColor, selectedColor);
        printStr = LANG::mainLang->VERSION;
        printStr += ": ";
        printStr+=currDownload->getVersionStr();
        mainScene->DrawText((char *) printStr.substr(0,DOWNLOAD_CHARACTER_LIMIT).c_str(), fontSmall, downloadDateX, repoRectangle.y+ 3 * repoRectangle.height / 8,
                            selectedColor, selectedColor);

        std::shared_ptr<fileDownloadRequest> downloadRequest = currDownload->getRequest();
        bool installed = currDownload->isInstalled();
        bool stored = currDownload->stored();
        bool finished = currDownload->hasFinished();
        bool downloading = downloadRequest->isDownloading();
        if(currDownload->hasFailed()) {
            printStringStream << LANG::mainLang->HAS_FAILED;
            printStringStream << LANG::mainLang->DOWNLOADED << downloadRequest->getDownloadedInMb();
            printStringStream << "MBs / " << downloadRequest->getTotalSizeInMb() << "MBs";
        }else if(!stored)
            printStringStream <<  LANG::mainLang->PENDING_DOWNLOAD;
        else if(finished && !installed) {
            printStringStream << LANG::mainLang->HAS_FINISHED;
            printStringStream << " | " << LANG::mainLang->SIZE << ": " << downloadRequest->getTotalSizeInMb() << "MBs";
        } else if(finished && installed)
            printStringStream << LANG::mainLang->INSTALLED;
        else {
            printStringStream << LANG::mainLang->DOWNLOADED << downloadRequest->getDownloadedInMb();
            printStringStream << "MBs / " << downloadRequest->getTotalSizeInMb() << "MBs";
        }
        mainScene->DrawText((char *) printStringStream.str().c_str(), fontSmall, repoRectangle.x,
                            repoRectangle.y + 3 * repoRectangle.height / 8,
                            selectedColor, selectedColor);
        currDownload->getIcon()->Draw(mainScene, repoIconX, repoRectangle.y - 3 * repoRectangle.height / 8);


        switch(option){
            case INSTALL:
                if(finished)
                    installIconSelected->Draw(mainScene, installIconX, repoRectangle.iconPosY);
                else if(downloading)
                    pauseIconSelected->Draw(mainScene, installIconX, repoRectangle.iconPosY);
                else
                    downloadIconSelected->Draw(mainScene, installIconX, repoRectangle.iconPosY);
                if(installed)
                    uninstallIcon->Draw(mainScene, uninstallIconX, repoRectangle.iconPosY);
                deleteIcon->Draw(mainScene, deleteIconX, repoRectangle.iconPosY);
                break;
            case UNINSTALL:
                if(finished)
                    installIcon->Draw(mainScene, installIconX, repoRectangle.iconPosY);
                else if(downloading)
                    pauseIcon->Draw(mainScene, installIconX, repoRectangle.iconPosY);
                else
                    downloadIcon->Draw(mainScene, installIconX, repoRectangle.iconPosY);
                uninstallIconSelected->Draw(mainScene, uninstallIconX, repoRectangle.iconPosY);
                deleteIcon->Draw(mainScene, deleteIconX, repoRectangle.iconPosY);
                break;
            case REMOVE:
            default:
                if(finished)
                    installIcon->Draw(mainScene, installIconX, repoRectangle.iconPosY);
                else if(downloading)
                    pauseIcon->Draw(mainScene, installIconX, repoRectangle.iconPosY);
                else
                    downloadIcon->Draw(mainScene, installIconX, repoRectangle.iconPosY);
                if(installed)
                    uninstallIcon->Draw(mainScene, uninstallIconX, repoRectangle.iconPosY);
                deleteIconSelected->Draw(mainScene, deleteIconX, repoRectangle.iconPosY);
                break;
        }

    }
    this->isUpdating = false;
}

void downloadView::pressX(){
    download * currDownload = currDownloads[selected];
    if(currDownload == nullptr)
        return;

    switch(option) {
        case INSTALL:
            if(currDownload->getRequest()->isDownloading())
                currDownload->getRequest()->pauseDownload();
            else if(currDownload->hasFinished())
                currDownload->install();
            else
                std::thread(&download::initDownload,std::ref(*currDownload)).detach();
            break;
        case UNINSTALL:
            option = INSTALL;
            currDownload->unInstall();
            break;
        case REMOVE:
        default:
            while(isUpdating)
                continue;
            deleteDownload(currDownload);
            break;
    }
}

int downloadView::deleteDownload(download * dld){
    std::string id = dld->getID();
    downloadList.erase(std::remove_if(downloadList.begin(), downloadList.end(), [&id](download* download){bool found = strcasecmp(download->getID(), id.c_str()) == 0; if(found) download->deleteDownload(); return found;}), downloadList.end());
    
    downloadsYAML.remove(id);

    std::ofstream downloadsFile(DOWNLOADS_PATH, std::ofstream::out | std::ofstream::trunc);
    downloadsFile << downloadsYAML;
    downloadsFile.flush();
    downloadsFile.close();
    if(!(selected==0 && currPage==0)) {
        selected--;
        if(selected < 0) {
            selected = downloadsPerPage - 1;
            currPage--;
        }
    }
    fillPage();
    AudioManager::mainAudioManager->playAudio(deleteWav,deleteWavCount);
    return 0;
}
void downloadView::pressCircle(){

}
void downloadView::pressTriangle(){

}
void downloadView::pressSquare(){
    option = REMOVE;
}
void downloadView::arrowUp(){
        selected--;
        if (selected + downloadsPerPage * currPage < 0) {
            selected = 0;
        } else if (selected < 0) {
            selected = downloadsPerPage - 1;
            currPage--;
            fillPage();
        }
}
void downloadView::arrowDown(){
        selected++;
        int repoSize = (int) downloadList.size();
        if (selected + downloadsPerPage * currPage < repoSize) {
            if (selected >= downloadsPerPage) {
                selected = 0;
                currPage++;
                fillPage();
            }
        } else {
            selected--;
        }
}
void downloadView::arrowRight() {
    if(currDownloads[selected] == nullptr)
        return;
    int installed = currDownloads[selected]->isInstalled();
    switch(option) {
        case INSTALL:
            if(installed)
                option = UNINSTALL;
            else
                option = REMOVE;
            break;
        case UNINSTALL:
            option = REMOVE;
            break;
        case REMOVE:
            break;
    }

}
void downloadView::arrowLeft() {
    if(currDownloads[selected] == nullptr)
        return;
    int installed = currDownloads[selected]->isInstalled();
    //int downloading = currDownloads[selected]->getRequest()->isDownloading();

    switch(option) {
        case REMOVE:
            if(installed)
                option = UNINSTALL;
            else
                option = INSTALL;
            break;
        case UNINSTALL:
            option = INSTALL;
            break;
        case INSTALL:
            break;
    }

}
bool downloadView::isActive() {
    return true;
}
downloadView::~downloadView() {
    for(auto repository : downloadList)
        delete repository;
    delete installIcon;
    delete installIconSelected;
    delete uninstallIcon;
    delete uninstallIconSelected;
    delete downloadIcon;
    delete downloadIconSelected;
    delete deleteIcon;
    delete deleteIconSelected;
    delete pauseIcon;
    delete pauseIconSelected;
    if(deleteWav!= nullptr)
        free(deleteWav);
}
void downloadView::deleteChild() {
    delete child;
    child = nullptr;
}
subView *downloadView::getParent() {
    return nullptr;
}

subView *downloadView::getChild() {
    return child;
}
