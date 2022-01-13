//
// Created by Víctor Jiménez Rugama on 12/29/21.
//
#include "../../include/repository/package.h"
#include "../../include/utils/logger.h"
#include "../../include/file/fileManager.h"

package::package(const char*name, const char*path, const char* packageType, const char*version, const char * iconPath, repository * repository){
    this->name = name;
    this->path = path;
    this->iconPath = iconPath;
    this->icon = new PNG(iconPath,ICON_DEFAULT_WIDTH,ICON_DEFAULT_HEIGHT);
    this->packageType = getPackageType(packageType);
    this->version = version;
    this->repo = repository;
}


package::PKGTypeENUM package::getPackageType(const char *packageType) {
    PKGTypeENUM pkgType = MISC;
    for (int i = GAME; i != MISC; i++ )
        if(strcasecmp(TypeStr[i],packageType) == 0)
            pkgType = static_cast<PKGTypeENUM>(i);
    return pkgType;
}

package::PKGTypeENUM package::getPackageType(){
    return packageType;
}
package::~package(){
    removeFile(icon->getPath().c_str());
    delete icon;
}

repository *package::getRepo() {
    return this->repo;
}

const char *package::getName() {
    return this->name.c_str();
}

const char * package::getVersion() {
    return this->version.c_str();
}

const char * package::getIconPath() {
    return this->iconPath.c_str();
}

const char *package::getPath() {
    return this->path.c_str();
}

PNG *package::getIcon() {
    return icon;
}
