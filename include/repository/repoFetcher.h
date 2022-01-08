//
// Created by Víctor Jiménez Rugama on 12/27/21.
//

#ifndef CYDI4_REPOFETCHER_H
#define CYDI4_REPOFETCHER_H
#include "repository.h"
#include "../file/download.h"
#include "../../include/repository/package.h"
#include <vector>
#include <yaml-cpp/yaml.h>

package::PKGTypeENUM getPKGType(const char * type);

int loadDownloadList(std::vector<download*> * downloadList, YAML::Node downloadsFile);
int loadSavedRepos(const std::shared_ptr<std::vector<repository*>>& repositoryList);
std::string getDefaultIconPath(package::PKGTypeENUM type);
int loadPackagesFromRepo(repository* repository);
repository* fetchRepo(const char *repoURLStr);
#endif //CYDI4_REPOFETCHER_H
