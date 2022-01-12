//
// Created by Víctor Jiménez Rugama on 1/2/22.
//
#include <sstream>
#include <orbis/AppInstUtil.h>
#include <orbis/Bgft.h>

#include "../../include/file/download.h"
#include "../../include/file/fileManager.h"
#include "../../include/repository/repoFetcher.h"
#include "../../include/view/downloadView.h"
#include "../../_common/notifi.h"
#include "../../include/utils/logger.h"

std::string download::genDate(){
    auto now = std::chrono::system_clock::now();
    time_t currentTime = std::chrono::system_clock::to_time_t(now);
    tm currentLocalTime = *localtime(&currentTime);

    std::ostringstream buffer;
    buffer << currentLocalTime.tm_year+1900;
    buffer << "/" <<currentLocalTime.tm_mon+1;
    buffer << "/" << currentLocalTime.tm_mday;
    buffer << " " << currentLocalTime.tm_hour;
    buffer << ":" << currentLocalTime.tm_min;

    return buffer.str();
}

download::download(const std::shared_ptr<package>& pkg){
    this->id = genRandom(10);
    this->date = genDate();
    std::string localDownloadPath = DOWNLOAD_PATH;
    localDownloadPath+= id+".pkg";
    this->path=localDownloadPath;
    std::string sourceURL = pkg->getPath();
    this->url = sourceURL;
    this->name = pkg->getName();
    this->version = pkg->getVersion();
    this->packageType = pkg->getPackageType();
    this->repoName = pkg->getRepo()->getName();
    this->iconPath = pkg->getIconPath();
    this->finished = false;
    this->failed = false;
    this->installed = false;
    this->icon = new PNG(pkg->getIconPath(),ICON_DEFAULT_WIDTH,ICON_DEFAULT_HEIGHT);

    this->downloadRequest = std::shared_ptr<fileDownloadRequest>(new fileDownloadRequest(sourceURL.c_str(),path.c_str()));
}

download::download(const char * id, const char * date, const char * localPath, const char * url, const char * name, const char * version, const char * repoName, package::PKGTypeENUM type, const char * iconPath, bool finished) {
    this->id = id;
    this->date = date;
    this->path=localPath;
    this->url = url;
    this->name = name;
    this->packageType = type;
    this->version = version;
    this->repoName = repoName;
    this->finished = false;
    this->installed = false;
    this->titleID = "";
    if (fileExists(localPath)) {
        if (finished){
            this->finished = true;
            setTitleID();
            this->installed = checkInstalled();
        }else
            removeFile(localPath);
    }
    this->failed = false;
    if(fileExists(iconPath)) {
        this->iconPath = iconPath;
        this->icon = new PNG(iconPath,ICON_DEFAULT_WIDTH,ICON_DEFAULT_HEIGHT);
    } else {
        this->iconPath = getDefaultIconPath(type);
        this->icon = new PNG(this->iconPath.c_str(),ICON_DEFAULT_WIDTH,ICON_DEFAULT_HEIGHT);
    }

    this->downloadRequest = std::shared_ptr<fileDownloadRequest>(new fileDownloadRequest(url,path.c_str()));
}

download::download(const char *url) {
    this->id = genRandom(10);
    std::string pkgURL = url;
    pkgURL = pkgURL.substr(pkgURL.find_last_of('/')+1);
    pkgURL = pkgURL.substr(0,pkgURL.find_last_of('.'));
    this->name=pkgURL;
    this->date = genDate();
    std::string localDownloadPath = DOWNLOAD_PATH;
    localDownloadPath+= id+".pkg";
    this->path=localDownloadPath;
    this->version = "UNKNOWN";
    this->packageType = package::MISC;
    this->url = url;
    this->repoName = "Direct Download";
    this->titleID = "";
    std::string iconDefaultPath = DATA_PATH;
    iconDefaultPath+="assets/images/repository/";
    iconDefaultPath+="miscDefaultIcon.png";
    this->iconPath = iconDefaultPath;
    this->finished = false;
    this->failed = false;
    this->installed = false;
    this->icon = new PNG(iconPath.c_str(),ICON_DEFAULT_WIDTH,ICON_DEFAULT_HEIGHT);

    this->downloadRequest = std::shared_ptr<fileDownloadRequest>(new fileDownloadRequest(url,path.c_str()));

}
void download::initDownload(){
    mutex.lock();
    if(downloadRequest != nullptr)
        downloadRequest->initDownload(this);
    if(downloadRequest->hasFinished())
        downloadView::downloadManager->setFinished(this);
    mutex.unlock();
}

