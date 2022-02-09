//
// Created by Víctor Jiménez Rugama on 1/21/22.
//
#include "../../include/utils/Updater.h"
#include "../../include/file/fileDownloadRequest.h"
#include "../../include/base.h"
#include "../../include/utils/logger.h"
#include "../../include/repository/package.h"
#include "../../include/utils/dialog.h"
#include "../../include/file/fileManager.h"

#include <yaml-cpp/yaml.h>
#include <orbis/AppInstUtil.h>

int Updater::checkForUpdate() {
    std::string localUpdatesDownloadFile = STORED_PATH;
    localUpdatesDownloadFile+=UPDATES_FILE;
    fileDownloadRequest downloadRequest(UPDATES_FILE_URL,localUpdatesDownloadFile.c_str());
    int ret = downloadRequest.initDownload();

    if(ret < 0) {
        LOG << "COULD NOT FETCH DOWNLOAD " << UPDATES_FILE_URL << " to " << localUpdatesDownloadFile;
        return -1;
    }

    std::string updateFile = getLatestUpdate(localUpdatesDownloadFile.c_str());
    if(!updateFile.empty()){
        localUpdatesDownloadFile = INSTALL_PATH + updateFile.substr(updateFile.find_last_of('/')+1);
        updateFile = MAIN_URL+updateFile;
        fileDownloadRequest updateDownloadRequest(updateFile.c_str(),localUpdatesDownloadFile.c_str());
        if(updateDownloadRequest.initDownload() < 0) {
            LOG << "COULD NOT FETCH DOWNLOAD" << updateFile << " to " << updateFile;
            return -1;
        }

        bool failed;
        package updatePKG(localUpdatesDownloadFile.c_str(),true, &failed);
        if(failed)
            return -2;
        if(!installUpdate(updatePKG))
            return -3;
        else
            popDialog("Installed Update!\nPlease restart the APP");
        if(!uninstallApp(APP_TITLEID))
            return -4;
    }
    return 0;
}

std::string Updater::getLatestUpdate(const char * YAML) {
    double version;
    try {
        YAML::Node updatesFile = YAML::LoadFile(YAML);
        for(YAML::const_iterator it=updatesFile.begin(); it!=updatesFile.end(); ++it){
            if(it->second){
                version = it->first.as<double>(0);
                if(version > APP_VERSION)
                    return it->second.as<std::string>("");
            }
        }
    } catch(const YAML::ParserException& ex) {
        LOG << ex.what();
        if(fileExists(YAML))
            removeFile(YAML);
    }

    return  "";
}

bool Updater::installUpdate(package updatePackage) {
    return updatePackage.install() == 0;
}

bool Updater::uninstallApp(const char * TITLE_ID) {
    if(sceAppInstUtilAppUnInstall(TITLE_ID) != 0)
        return false;
    return true;
}
