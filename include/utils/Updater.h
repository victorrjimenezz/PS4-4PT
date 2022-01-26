//
// Created by Víctor Jiménez Rugama on 1/21/22.
//

#ifndef INC_4PT_UPDATER_H
#define INC_4PT_UPDATER_H

#include <string>
class package;
class Updater{
private:
    static std::string getLatestUpdate(const char * YAML);
    static bool installUpdate(package package);
    int static migrateFiles(const char * newStoredPath);
    static bool uninstallApp(const char *);
public:
    static int checkForUpdate();
};
#endif //INC_4PT_UPDATER_H
