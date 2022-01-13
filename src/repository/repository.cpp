//
// Created by Víctor Jiménez Rugama on 12/24/21.
//
#include "../../_common/notifi.h"
#include "../../include/utils/logger.h"
#include "../../include/repository/repository.h"
#include "../../include/repository/repoFetcher.h"
#include "../../include/file/fileManager.h"
#include "../../include/view/packageSearch.h"
#include "../../include/base.h"

#include <utility>
repository::repository(const char * id, const char *name, const char *repoURL, const char * repoLocalPath, const char * iconPath) : id(id), repoURL(repoURL), repoLocalPath(repoLocalPath) {
    this->name = std::string(name);
    this->icon = new PNG(iconPath,ICON_DEFAULT_WIDTH,ICON_DEFAULT_HEIGHT);
    this->updating = false;
    this->updated = true;
    packageList = new std::vector<std::shared_ptr<package>>;

    loadPackages();
}
const char * repository::getID() {
    return this->id.c_str();
}
PNG * repository::getIcon() {
    return this->icon;
}
const char * repository::getName() {
    return this->name.c_str();
}
const char * repository::getRepoURL() {
    return this->repoURL.c_str();
}
const char * repository::getLocalPath() {
    return this->repoLocalPath.c_str();
}
void repository::setName(const char * newName) {
    this->name = std::string(newName);
}

void repository::addPkg(const std::shared_ptr<package>& package){
    packageList->emplace_back(package);
}

int repository::updateYML() {
    std::string OldLocalYMLPath = repoLocalPath+"repo.yml";
    std::string localTEMPDownloadPath = repoLocalPath+"TEMPRepo.yml";
    std::string downloadURL = repoURL + "repo.yml";
    fileDownloadRequest YMLDownloadRequest(downloadURL.c_str(),localTEMPDownloadPath.c_str());
    if(YMLDownloadRequest.initDownload() <0){
        LOG << "Error when downloading repo.yml from " << downloadURL<< " to "<< localTEMPDownloadPath;
        return -1;
    }

    YAML::Node repoYAML  = YAML::LoadFile(localTEMPDownloadPath);
    if(!repoYAML || !repoYAML["name"]) {
        removeFile(localTEMPDownloadPath.c_str());
        return -1;
    }
    removeFile(OldLocalYMLPath.c_str());

    setName(repoYAML["name"].as<std::string>().c_str());
    repoYAML["repoURL"] = repoURL;
    std::ofstream fout(OldLocalYMLPath);
    fout << repoYAML;
    fout.close();

    removeFile(localTEMPDownloadPath.c_str());
    return 0;
}

int repository::updateIcon() {
    std::string YMLPath = repoLocalPath+"repo.yml";
    YAML::Node repoYAML = YAML::LoadFile(YMLPath);
    if(!repoYAML)
        return -1;

    std::string OldLocalIconPath = icon->getPath();

    if(!repoYAML["iconPath"])
        return 0;

    std::string repoIconPath = repoYAML["iconPath"].as<std::string>();
    std::string repoExt = repoIconPath.substr(repoIconPath.find_last_of('.'));

    std::string localDownloadPath = repoLocalPath+"tempicon"+repoExt;
    std::string downloadURL = repoURL;
    downloadURL += repoIconPath;

    fileDownloadRequest iconDownloadRequest(downloadURL.c_str(),localDownloadPath.c_str());
    if(iconDownloadRequest.initDownload() < 0)
        return -1;

    removeFile(OldLocalIconPath.c_str());
    OldLocalIconPath = OldLocalIconPath.substr(0,OldLocalIconPath.find_last_of('.')) +repoExt;
    moveFile(localDownloadPath.c_str(),OldLocalIconPath.c_str());
    int width = icon->getWidth(), height = icon->getHeight();
    delete icon;
    icon = new PNG(OldLocalIconPath.c_str(),width,height);
    return 0;
}

int repository::updateRepository(AnimatedPNG * updateIconPNG) {
    if(updating)
        return -1;
    updating = true;
    if(updateIconPNG != nullptr)
        updateIconPNG->play();
    updateYML();
    updateIcon();
    int packages = loadPackagesFromRepo(this);
    if(updateIconPNG != nullptr)
        updateIconPNG->stop();
    updating = false;
    packageSearch::mainPackageSearch->updatePackages();
    updated = true;
    return packages;
}

std::vector<std::shared_ptr<package>> * repository::getPackageList() {
    return packageList;
}
bool repository::hasUpdated(){
    bool oldUpdated = updated;
    updated = false;
    return oldUpdated;
}
void repository::clearPackageList() {
    for(auto & package : *packageList)
        package.reset();
    packageList->clear();
}

int repository::loadPackages() {
    return loadPackagesFromRepo(this);
}

void repository::deleteRepository() {
    while(updating)
        continue;
    removeDir(repoLocalPath.c_str());
    delete this;
}
repository::~repository() {
    delete icon;

    for(auto & package : *packageList)
        package.reset();

    delete(packageList);
}

bool repository::isUpdating() const {
    return updating;
}

