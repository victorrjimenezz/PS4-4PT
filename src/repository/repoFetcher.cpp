//
// Created by Víctor Jiménez Rugama on 12/27/21.
//
#include <string>

//Includes
#include "../../include/repository/repoFetcher.h"
#include "../../include/file/fileManager.h"
#include "../../include/utils/logger.h"
#include "../../include/utils/utils.h"
#include <yaml-cpp/yaml.h>

package::PKGTypeENUM getPKGType(const char * type){
    package::PKGTypeENUM packageType = package::MISC;
    for(int i = 0; i < PKG_TYPE_AMOUNT; i ++){
        if(strcasecmp(TypeStr[i],type) == 0)
        packageType = static_cast<package::PKGTypeENUM>(i);
    }
    return packageType;
}

int loadDownloadList(std::vector<download*> * downloadList, YAML::Node downloadsFile){
    for(YAML::const_iterator it=downloadsFile.begin(); it!=downloadsFile.end(); ++it){
        if(it->second){
            const std::string &key=it->first.as<std::string>();
            if(strcasecmp(key.c_str(),"name") == 0 || strcasecmp(key.c_str(),"repoURL") == 0)
                continue;
            std::string id = key;

            YAML::Node attributes = it->second;
            if(!attributes["date"] || !attributes["path"] || !attributes["url"] || !attributes["name"] || !attributes["version"] || !attributes["repoName"] || !attributes["type"] || !attributes["iconPath"] || !attributes["finished"])
                continue;
            std::string date = attributes["date"].as<std::string>();
            std::string path = attributes["path"].as<std::string>();
            std::string url = attributes["url"].as<std::string>();
            std::string name = attributes["name"].as<std::string>();
            std::string version = attributes["version"].as<std::string>();
            std::string repoName = attributes["repoName"].as<std::string>();
            std::string type = attributes["type"].as<std::string>();
            std::string iconPath = attributes["iconPath"].as<std::string>();
            bool finished = attributes["finished"].as<bool>(false);
            downloadList->emplace_back(new download(id.c_str(),date.c_str(),path.c_str(),url.c_str(),name.c_str(),version.c_str(),repoName.c_str(),getPKGType(type.c_str()),iconPath.c_str(),finished));
        }
    }
    return 0;
}

int loadSavedRepos(repositoryView * repositoryView){
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
            repoYAML = YAML::LoadFile(loadingRepoPathTEMP);
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
                    repositoryView->addRepository(new repository(file.c_str(), repoName.c_str(), repoURL.c_str(), loadingRepoFolder.c_str(), localIconDir.c_str()));
                } else {
                    repoYAML = YAML::LoadFile(loadingRepoPath);

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
                    repositoryView->addRepository(new repository(file.c_str(), repoName.c_str(), repoURL.c_str(), loadingRepoFolder.c_str(), localDownloadPath.c_str()));
                }
            } else {
                loadingRepoPath = repoPath+file;
                LOG << "RepoURL NOT found. Removing Directory" << loadingRepoPath;
                removeDir(loadingRepoPath.c_str());
            }
        } else {
            loadingRepoPath = repoPath+file;
            LOG << "repo.yml NOT found. Removing Directory" << loadingRepoPath;
            removeDir(loadingRepoPath.c_str());
        }


    }
    return 0;
}

std::string getDefaultIconPath(package::PKGTypeENUM type){

    std::string iconDefaultPath = DATA_PATH;
    iconDefaultPath+="assets/images/repository/";
    std::string defaultGameIcon = iconDefaultPath+"gameDefaultIcon.png";
    std::string defaultAppIcon = iconDefaultPath+"appDefaultIcon.png";
    std::string defaultUpdateIcon = iconDefaultPath+"updateDefaultIcon.png";
    std::string defaultThemeIcon = iconDefaultPath+"themeDefaultIcon.png";
    std::string defaultToolIcon = iconDefaultPath+"toolDefaultIcon.png";
    std::string defaultCheatIcon = iconDefaultPath+"cheatDefaultIcon.png";
    std::string defaultMiscIcon = iconDefaultPath+"miscDefaultIcon.png";

    std::string localIconPath;
    switch (type) {
        case package::PKGTypeENUM::GAME:
            localIconPath = defaultGameIcon;
            break;
        case package::PKGTypeENUM::APP:
            localIconPath = defaultAppIcon;
            break;
        case package::PKGTypeENUM::UPDATE:
            localIconPath = defaultUpdateIcon;
            break;
        case package::PKGTypeENUM::THEME:
            localIconPath = defaultThemeIcon;
            break;
        case package::PKGTypeENUM::TOOL:
            localIconPath = defaultToolIcon;
            break;
        case package::PKGTypeENUM::CHEAT:
            localIconPath = defaultCheatIcon;
            break;
        case package::PKGTypeENUM::MISC:
        default:
            localIconPath = defaultMiscIcon;
            break;

    }
    return localIconPath;
}

int loadPackagesFromRepo(repository* repository){
    std::string localRepoPath = repository->getLocalPath();
    std::string repoYMLPath = localRepoPath + "repo.yml";

    if(fileExists(repoYMLPath.c_str()) == 0){
        LOG << "REPO DIRECTORY DOES NOT EXIST!" << repoYMLPath;
        return -1;
    }

    repository->clearPackageList();

    std::string repoURL = repository->getRepoURL();
    std::string downloadURL;

    YAML::Node repoYAML  = YAML::LoadFile(repoYMLPath);
    for(YAML::const_iterator it=repoYAML.begin(); it!=repoYAML.end(); ++it){
        if(it->second){
        const std::string &key=it->first.as<std::string>();
        if(strcasecmp(key.c_str(),"name") == 0 || strcasecmp(key.c_str(),"repoURL") == 0 || strcasecmp(key.c_str(),"iconPath") == 0)
            continue;
        std::string packageName = key;

        YAML::Node attributes = it->second;
        if(!attributes["pkgPath"] || !attributes["type"] || !attributes["version"])
            continue;

        std::string pkgPath = repoURL+attributes["pkgPath"].as<std::string>();
        std::string iconPath;
        bool foundIcon = false;
        if(attributes["iconPath"]) {
            iconPath = attributes["iconPath"].as<std::string>();
            foundIcon = true;
        }
        if(iconPath.find('.') == std::string::npos || pkgPath.find('.') == std::string::npos)
            continue;

        std::string type = attributes["type"].as<std::string>();
        std::string version = attributes["version"].as<std::string>();

        std::string localIconPath;
        if(foundIcon) {
            downloadURL = repoURL + iconPath;
            localIconPath = localRepoPath + genRandom(10) + iconPath.substr(iconPath.find_last_of('.'));
            fileDownloadRequest downloadRequest(downloadURL.c_str(), localIconPath.c_str());
            if (downloadRequest.initDownload() < 0) {
                LOG << "Could Not Download Icon" << downloadURL << " to " << localIconPath;
                localIconPath = getDefaultIconPath(package::getPackageType(type.c_str()));
            }
        } else
            localIconPath = getDefaultIconPath(package::getPackageType(type.c_str()));
        repository->addPkg(std::shared_ptr<package>(new package(packageName.c_str(),pkgPath.c_str(),type.c_str(),version.c_str(),localIconPath.c_str(),repository)));
        }
    }

    return 0;
}

repository* fetchRepo(const char *repoURL) {
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

    YAML::Node repoYAML  = YAML::LoadFile(localDownloadPath);

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