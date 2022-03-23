//
// Created by Víctor Jiménez Rugama on 12/29/21.
//
#include "../../include/repository/package.h"
#include "../../include/file/fileManager.h"
#include "../../include/file/fileDownloadRequest.h"
#include "../../include/repository/PKGInfo.h"
#include "../../include/utils/logger.h"
#include "../../include/utils/PNG.h"
#include "../../include/utils/notifi.h"
#include "../../include/utils/LANG.h"
#include "../../include/main.h"

#include <sstream>
#include <orbis/AppInstUtil.h>
#include <orbis/bgft.h>
#include <orbis/UserService.h>

package::package(const char*url, bool local, bool * failed, const char * type, const char * repositoryName){
    std::ostringstream stringStream;
    stringStream.precision(2);
    PKGInfo pkgInfo(url,local);
    this->icon = pkgInfo.getIconCopy();
    this->url = url;
    const char * fileExt;
    if(this->url.find_last_of('.') == std::string::npos) {
        LOG << "Invalid PKG at " << url;
        goto err;
    }
    fileExt = this->url.substr(this->url.find_last_of('.')).c_str();
    if(this->url.size()<4 || strcasecmp(fileExt, ".pkg") != 0 || strcasecmp(pkgInfo.getTitle(),"") == 0 || strcasecmp(pkgInfo.getTitleID(),"") == 0 || pkgInfo.getVersion()==-1){
        LOG << "Invalid PKG at " << url;
        goto err;
    }
    this->TITLE_ID = pkgInfo.getTitleID();
    this->CONTENT_ID = pkgInfo.getContentID();
    this->name = pkgInfo.getTitle();
    this->packageType = getPackageType(type);
    if(this->icon == nullptr)
        setDefaultIcon();
    this->version = pkgInfo.getVersion();
    this->systemVersion = pkgInfo.getSystemVersion();
    this->repoName = repositoryName;
    this->packageSizeBytes = pkgInfo.getPkgSize();

    stringStream << std::fixed << packageSizeBytes/ONE_MB;
    this->packageSizeMB = stringStream.str();
    stringStream.str(std::string());
    this->oldInstalled = isInstalledPrivate();

    updateCurrVersion();

    stringStream << std::fixed << version;
    this->versionString = stringStream.str();

    stringStream.str(std::string());
    stringStream << std::fixed << systemVersion;
    this->systemVersionString = stringStream.str();

    pkgSFOType = "";
    if(strcasecmp(pkgInfo.getType(),"gc") == 0)
        pkgSFOType = getMainLang()->GAME_CONTENT;
    else if(strcasecmp(pkgInfo.getType(),"gpc") == 0 || strcasecmp(pkgInfo.getType(),"gpd") == 0)
        pkgSFOType = getMainLang()->PATCH;

    *failed = false;
    return;
    err:
    delete icon;
    icon = nullptr;
    *failed = true;
}

double package::getCurrentInstalledVersion(){
    return currentInstalledVersion;
}

double package::getSystemVersion() const{
    return systemVersion;
}

package::PKGTypeENUM package::getPackageType(const char *packageType) {
    PKGTypeENUM pkgType = MISC;
    for(int i = 0; i < PKG_TYPE_AMOUNT;i++)
        if(strcasecmp(TypeStr[i],packageType) == 0)
            pkgType = static_cast<PKGTypeENUM>(i);
    return pkgType;
}

bool package::isInstalledPrivate() {
    return fileExists((INSTALL_PATH+std::string(TITLE_ID)).c_str());
}

bool package::isInstalled() {
    bool installed = isInstalledPrivate();
    if(installed != oldInstalled) {
        oldInstalled = installed;
        updateCurrVersion();
    }
    return installed;
}

int package::unInstall() {
    if(sceAppInstUtilAppUnInstall(TITLE_ID.c_str()) != 0)
        return -1;
    return 0;
}

int package::install(const char * path) {
    int  ret, userID;
    int  task_id = -1;
    char buffer[255];
    const char * titleID = TITLE_ID.c_str();
    const char * installPath;
    if(strcasecmp(path,"") == 0)
        return install();
    else
        installPath = path;

    ret = sceUserServiceGetForegroundUser(&userID);
    if (ret) {
        LOG << "sceUserServiceGetForegroundUser failed: " << ret;
        goto err;
    }

    snprintf(buffer, 254, "%s via 4PT", titleID);
    OrbisBgftDownloadParamEx download_params;
    memset(&download_params, 0, sizeof(download_params));
    download_params.params.entitlementType = 5;
    download_params.params.id = titleID;
    download_params.params.userId = userID;
    download_params.params.contentUrl = installPath;
    download_params.params.contentName = buffer;
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
    } else if(ret){
        LOG << "Error on sceBgftServiceIntDownloadRegisterTaskByStorageEx";
        goto err;
    }

    ret = sceBgftServiceDownloadStartTask(task_id);
    if(ret){
        LOG << "Error on sceBgftDownloadStartTask";
        goto err;
    }

    return 0;

    err:
    std::string message = getMainLang()->ERROR_WHEN_INSTALLING_APP;
    message+=name;
    notifi(NULL,message.c_str());
    return -1;

}

