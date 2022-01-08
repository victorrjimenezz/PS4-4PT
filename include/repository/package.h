//
// Created by Víctor Jiménez Rugama on 12/29/21.
//

#ifndef CYDI4_PACKAGE_H
#define CYDI4_PACKAGE_H

#include <cstdlib>
#include <string>
#include "../utils/PNG.h"
#include "../base.h"

class repository;

static const char* TypeStr[PKG_TYPE_AMOUNT]={"GAME", "APP", "UPDATE", "THEME", "TOOL", "CHEAT", "MISC"};

class package {
public:
    enum PKGTypeENUM{
        GAME,APP, UPDATE, THEME, TOOL,CHEAT, MISC
    };
private:
    std::string name;
    std::string path;
    std::string iconPath;
    repository * repo;
    PNG * icon;
    enum PKGTypeENUM packageType;
    std::string version;
public:
    static PKGTypeENUM getPackageType(const char * packageType);
    explicit package(const char*name, const char*path, const char* packageType, const char*version, const char * icon, repository * repository);
    const char * getName();
    PNG * getIcon();
    repository * getRepo();
    const char * getPath();
    const char * getVersion();
    const char * getIconPath();
    enum PKGTypeENUM getPackageType();
    ~package();
};

#endif //CYDI4_PACKAGE_H
