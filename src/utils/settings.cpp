//
// Created by Víctor Jiménez Rugama on 2/8/22.
//
#include "../../include/utils/settings.h"
#include "../../include/base.h"
#include "../../include/utils/logger.h"
#include "../../include/utils/LANG.h"
#include "../../include/file/fileManager.h"
#include <fstream>

settings * settings::mainSettings = nullptr;

int settings::initSettings() {
    if(mainSettings!= nullptr)
        return 1;
    mainSettings = new settings();
    return 0;
}

std::string settings::getCurrLang() {
    return currLANG;
}

void settings::setCurrLang(const std::string &currLang) {
    currLANG = currLang;
    LANG::mainLang->loadLang();
    saveSettings();
}

bool settings::isFinishedDownloadingNotification() {
    return finishedDownloadingNotification;
}

void settings::setFinishedDownloadingNotification(bool finishedDownloadingNotification) {
    settings::finishedDownloadingNotification = finishedDownloadingNotification;
    saveSettings();
}

bool settings::isAddedToDownloadsNotification() {
    return addedToDownloadsNotification;
}

void settings::setAddedToDownloadsNotification(bool addedToDownloadsNotification) {
    settings::addedToDownloadsNotification = addedToDownloadsNotification;
    saveSettings();
}

void settings::saveSettings() {
    if(mainSettings == nullptr)
        return;
    YAML::Node settingsFileYML;
    settingsFileYML["LANG"] = currLANG;
    settingsFileYML["FAILED_DOWNLOADING_NOTIFICATION"] = failedDownloadingNotification;
    settingsFileYML["ADD_DOWNLOAD_NOTIFICATION"] = addedToDownloadsNotification;
    settingsFileYML["DOWNLOAD_NOTIFICATION"] = finishedDownloadingNotification;
    settingsFileYML["INSTALL_DIRECTLY_PS4"] = installDirectlyPS4;

    std::ofstream downloadsFile(SETTINGS_FILE, std::ofstream::out | std::ofstream::trunc);
    downloadsFile << settingsFileYML;
    downloadsFile.flush();
    downloadsFile.close();
}

void settings::loadSettings() {
    if(fileExists(SETTINGS_FILE))
        try {
            YAML::Node settingsFileYML = YAML::LoadFile(SETTINGS_FILE);
            if(settingsFileYML["LANG"])
                currLANG= settingsFileYML["LANG"].as<std::string>(currLANG);

            if(settingsFileYML["FAILED_DOWNLOADING_NOTIFICATION"])
                failedDownloadingNotification = settingsFileYML["FAILED_DOWNLOADING_NOTIFICATION"].as<bool>(failedDownloadingNotification);

            if(settingsFileYML["ADD_DOWNLOAD_NOTIFICATION"])
                addedToDownloadsNotification = settingsFileYML["ADD_DOWNLOAD_NOTIFICATION"].as<bool>(addedToDownloadsNotification);

            if(settingsFileYML["DOWNLOAD_NOTIFICATION"])
                finishedDownloadingNotification = settingsFileYML["DOWNLOAD_NOTIFICATION"].as<bool>(finishedDownloadingNotification);

            if(settingsFileYML["INSTALL_DIRECTLY_PS4"])
                installDirectlyPS4 = settingsFileYML["INSTALL_DIRECTLY_PS4"].as<bool>(installDirectlyPS4);

        } catch(YAML::Exception & exception) {
            LOG << "ERROR WHEN LOADING SETTINGS" << exception.what();
        }
}

int settings::termSettings() {
    if(mainSettings!= nullptr){
        delete mainSettings;
        mainSettings = nullptr;
    }
    return 0;
}

settings::settings() {
    currLANG = "NONE";
    addedToDownloadsNotification = true;
    failedDownloadingNotification = true;
    finishedDownloadingNotification = true;
    installDirectlyPS4 = false;
}

settings *settings::getMainSettings() {
    return mainSettings;
}

int settings::getNotificationsOptions() {
    return notificationsOptions;
}

bool settings::isFailedDownloadingNotification() const {
    return failedDownloadingNotification;
}

void settings::setFailedDownloadingNotification(bool failedDownloadingNotification) {
    settings::failedDownloadingNotification = failedDownloadingNotification;
    saveSettings();
}

bool settings::shouldInstallDirectlyPS4() const {
    return installDirectlyPS4;
}

void settings::setInstallDirectlyPS4(bool installDirectlyPS4) {
    this->installDirectlyPS4 = installDirectlyPS4;
    saveSettings();
}

