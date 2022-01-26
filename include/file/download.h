//
// Created by Víctor Jiménez Rugama on 1/2/22.
//

#ifndef CYDI4_DOWNLOAD_H
#define CYDI4_DOWNLOAD_H

#include "../repository/package.h"

#include <mutex>
#include <string>

class package;
class fileDownloadRequest;
class PNG;
class download {
private:
    std::string id;
    std::string date;
    std::string path;
    std::mutex mutex;
    std::shared_ptr<fileDownloadRequest> downloadRequest;
    std::shared_ptr<package> pkg;
    bool finished;
    bool failed;
public:
    download(const char *url, bool * failedInit);
    download(const char * id, const char * date, const char * localPath, bool * failedInit, const char * url="", const char * type = "MISC", const char * repoName = "UNKOWN");
    explicit download(const std::shared_ptr<package>& pkg);
    const char * getID();
    const char * getPath();
    const char * getDate();
    const char * getName();
    const char * getURL();
    const char * getVersionStr();
    int install();
    int unInstall();
    package::PKGTypeENUM getPackageType();
    const char * getRepoName();
    const char * getTitleID();
    double getVersion();
    void initDownload();
    void deleteDownload();
    bool stored();
    std::shared_ptr<fileDownloadRequest> getRequest();
    bool hasFinished();
    bool hasFailed();
    void setFinished();
    void setFailed(bool failed);
    bool isInstalled();
    PNG * getIcon();
    ~download();
};
#endif //CYDI4_DOWNLOAD_H
