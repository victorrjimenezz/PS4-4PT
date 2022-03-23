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
    std::string currLANG;

    bool addedToDownloadsNotification;
    bool finishedDownloadingNotification;
    bool failedDownloadingNotification;
    const int notificationsOptions = 3;

    bool installDirectlyPS4;

    void saveSettings();
public:
    explicit settings();

    void loadSettings();

    std::string getCurrLang();

    void setCurrLang(const std::string &currLang);

    bool isFinishedDownloadingNotification();
    void setFinishedDownloadingNotification(bool finishedDownloadingNotification);

    bool isAddedToDownloadsNotification();
    void setAddedToDownloadsNotification(bool addedToDownloadsNotification);

    bool isFailedDownloadingNotification() const;
    void setFailedDownloadingNotification(bool failedDownloadingNotification);

    bool shouldInstallDirectlyPS4() const;
    void setInstallDirectlyPS4(bool installDirectlyPS4);

    int getNotificationsOptions();
};
#endif //INC_4PT_SETTINGS_H