std::shared_ptr<fileDownloadRequest> download::getRequest() {
    return downloadRequest;
}

void download::deleteDownload() {
    downloadRequest->cancelDownload();
    while(downloadRequest->isDownloading());

    if(stored())
        removeFile(path.c_str());

    delete this;
}
const char * download::getID() {
    return id.c_str();
}

PNG * download::getIcon() {
    return icon;
}

const char * download::getIconPath() {
    return iconPath.c_str();
}
package::PKGTypeENUM download::getPackageType(){
    return packageType;
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
    if(sceAppInstUtilAppUnInstall(&titleID[0]) != 0)
        return -1;
    installed = false;
    return 0;
}
int download::setTitleID(){
    int ret = 0, isApp = 0;
    char tempTitleID[16];
    if(titleID.empty()) {
        ret = sceAppInstUtilGetTitleIdFromPkg(path.c_str(), &tempTitleID[0], &isApp);
        titleID = tempTitleID;
        if (ret)
            LOG << "Error on sceAppInstUtilGetTitleIdFromPkg";
    }
    return ret;
}

int download::install() {
    int  ret;
    int  task_id = -1;
    char buffer[255];

    ret = setTitleID();

    if(ret)
        goto err;


    snprintf(buffer, 254, "%s via 4PT", titleID.c_str());
    OrbisBgftDownloadParamEx download_params;
    memset(&download_params, 0, sizeof(download_params));
    download_params.params.entitlementType = 5;
    download_params.params.id = titleID.c_str();
    download_params.params.contentUrl = path.c_str();
    download_params.params.contentName = buffer;
    download_params.params.iconPath = iconPath.c_str();
    download_params.params.playgoScenarioId = "0";
    download_params.params.option = ORBIS_BGFT_TASK_OPT_DELETE_AFTER_UPLOAD;
    download_params.slot = 0;


    retry:
    ret = sceBgftServiceIntDownloadRegisterTaskByStorageEx(&download_params, &task_id);

    if(ret == ORBIS_APPINSTUTIL_APP_ALREADY_INSTALLED) {
        ret = unInstall();
        if(ret != 0){
            LOG << "Error on sceAppInstUtilAppUnInstall";
            goto err;
        }

        goto retry;
    }
    else
    if(ret){
        LOG << "Error on sceBgftServiceIntDownloadRegisterTaskByStorageEx";
        goto err;
    }

    ret = sceBgftServiceDownloadStartTask(task_id);
    if(ret){
        LOG << "Error on sceBgftDownloadStartTask";
        goto err;
    }
    installed = true;
    return 0;

    err:
    std::string message = "Error when installing app";
    message+=name;
    notifi(NULL,message.c_str());
    return -1;

}

bool download::isInstalled() {
    return installed;
}

bool download::hasFailed() {
    return failed;
}

bool download::hasFinished() {
    return finished;
}

const char *download::getName() {
    return name.c_str();
}

const char *download::getURL() {
    return url.c_str();
}

const char *download::getVersion() {
    return version.c_str();
}

const char *download::getRepoName() {
    return repoName.c_str();
}
download::~download() {
    downloadRequest.reset();
    delete icon;
}

int download::checkInstalled() {
    int isInstalled = 0;
    if(!titleID.empty())
        sceAppInstUtilAppExists(titleID.c_str(), &isInstalled);
    return isInstalled;

}

void download::setFinished() {
    std::string downloadMessage = "Finished downloading:\n";
    downloadMessage += name;
    notifi(NULL, downloadMessage.c_str());
    finished = true;
    if(setTitleID() == 0)
        installed = checkInstalled();

}

void download::setFailed(bool failed) {
    if(failed) {
        std::string downloadMessage = "Error when downloading:\n";
        downloadMessage += url;
        notifi(NULL, downloadMessage.c_str());
    }
    this->failed = failed;
}
