//
// Created by Víctor Jiménez Rugama on 12/24/21.
//
#include "../../_common/notifi.h"

#include "../../include/view/downloadView.h"
#include "../../include/base.h"
#include "../../include/ControllerManager.h"
#include "../../include/utils/logger.h"
#include "../../include/utils/dialog.h"
#include "../../include/file/fileManager.h"
#include "../../include/repository/package.h"
#include "../../include/repository/repoFetcher.h"

#include <sstream>
#include <string>
#include <thread>
#include <ostream>
#include <vector>
#include <iterator>
downloadView* downloadView::downloadManager;
downloadView::downloadView(Scene2D * mainScene, FT_Face fontLarge, FT_Face fontMedium, FT_Face fontSmall, int frameWidth, int frameHeight) : downloadList(),currDownloads(), frameWidth(frameWidth), frameHeight(frameHeight), viewWidth(frameWidth), viewHeight(frameHeight * TOPVIEWSIZE) {
    this->mainScene = mainScene;
    downloadManager = this;
    if(fileExists(DOWNLOADS_PATH))  {
        downloadsYAML = YAML::LoadFile(DOWNLOADS_PATH);
        loadDownloadList(&downloadList,downloadsYAML);
    }

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

    int repoX = static_cast<int>(frameWidth*REPO_X_POS);
    this->repoIconX = static_cast<int>(REPO_ICON_POS*repoX);
    for(int i =0; i < downloadsPerPage; i++)
        downloadRectangles[i] = {repoX, viewHeight + i * rectangleBaseHeight + rectangleBaseHeight / 2, frameWidth, rectangleBaseHeight};
    downloadView::fillPage();

    this->downloadDateX=repoX+ (downloadRectangles[0].width - repoX) * DOWNLOAD_DATE_POS;
    this->packageTypeX=repoX+ (downloadRectangles[0].width - repoX) * PACKAGE_TYPE_POS;

    this->deletedSelected = false;
    this->deleteIconX=repoX+ (downloadRectangles[0].width - repoX) * DELETE_ICON_POS;
    std::string deleteIconPath = DATA_PATH;
    deleteIconPath+="assets/images/";
    this->deleteIcon= new PNG((deleteIconPath+"delete.png").c_str());
    this->deleteIconSelected= new PNG((deleteIconPath+"deleteSelected.png").c_str());

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
}
void downloadView::addDownload(download * newDownload) {
    std::ofstream downloadsFile(DOWNLOADS_PATH, std::ofstream::out | std::ofstream::trunc);
    std::thread(&download::initDownload,std::ref(*newDownload)).detach();
    std::string notification = newDownload->getName();
    notification += "\nAdded to downloads";
    notifi(newDownload->getIconPath(),notification.c_str());
    downloadList.emplace_back(newDownload);
    downloadsYAML[newDownload->getID()];
    downloadsYAML[newDownload->getID()]["date"] = newDownload->getDate();
    downloadsYAML[newDownload->getID()]["path"] = newDownload->getPath();
    downloadsYAML[newDownload->getID()]["url"] = newDownload->getURL();
    downloadsYAML[newDownload->getID()]["name"] = newDownload->getName();
    downloadsYAML[newDownload->getID()]["version"] = newDownload->getVersion();
    downloadsYAML[newDownload->getID()]["repoName"] = newDownload->getRepoName();
    downloadsYAML[newDownload->getID()]["type"] = TypeStr[newDownload->getPackageType()];
    downloadsYAML[newDownload->getID()]["iconPath"] = newDownload->getIconPath();
    downloadsYAML[newDownload->getID()]["finished"] = newDownload->hasFinished();
    downloadsFile << downloadsYAML;
    downloadsFile.flush();
    downloadsFile.close();
    fillPage();
}
void downloadView::setFinished(download * newDownload) {
    downloadsYAML[newDownload->getID()]["finished"] = true;
    std::ofstream downloadsFile(DOWNLOADS_PATH, std::ofstream::out | std::ofstream::trunc);
    downloadsFile << downloadsYAML;
    downloadsFile.flush();
    downloadsFile.close();
}
void downloadView::updateView() {
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
                if(currDownload->hasFailed())
                    printStringStream << "Has Failed";
                else if(!currDownload->stored())
                    printStringStream <<  "Pending Download...";
                else if(currDownload->hasFinished() && !currDownload->isInstalled())
                    printStringStream << "Has finished";
                else if(currDownload->hasFinished() && currDownload->isInstalled())
                    printStringStream << "Is installed";
                else if(downloadRequest->isDownloading()){
                    printStringStream << "Downloaded " << downloadRequest->getDownloadedInMb();
                    printStringStream << "MBs / " << downloadRequest->getTotalSizeInMb() << "MBs";
                }
                currDownload->getIcon()->Draw(mainScene, repoIconX, repoRectangle.y - 3 * repoRectangle.height / 8);
                mainScene->DrawText((char *) std::string(currDownload->getName()).substr(0,DOWNLOAD_NAME_CHARACTER_LIMIT).c_str(), fontMedium, repoRectangle.x, repoRectangle.y,
                                    selectedColor, textColor);
                mainScene->DrawText((char *) printStringStream.str().c_str(), fontSmall, repoRectangle.x, repoRectangle.y + 3 * repoRectangle.height / 8,
                                    selectedColor, textColor);
                printStr = "Type: ";
                printStr+=TypeStr[currDownload->getPackageType()];
                mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX, repoRectangle.y- 1 * repoRectangle.height / 8,
                                    selectedColor, selectedColor);
                printStr = "Source: ";
                printStr+=currDownload->getRepoName();
                mainScene->DrawText((char *) printStr.substr(0,DOWNLOAD_CHARACTER_LIMIT).c_str(), fontSmall, packageTypeX, repoRectangle.y+ 3 * repoRectangle.height / 8,
                                    selectedColor, selectedColor);
                printStr = "Download Date: ";
                printStr+=currDownload->getDate();
                mainScene->DrawText((char *) printStr.c_str(), fontSmall, downloadDateX, repoRectangle.y- 1 * repoRectangle.height / 8,
                                    selectedColor, selectedColor);
                printStr = "Version: ";
                printStr+=currDownload->getVersion();
                mainScene->DrawText((char *) printStr.substr(0,DOWNLOAD_CHARACTER_LIMIT).c_str(), fontSmall, downloadDateX, repoRectangle.y+ 3 * repoRectangle.height / 8,
                                    selectedColor, selectedColor);
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
        mainScene->DrawText((char *) std::string(currDownload->getName()).substr(0,DOWNLOAD_NAME_CHARACTER_LIMIT).c_str(), fontMedium, repoRectangle.x, repoRectangle.y,
                            selectedColor, selectedColor);
        printStr = "Type: ";
        printStr+=TypeStr[currDownload->getPackageType()];
        mainScene->DrawText((char *) printStr.c_str(), fontSmall, packageTypeX, repoRectangle.y- 1 * repoRectangle.height / 8,
                            selectedColor, selectedColor);
        printStr = "Source: ";
        printStr+=currDownload->getRepoName();
        mainScene->DrawText((char *) printStr.substr(0,DOWNLOAD_CHARACTER_LIMIT).c_str(), fontSmall, packageTypeX, repoRectangle.y+ 3 * repoRectangle.height / 8,
                            selectedColor, selectedColor);
        printStr = "Download Date: ";
        printStr+=currDownload->getDate();
        mainScene->DrawText((char *) printStr.c_str(), fontSmall, downloadDateX, repoRectangle.y- 1 * repoRectangle.height / 8,
                            selectedColor, selectedColor);
        printStr = "Version: ";
        printStr+=currDownload->getVersion();
        mainScene->DrawText((char *) printStr.substr(0,DOWNLOAD_CHARACTER_LIMIT).c_str(), fontSmall, downloadDateX, repoRectangle.y+ 3 * repoRectangle.height / 8,
                            selectedColor, selectedColor);

        std::shared_ptr<fileDownloadRequest> downloadRequest = currDownload->getRequest();
        if(currDownload->hasFailed())
            printStringStream << "Has Failed";
        else if(!currDownload->stored())
            printStringStream <<  "Pending Download... (X to retry)";
        else if(currDownload->hasFinished() && !currDownload->isInstalled())
            printStringStream << "Has finished. (X to install)";
        else if(currDownload->hasFinished() && currDownload->isInstalled())
            printStringStream << "Is installed";
        else if(downloadRequest->isDownloading()){
            printStringStream << "Downloaded " << downloadRequest->getDownloadedInMb();
            printStringStream << "MBs / " << downloadRequest->getTotalSizeInMb() << "MBs";
        }
        mainScene->DrawText((char *) printStringStream.str().c_str(), fontSmall, repoRectangle.x,
                            repoRectangle.y + 3 * repoRectangle.height / 8,
                            selectedColor, textColor);
        currDownload->getIcon()->Draw(mainScene, repoIconX, repoRectangle.y - 3 * repoRectangle.height / 8);
            if (deletedSelected)
                deleteIconSelected->Draw(mainScene, deleteIconX, repoRectangle.y - 3 * repoRectangle.height / 8);
            else
                deleteIcon->Draw(mainScene, deleteIconX, repoRectangle.y - 3 * repoRectangle.height / 8);
    }
}

void downloadView::pressX(){
    download * currDownload = currDownloads[selected];
    if(deletedSelected)
        deleteDownload(currDownload->getID());
    else if(!currDownload->stored()){
        std::thread(&download::initDownload,std::ref(*currDownload)).detach();
    } else if (currDownload->stored() && currDownload->getRequest()->hasFinished())
            currDownload->install();
}

int downloadView::deleteDownload(const char * id){
    downloadList.erase(std::remove_if(downloadList.begin(), downloadList.end(), [&id](download* download){bool found = strcasecmp(download->getID(), id) == 0; if(found) download->deleteDownload(); return found;}), downloadList.end());
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
    return 0;
}
void downloadView::pressCircle(){

}
void downloadView::pressTriangle(){

}
void downloadView::pressSquare(){
    deletedSelected = !deletedSelected;
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
        if (!deletedSelected)
            deletedSelected = true;

}
void downloadView::arrowLeft() {
        if (deletedSelected)
            deletedSelected = false;

}
bool downloadView::isActive() {
    return true;
}
downloadView::~downloadView() {
    for(auto repository : downloadList)
        delete repository;
    delete deleteIcon;
    delete deleteIconSelected;
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
