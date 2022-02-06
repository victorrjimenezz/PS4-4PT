//
// Created by Víctor Jiménez Rugama on 1/29/22.
//

#ifndef INC_4PT_LANG_H
#define INC_4PT_LANG_H

#include <cstdint>
#include <cstddef>
#include <string>

class LANG{
private:
    static int32_t systemLanguage;
    int loadLangFrom(const std::string & langFile);
    int loadDefLang();
    void langChanged();
    int changeLangTo(std::string lang);
public:
    std::string FAILED_TO_DOWNLOAD_PKG_FROM;
    std::string FINISHED_DOWNLOADING;
    std::string ERROR_WHEN_DOWNLOADING;
    std::string ERROR_WHEN_INSTALLING_APP;
    std::string GAME_CONTENT;
    std::string PATCH;
    std::string HAS_FAILED;
    std::string DOWNLOADED;
    std::string PENDING_DOWNLOAD;
    std::string HAS_FINISHED;
    std::string TYPE;
    std::string SOURCE;
    std::string DOWNLOAD_DATE;
    std::string VERSION;
    std::string WELCOME_MESSAGE;
    std::string DEVELOPED_BY;
    std::string PKG_DIRECT_DOWNLOAD;
    std::string INVALID_URL;
    std::string UPDATE_AVAILABLE;
    std::string INSTALLED;
    std::string SIZE;
    std::string NO_REPO_FOUND_AT;
    std::string REPO_ALREADY_LOADED;
    std::string ADD_REPO;
    std::string DOWNLOAD;
    std::string SEARCH;
    std::string SPACE;
    std::string FOR_FW;
    std::string ONLY_UPDATES;
    std::string ORDER;
    std::string FILTER;
    std::string ALPHABETICALLY;
    std::string ASCENDING;
    std::string DESCENDING;
    static LANG * mainLang;
    explicit LANG();
    static int initLang();
    int loadLang();
    static int termLang();
};
#endif //INC_4PT_LANG_H
