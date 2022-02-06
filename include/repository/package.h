//
// Created by Víctor Jiménez Rugama on 12/29/21.
//

#ifndef CYDI4_PACKAGE_H
#define CYDI4_PACKAGE_H

#include <cstdlib>
#include <string>


#include "../base.h"

class repository;
class PNG;

static const char* TypeStr[PKG_TYPE_AMOUNT]={"GAME", "APP", "UPDATE", "THEME", "TOOL", "CHEAT", "MISC"};

class package {
public:
    enum PKGTypeENUM{
        GAME,APP, UPDATE, THEME, TOOL,CHEAT, MISC
    };
private:
    std::string name;
    std::string repoName;
    std::string url;
    PNG * icon;
    enum PKGTypeENUM packageType;
    std::string TITLE_ID;
    uint64_t packageSizeBytes;
    std::string packageSizeMB;
    double version;
    std::string pkgSFOType;
    std::string versionString;
    std::string systemVersionString;
    double systemVersion;
    double currentInstalledVersion;
    double getCurrentInstalledVersion() const;
    void setDefaultIcon();

public:
    static PKGTypeENUM getPackageType(const char * packageType);
    package(package * oldPackage);
    package(const char*url, bool local, bool * failed, const char * type = "", const char * repositoryName = "Direct Download");
    const char * getName();
    const char * getVersionStr();
    const char * getSystemVersionStr();
    PNG * getIcon();
    const char * getTitleID();
    const char * getRepoName();
    const char * getURL();
    const char * getSFOType();
    double getSystemVersion() const;
    double getVersion() const;
    uint64_t getPkgSize();
    const char * getPkgSizeMB();
    enum PKGTypeENUM getPackageType();
    ~package();
    double getCurrVer();
    int unInstall();
    int install(const char * path = "");
    bool updateAvailable();
    bool isInstalled() const;
};

#endif //CYDI4_PACKAGE_H
