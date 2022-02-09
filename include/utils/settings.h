//
// Created by Víctor Jiménez Rugama on 2/8/22.
//

#ifndef INC_4PT_SETTINGS_H
#define INC_4PT_SETTINGS_H

#include <string>
#include <yaml-cpp/yaml.h>

//class settings;

class settings {
private:
    static settings * mainSettings;
    //finished installing, finished downloading, added to downloads
    std::string currLANG;

    bool addedToDownloadsNotification;
    bool finishedDownloadingNotification;
    bool failedDownloadingNotification;
    const int notificationsOptions = 3;

    void saveSettings();
public:
    static settings * getMainSettings();
    explicit settings();
    static int initSettings();
    static int termSettings();

    void loadSettings();

    std::string getCurrLang();

    void setCurrLang(const std::string &currLang);

    bool isFinishedDownloadingNotification();
    void setFinishedDownloadingNotification(bool finishedDownloadingNotification);

    bool isAddedToDownloadsNotification();
    void setAddedToDownloadsNotification(bool addedToDownloadsNotification);

    bool isFailedDownloadingNotification() const;
    void setFailedDownloadingNotification(bool failedDownloadingNotification);

    int getNotificationsOptions();
};
#endif //INC_4PT_SETTINGS_H
