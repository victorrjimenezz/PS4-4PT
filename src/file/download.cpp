//
// Created by Víctor Jiménez Rugama on 1/2/22.
//

#include "../../include/file/download.h"
#include "../../include/file/fileDownloadRequest.h"
#include "../../include/file/fileManager.h"
#include "../../include/utils/notifi.h"
#include "../../include/utils/utils.h"
#include "../../include/repository/PKGInfo.h"
#include "../../include/utils/logger.h"
#include "../../include/utils/LANG.h"
#include "../../include/utils/settings.h"


download::download(const std::shared_ptr<package>& pkg){
    this->id = genRandom(10);
    this->date = genDate();
    std::string localDownloadPath = DOWNLOAD_PATH;
    localDownloadPath+= id+".pkg";
    this->path=localDownloadPath;
    this->pkg = std::shared_ptr<package>(new package(pkg.get()));
    this->finished = false;
    this->failed = false;

    this->downloadRequest = std::shared_ptr<fileDownloadRequest>(new fileDownloadRequest(pkg->getURL(),path.c_str()));
}

download::download(const char * id, const char * date, const char * localPath, bool * failedInit, const char * url, const char * type,const char * repoName) {
    uint64_t downloadedFileSize = getFileSize(localPath);
    bool urlValid = strcasecmp(url,"");
    if(urlValid)
        this->pkg = std::shared_ptr<package>(new package(url,false,failedInit,type, repoName));
    if(*failedInit || !urlValid) {
        pkg.reset();
        this->pkg = std::shared_ptr<package>(new package(localPath,true,failedInit,type, repoName));
        if(*failedInit){
            pkg.reset();
            LOG << "Could not load download from "<< url;
            return;
        }
    }

    this->id = id;
    this->date = date;
    this->path=localPath;
    uint64_t totalFileSize = pkg->getPkgSize();
    this->finished = downloadedFileSize == totalFileSize;

    this->failed = false;
    this->downloadRequest = std::shared_ptr<fileDownloadRequest>(new fileDownloadRequest(url,path.c_str(),downloadedFileSize, totalFileSize));
}

download::download(const char *url, bool * failedInit) {

    this->pkg = std::shared_ptr<package>(new package(url,false,failedInit));
    if(*failedInit && settings::getMainSettings()->isFailedDownloadingNotification()) {
        std::string msg(LANG::mainLang->FAILED_TO_DOWNLOAD_PKG_FROM + ":\n");
        msg += url;
        notifi(NULL,msg.c_str());
        return;
    }

    this->id = genRandom(10);
    this->date = genDate();

    std::string localDownloadPath = DOWNLOAD_PATH;
    localDownloadPath+= id+".pkg";
    this->path=localDownloadPath;

    this->finished = false;
    this->failed = false;
    this->downloadRequest = std::shared_ptr<fileDownloadRequest>(new fileDownloadRequest(url,path.c_str()));

}
void download::initDownload(){
    LOG << "Starting download" << getURL();
    mutex.lock();
    if(downloadRequest != nullptr)
        downloadRequest->initDownload(this);
    mutex.unlock();
    LOG << "Finishing download" << getURL();
}

std::shared_ptr<fileDownloadRequest> download::getRequest() {
    return downloadRequest;
}

void download::deleteDownload() {
    downloadRequest->pauseDownload();
    while(downloadRequest->isDownloading());

    if(stored())
        removeFile(path.c_str());

    delete this;
}
const char * download::getID() {
    return id.c_str();
}

PNG * download::getIcon() {
    return pkg->getIcon();
}

package::PKGTypeENUM download::getPackageType(){
    return pkg->getPackageType();
}

const char * download::getDate() {
    return date.c_str();
}

const char * download::getPath() {
    return path.c_str();
}

bool download::stored() {
    return fileExists(path.c_str());
}
int download::unInstall() {
    return pkg->unInstall();
}

int download::install() {
    return pkg->install(path.c_str());
}

bool download::isInstalled() {
    return pkg->isInstalled();
}

bool download::hasFailed() {
    return failed;
}

bool download::hasFinished() {
    return finished;
}

const char *download::getName() {
    return pkg->getName();
}

const char *download::getURL() {
    return pkg->getURL();
}

double download::getVersion() {
    return pkg->getVersion();
}
const char * download::getVersionStr() {
    return pkg->getVersionStr();
}

bool download::updateAvailable() {
    return pkg->updateAvailable();
}

const char *download::getRepoName() {
    return pkg->getRepoName();
}
download::~download() {
    downloadRequest.reset();
    pkg.reset();
}

void download::setFinished() {
    if(settings::getMainSettings()->isFinishedDownloadingNotification()){
        std::string downloadMessage = LANG::mainLang->FINISHED_DOWNLOADING + ":\n";
        downloadMessage += pkg->getName();
        notifi(NULL, downloadMessage.c_str());
    }
    finished = true;

}

void download::setFailed(bool failed) {
    if(failed && settings::getMainSettings()->isFailedDownloadingNotification()) {
        std::string downloadMessage = LANG::mainLang->ERROR_WHEN_DOWNLOADING + ":\n";;
        downloadMessage += pkg->getURL();
        notifi(NULL, downloadMessage.c_str());
    }
    this->failed = failed;
}

const char *download::getTitleID() {
    return pkg->getTitleID();
}

double download::getCurrentSpeedInMB() {
    return downloadRequest->getDownloadSpeedInMB();
}

double download::getTimeLeftInMinutes() {
    return downloadRequest->getTimeLeftInMinutes();
}
