//
// Created by Víctor Jiménez Rugama on 12/24/21.
//

#ifndef CYDI4_REPOSITORY_H
#define CYDI4_REPOSITORY_H
#include "../utils/PNG.h"
#include <string>
#include <vector>
#include "package.h"
#include "../utils/AnimatedPNG.h"

class repository {
private:
    const std::string id;
    std::string name;
    const std::string repoURL;
    const std::string repoLocalPath;
    PNG * icon;
    std::vector<std::shared_ptr<package>> *packageList;
    bool updating;
    bool updated;
    int updateYML();
    int updateIcon();
public:
    explicit repository(const char * id, const char *name, const char * repoURL, const char * repoLocalPath, const char * iconPath);
    int loadPackages();
    int updateRepository(AnimatedPNG * updateIcon = nullptr);
    PNG * getIcon();
    const char * getID();
    const char * getName();
    const char * getRepoURL();
    const char * getLocalPath();
    bool hasUpdated();
    void clearPackageList();
    void addPkg(const std::shared_ptr<package>& package);
    void setName(const char * newName);
    void deleteRepository();
    std::vector<std::shared_ptr<package>>* getPackageList();
    bool isUpdating() const;
    ~repository();

};
#endif //CYDI4_REPOSITORY_H
