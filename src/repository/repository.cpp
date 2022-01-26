//
// Created by Víctor Jiménez Rugama on 12/24/21.
//
#include "../../include/utils/logger.h"
#include "../../include/repository/repository.h"
#include "../../include/repository/package.h"
#include "../../include/file/fileManager.h"
#include "../../include/file/fileDownloadRequest.h"
#include "../../include/utils/notifi.h"
#include "../../include/utils/PNG.h"
#include "../../include/utils/AnimatedPNG.h"
#include "../../include/utils/utils.h"
#include "../../include/view/packageSearch.h"
#include "../../include/repository/PKGInfo.h"

#include <utility>
#include <yaml-cpp/yaml.h>

repository::repository(const char * id, const char *name, const char *repoURL, const char * repoLocalPath, const char * iconPath) : id(id), repoURL(repoURL), repoLocalPath(repoLocalPath) {
    this->name = std::string(name);
    this->icon = new PNG(iconPath,ICON_DEFAULT_WIDTH,ICON_DEFAULT_HEIGHT);
    this->updating = false;
    this->updated = true;
    packageList = new std::vector<std::shared_ptr<package>>;

    updatePKGS();
}

const char * repository::getID() {
    return this->id.c_str();
}

int repository::updatePKGS() {

    std::string repoYMLPath = repoLocalPath + "repo.yml";

    if(fileExists(repoYMLPath.c_str()) == 0){
        LOG << "REPO DIRECTORY DOES NOT EXIST!" << repoYMLPath;
        return -1;
    }
    this->clearPackageList();
    std::string downloadURL;

    bool failedInit;
    YAML::Node repoYAML;
    try {
        repoYAML = YAML::LoadFile(repoYMLPath);
    } catch(const YAML::ParserException& ex) {
        LOG << ex.what();
        return -1;
    }
    for(YAML::const_iterator it=repoYAML.begin(); it!=repoYAML.end(); ++it) {
        if (it->second) {
            if((it->second).IsMap()) {
                const std::string &key = it->first.as<std::string>();
                if (strcasecmp(key.c_str(), "name") == 0 || strcasecmp(key.c_str(), "repoURL") == 0)
                    continue;

                YAML::Node attributes = it->second;
                if (!attributes["pkgPath"])
                    continue;

                if (attributes["pkgPath"].as<std::string>().find('.') == std::string::npos)
                    continue;
                std::string pkgPath = repoURL + attributes["pkgPath"].as<std::string>("");
                std::string type = attributes["type"].as<std::string>("");
                std::shared_ptr<package> pkg(
                        new package(pkgPath.c_str(), false, &failedInit, type.c_str(), name.c_str()));
                if (failedInit) {
                    LOG << "FAILED TO FETCH " << pkgPath;
                    pkg.reset();
                    continue;
                }
                addPkg(pkg);
            }
        }
    }
    return 0;
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

    YAML::Node repoYAML;
    try {
        repoYAML  = YAML::LoadFile(localTEMPDownloadPath);
    } catch(const YAML::ParserException& ex) {
        LOG << ex.what();
        return -1;
    }
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
    YAML::Node repoYAML;
    try {
        repoYAML = YAML::LoadFile(YMLPath);
    } catch(const YAML::ParserException& ex) {
        LOG << ex.what();
        return -1;
    }
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
    int packages = updatePKGS();
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

repository *repository::fetchRepo(const char *repoURL) {
        repository * repo = nullptr;

        std::string repoURLStr = repoURL;
        if(repoURLStr.back() != '/')
            repoURLStr+= '/';

        std::string localRepositoryFolder(STORED_PATH);
        localRepositoryFolder += REPO_PATH;

        std::string repoID = genRandom(10);
        localRepositoryFolder += repoID;
        while(folderExists(localRepositoryFolder.c_str())) {
            localRepositoryFolder = STORED_PATH;
            localRepositoryFolder += REPO_PATH;
            repoID = genRandom(10);
            localRepositoryFolder += repoID;
        }

        if(mkDir(localRepositoryFolder.c_str()) <0){
            LOG << "Error when creating folder " << localRepositoryFolder;
            return repo;
        }

        std::string localDownloadPath = localRepositoryFolder+"/repo.yml";
        std::string downloadURL = repoURLStr;
        downloadURL+="repo.yml";
        fileDownloadRequest YMLDownloadRequest(downloadURL.c_str(),localDownloadPath.c_str());
        if(YMLDownloadRequest.initDownload() <0){
            LOG << "Error when downloading repo.yml from " << downloadURL<< " to "<< localRepositoryFolder;
            removeDir(localRepositoryFolder.c_str());
            return nullptr;
        }
        YAML::Node repoYAML;
        try {
            repoYAML = YAML::LoadFile(localDownloadPath);
        } catch(const YAML::ParserException& ex) {
            LOG << ex.what();
            if(folderExists(localRepositoryFolder.c_str()))
                removeFile(localRepositoryFolder.c_str());
            return nullptr;
        }
        repoYAML["repoURL"] = repoURLStr;
        std::ofstream fout(localDownloadPath);
        fout << repoYAML;
        fout.close();

        std::string repoName = "Default Repo Name";
        if(repoYAML["name"]) {
            repoName = repoYAML["name"].as<std::string>();
        }

        std::string repoIconPath;
        if(repoYAML["iconPath"]) {
            repoIconPath = repoYAML["iconPath"].as<std::string>();
        }

        std::string iconDefaultPath = DATA_PATH;
        iconDefaultPath+="assets/images/repository/repoDefaultIcon.png";
        const char * iconDefaultPathChar = iconDefaultPath.c_str();

        int ret = -1;
        if(!repoIconPath.empty()){
            localDownloadPath = localRepositoryFolder+"/icon"+repoIconPath.substr(repoIconPath.find_last_of('.'));
            downloadURL = repoURLStr;
            downloadURL += repoIconPath;
            fileDownloadRequest iconDownloadRequest(downloadURL.c_str(),localDownloadPath.c_str());
            ret = iconDownloadRequest.initDownload();
        }

        if(ret < 0) {
            LOG << "Error when downloading icon from " << downloadURL<< " to "<< localDownloadPath;
            localDownloadPath = localRepositoryFolder + "/icon.png";
            copyFile(iconDefaultPathChar, localDownloadPath.c_str());
        }

        localRepositoryFolder+="/";
        repo = new repository(repoID.c_str(), repoName.c_str(), repoURLStr.c_str(), localRepositoryFolder.c_str(), localDownloadPath.c_str());

        return repo;
}

