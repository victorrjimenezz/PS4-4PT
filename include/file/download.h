//
// Created by Víctor Jiménez Rugama on 1/2/22.
//

#ifndef CYDI4_DOWNLOAD_H
#define CYDI4_DOWNLOAD_H
#include "../repository/package.h"
#include "../repository/repository.h"
#include "../utils/utils.h"
#include "fileDownloadRequest.h"
#include <mutex>
#include <string>

class download {
private:
    std::string id;
    std::string date;
    std::string name;
    package::PKGTypeENUM packageType;
    std::string url;
    std::string version;
    std::string iconPath;
    std::string repoName;
    std::string path;
    std::mutex mutex;
    std::shared_ptr<fileDownloadRequest> downloadRequest;
    std::string genDate();
    PNG * icon;
    bool installed;
    bool finished;
    bool failed;
public:
    download(const char *url);
    download(const char *id, const char *date, const char *localPath, const char *url, const char *name,
             const char *version, const char *repoName, package::PKGTypeENUM type, const char *iconPath, bool finished);
    explicit download(const std::shared_ptr<package>& pkg);
    const char * getID();
    const char * getPath();
    const char * getDate();
    const char * getName();
    const char * getURL();
    const char * getIconPath();
    package::PKGTypeENUM getPackageType();
    const char * getRepoName();
    const char * getVersion();
    //std::shared_ptr<package> getPKG();
    void initDownload();
    void deleteDownload();
    void install();
    bool stored();
    std::shared_ptr<fileDownloadRequest> getRequest();
    //bool isDownloading();
    bool hasFinished();
    bool hasFailed();
    void setFinished();
    void setFailed(bool failed);
    bool isInstalled();
    PNG * getIcon();
    ~download();
};
#endif //CYDI4_DOWNLOAD_H
