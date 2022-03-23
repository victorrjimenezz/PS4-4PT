//
// Created by Víctor Jiménez Rugama on 12/24/21.
//

#ifndef CYDI4_REPOSITORY_H
#define CYDI4_REPOSITORY_H
#include <string>

#include <vector>
#include "../../include/utils/threadPool.h"

class PNG;
class package;
class repository {
private:
    std::mutex updateMtx;
    std::mutex addPKGMtx;

    const std::string id;
    std::string name;
    std::string repoURL;
    const std::string repoLocalPath;
    std::atomic_int32_t updatedCount;

    PNG * icon;
    std::vector<std::shared_ptr<package>> *packageList;
    void addPKG(std::string pkgURL, std::string pkgType);
    bool updating;
    bool updated;
    bool willDelete;
    int updateYML();
    int updateIcon();
    int updatePKGS();
    void sendTerminalMessage(const char * message);
    static void sendTerminalMessage(const char *message, const char * repoURLNew);
public:
    explicit repository(const char * id, const char *name, const char * repoURL, const char * repoLocalPath, const char * iconPath);
    static repository* fetchRepo(const char *repoURLStr);
    int updateRepository();
    PNG * getIcon();
    const char * getID();
    const char * getName();
    const char * getRepoURL();
    const char * getLocalPath();
    bool hasUpdated();
    void setName(const char * newName);
    void deleteRepository();
    std::vector<std::shared_ptr<package>>* getPackageList();
    bool isUpdating() const;
    ~repository();

};
#endif //CYDI4_REPOSITORY_H