int package::install() {
    int  ret, userID;
    int  task_id = -1;
    char buffer[255];

    LOG << "Installing app from url" << url;

    ret = sceUserServiceGetForegroundUser(&userID);
    if (ret) {
        LOG << "sceUserServiceGetForegroundUser failed: " << ret;
        goto err;
    }

    snprintf(buffer, 254, "%s via 4PT", TITLE_ID.c_str());
    OrbisBgftDownloadParam download_params;
    memset(&download_params, 0, sizeof(download_params));
    {

        download_params.userId = userID;
        download_params.id = CONTENT_ID.c_str();
        download_params.contentUrl = url.c_str();
        download_params.contentName = TITLE_ID.c_str();
        download_params.iconPath = "";
        download_params.playgoScenarioId = "0";
        download_params.option = ORBIS_BGFT_TASK_OPT_DISABLE_CDN_QUERY_PARAM;
        download_params.packageType = pkgSFOType.c_str();
        download_params.packageSubType = "";
        download_params.packageSize = packageSizeBytes;
    }

    retry:
    ret = sceBgftServiceIntDownloadRegisterTask(&download_params, &task_id);

    if(ret == ORBIS_APPINSTUTIL_APP_ALREADY_INSTALLED) {
        ret = unInstall();
        if(ret != 0){
            LOG << "Error on sceAppInstUtilAppUnInstall";
            goto err;
        }

        goto retry;
    } else if(ret){
        LOG << "Error on sceBgftServiceIntDownloadRegisterTask";
        goto err;
    }

    ret = sceBgftServiceDownloadStartTask(task_id);
    if(ret){
        LOG << "Error on sceBgftDownloadStartTask";
        goto err;
    }

    return 0;

    err:
    std::string message = getMainLang()->ERROR_WHEN_INSTALLING_APP;
    message+=name;
    notifi(NULL,message.c_str());
    return -1;

}

package::PKGTypeENUM package::getPackageType(){
    return packageType;
}
package::~package(){
    delete icon;
}

const char *package::getName() {
    return this->name.c_str();
}

const char *package::getURL() {
    return this->url.c_str();
}

double package::getVersion() const {
    return this->version;
}

PNG *package::getIcon() {
    return icon;
}

const char *package::getTitleID() {
    return TITLE_ID.c_str();
}
const char * package::getVersionStr(){
    return versionString.c_str();
}
const char * package::getSystemVersionStr(){
    return systemVersionString.c_str();
}

uint64_t package::getPkgSize() {
    return packageSizeBytes;
}

const char * package::getPkgSizeMB() {
    return packageSizeMB.c_str();
}

const char *package::getRepoName() {
    return repoName.c_str();
}

const char *package::getSFOType() {
    return pkgSFOType.c_str();
}

package::package(package *oldPackage) {
    name = oldPackage->getName();
    repoName = oldPackage->getRepoName();
    url = oldPackage->getURL();
    if(oldPackage->getIcon() == nullptr)
        icon = nullptr;
    else
        icon = new PNG(oldPackage->getIcon());
    packageType = oldPackage->getPackageType();
    TITLE_ID = oldPackage->getTitleID();
    packageSizeBytes = oldPackage->getPkgSize();
    packageSizeMB = oldPackage->getPkgSizeMB();
    version = oldPackage->getVersion();
    pkgSFOType = oldPackage->getSFOType();
    versionString = oldPackage->getVersionStr();
    currentInstalledVersion = oldPackage->getCurrentInstalledVersion();
}

void package::setDefaultIcon() {
    std::string iconPath = DATA_PATH;
    iconPath+="assets/images/repository/";
    switch(packageType){
        case GAME:
            iconPath+="gameDefaultIcon.png";
            break;
        case APP:
            iconPath+="appDefaultIcon.png";
            break;
        case UPDATE:
            iconPath+="updateDefaultIcon.png";
            break;
        case THEME:
            iconPath+="themeDefaultIcon.png";
            break;
        case TOOL:
            iconPath+="toolDefaultIcon.png";
            break;
        case CHEAT:
            iconPath+="cheatDefaultIcon.png";
            break;
        case MISC:
        default:
            iconPath+="miscDefaultIcon.png";
            break;
    }
    this->icon = new PNG(iconPath.c_str(),ICON_DEFAULT_WIDTH,ICON_DEFAULT_HEIGHT);
}

bool package::updateAvailable() {
    return this->getVersion()>this->getCurrentInstalledVersion();
}

void package::updateCurrVersion() {
    if(isInstalledPrivate()){
        PKGInfo pkgInfoInstalled((INSTALL_PATH+std::string(TITLE_ID)+"/app.pkg").c_str(),true);
        this->currentInstalledVersion = pkgInfoInstalled.getVersion();
    } else
        this->currentInstalledVersion = version;
}
