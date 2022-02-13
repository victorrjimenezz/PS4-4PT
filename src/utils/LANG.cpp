//
// Created by Víctor Jiménez Rugama on 1/29/22.
//
#include "../../include/utils/LANG.h"
#include "../../include/base.h"
#include "../../include/file/fileManager.h"
#include "../../include/view/subView.h"
#include "../../include/utils/logger.h"
#include "../../include/ControllerManager.h"

#include <orbis/SystemService.h>
#include <string>
#include "../../include/utils/settings.h"
#include <yaml-cpp/yaml.h>

int32_t LANG::systemLanguage = 0;
LANG * LANG::mainLang = nullptr;

int LANG::changeLangTo(const std::string& lang){
    int loadLangRet;
    std::string langFile = LANG_PATH;
    langFile = langFile+lang+".yml";
    if(!fileExists(langFile.c_str()))
        return 1;
    try {
        loadLangRet = loadLangFrom(langFile);
    } catch(const YAML::ParserException& ex){
        LOG << ex.what();
        loadLangRet= -1;
    }
    if(loadLangRet<0) {
        LOG << "Could not load lang, missing code: " << loadLangRet;
        loadDefLang();
    }
    return loadLangRet;
}
int LANG::loadLang(){
    int ret = 1;
    sceSystemServiceParamGetInt(ORBIS_SYSTEM_SERVICE_PARAM_ID_LANG, &systemLanguage);
    if(settings::getMainSettings() != nullptr) {
        ret = changeLangTo(settings::getMainSettings()->getCurrLang());
        if (ret == 0) {
            langChanged();
            return 0;
        }
    }
    switch (systemLanguage){
        case ORBIS_SYSTEM_PARAM_LANG_HUNGARIAN:
            LOG << "DETECTED HUNGARIAN";
            ret= changeLangTo("HU");
            break;
        case ORBIS_SYSTEM_PARAM_LANG_GERMAN:
            LOG << "DETECTED GERMAN";
            ret= changeLangTo("DE");
            break;
        case ORBIS_SYSTEM_PARAM_LANG_SPANISH_LA:
        case ORBIS_SYSTEM_PARAM_LANG_SPANISH:
            LOG << "DETECTED SPANISH";
            ret= changeLangTo("ES");
            break;
       case ORBIS_SYSTEM_PARAM_LANG_ITALIAN:
            LOG << "DETECTED ITALIAN";
            ret= changeLangTo("IT");
            break;
        case ORBIS_SYSTEM_PARAM_LANG_ENGLISH_GB:
        case ORBIS_SYSTEM_PARAM_LANG_ENGLISH_US:
            LOG << "DETECTED ENGLISH";
            ret= changeLangTo("EN");
            break;
        default:
            break;
    }
    if(ret==0)
        langChanged();
    return 0;
}

void LANG::langChanged(){
    int subViewSize = ControllerManager::getControllerManager()->getSubViews();
    subView * subView;
    for(int i = 0; i<subViewSize; i++) {
        subView = ControllerManager::getControllerManager()->getSubViewAt(i);
        if(subView== nullptr)
            continue;
        subView->langChanged();
    }
}

int LANG::loadDefLang(){
    FAILED_TO_DOWNLOAD_PKG_FROM = ("Failed to Download PKG From");
    FINISHED_DOWNLOADING = ("Finished downloading");
    ERROR_WHEN_DOWNLOADING = ("Error when downloading");
    ERROR_WHEN_INSTALLING_APP = ("Error when installing app:\n");
    GAME_CONTENT = ("Game Content");
    PATCH = ("Patch");
    HAS_FAILED = ("Has Failed");
    DOWNLOADED = ("Downloaded ");
    PENDING_DOWNLOAD = ("Pending download...");
    HAS_FINISHED = ("Has finished");
    TYPE = ("Type: ");
    SOURCE = ("Source: ");
    DOWNLOAD_DATE = ("Download Date: ");
    VERSION = ("Version");
    WELCOME_MESSAGE = ("Welcome .user. to 4PT\n ") + VERSION;
    DEVELOPED_BY = ("Developed by: ");
    PKG_DIRECT_DOWNLOAD = ("PKG Direct Download:");
    INVALID_URL = ("INVALID URL");
    UPDATE_AVAILABLE = ("Update Available");
    INSTALLED = ("Installed");
    SIZE = ("Size");
    NO_REPO_FOUND_AT = ("No repository found at: ");
    REPO_ALREADY_LOADED = ("Repository already loaded!");
    ADD_REPO = ("ADD REPO");
    DOWNLOAD = ("DOWNLOAD");
    SEARCH = ("SEARCH");
    SPACE = ("SPACE");
    FOR_FW = ("For Firmware: ");
    ONLY_UPDATES = ("Only Updates");
    ORDER = ("Order: ");
    FILTER = ("Filter: ");
    ALPHABETICALLY = ("Alphabetically");
    ASCENDING = ("Ascending");
    DESCENDING = ("Descending");
    LANGUAGE = ("Language: ");
    NOTIFICATIONS = ("Notifications: ");
    ADDED_TO_DOWNLOADS = ("Added to downloads");
    INSTALL_DIRECTLY_PS4 = ("Install Directly to PS4");
    INSTALL_DIRECTLY_PS4_DESC = ("Allows playing games while downloading and only stores one copy of the app\nDisables ability to pause/resume download");
    return 0;
}

int LANG::initLang() {
    if(LANG::mainLang!= nullptr)
        return 1;
    LANG::mainLang = new LANG();
    return 0;
}

int LANG::loadLangFrom(const std::string& langFile) {
    YAML::Node langFileYAML = YAML::LoadFile(langFile);
    if(langFileYAML["FAILED_TO_DOWNLOAD_PKG_FROM"]){
        FAILED_TO_DOWNLOAD_PKG_FROM = langFileYAML["FAILED_TO_DOWNLOAD_PKG_FROM"].as<std::string>("");
    } else {
        return -1;
    }

    if(langFileYAML["FINISHED_DOWNLOADING"]){
        FINISHED_DOWNLOADING = langFileYAML["FINISHED_DOWNLOADING"].as<std::string>("");
    } else {
        return -2;
    }

    if(langFileYAML["ERROR_WHEN_DOWNLOADING"]){
        ERROR_WHEN_DOWNLOADING = langFileYAML["ERROR_WHEN_DOWNLOADING"].as<std::string>("");
    } else {
        return -3;
    }

    if(langFileYAML["ERROR_WHEN_INSTALLING_APP"]){
        ERROR_WHEN_INSTALLING_APP = langFileYAML["ERROR_WHEN_INSTALLING_APP"].as<std::string>("");
    } else {
        return -4;
    }
    if(langFileYAML["GAME_CONTENT"]){
        GAME_CONTENT = langFileYAML["GAME_CONTENT"].as<std::string>("");
    } else {
        return -5;
    }

    if(langFileYAML["PATCH"]){
        PATCH = langFileYAML["PATCH"].as<std::string>("");
    } else {
        return -6;
    }
    if(langFileYAML["HAS_FAILED"]){
        HAS_FAILED = langFileYAML["HAS_FAILED"].as<std::string>("");
    } else {
        return -7;
    }
    if(langFileYAML["DOWNLOADED"]){
        DOWNLOADED = langFileYAML["DOWNLOADED"].as<std::string>("");
    } else {
        return -8;
    }
    if(langFileYAML["PENDING_DOWNLOAD"]){
        PENDING_DOWNLOAD = langFileYAML["PENDING_DOWNLOAD"].as<std::string>("");
    } else {
        return -9;
    }
    if(langFileYAML["HAS_FINISHED"]){
        HAS_FINISHED = langFileYAML["HAS_FINISHED"].as<std::string>("");
    } else {
        return -10;
    }
    if(langFileYAML["TYPE"]){
        TYPE = langFileYAML["TYPE"].as<std::string>("");
    } else {
        return -11;
    }
    if(langFileYAML["SOURCE"]){
        SOURCE = langFileYAML["SOURCE"].as<std::string>("");
    } else {
        return -12;
    }
    if(langFileYAML["DOWNLOAD_DATE"]){
        DOWNLOAD_DATE = langFileYAML["DOWNLOAD_DATE"].as<std::string>("");
    } else {
        return -13;
    }
    if(langFileYAML["VERSION"]){
        VERSION = langFileYAML["VERSION"].as<std::string>("");
    } else {
        return -14;
    }
    if(langFileYAML["WELCOME_MESSAGE"]){
        WELCOME_MESSAGE = langFileYAML["WELCOME_MESSAGE"].as<std::string>("") + VERSION;
    } else {
        return -15;
    }
    if(langFileYAML["DEVELOPED_BY"]){
        DEVELOPED_BY = langFileYAML["DEVELOPED_BY"].as<std::string>("");
    } else {
        return -16;
    }
    if(langFileYAML["PKG_DIRECT_DOWNLOAD"]){
        PKG_DIRECT_DOWNLOAD = langFileYAML["PKG_DIRECT_DOWNLOAD"].as<std::string>("");
    } else {
        return -17;
    }
    if(langFileYAML["INVALID_URL"]){
        INVALID_URL = langFileYAML["INVALID_URL"].as<std::string>("");
    } else {
        return -18;
    }
    if(langFileYAML["UPDATE_AVAILABLE"]){
        UPDATE_AVAILABLE = langFileYAML["UPDATE_AVAILABLE"].as<std::string>("");
    } else {
        return -19;
    }
    if(langFileYAML["INSTALLED"]){
        INSTALLED = langFileYAML["INSTALLED"].as<std::string>("");
    } else {
        return -20;
    }
    if(langFileYAML["SIZE"]){
        SIZE = langFileYAML["SIZE"].as<std::string>("");
    } else {
        return -21;
    }
    if(langFileYAML["NO_REPO_FOUND_AT"]){
        NO_REPO_FOUND_AT = langFileYAML["NO_REPO_FOUND_AT"].as<std::string>("");
    } else {
        return -22;
    }
    if(langFileYAML["REPO_ALREADY_LOADED"]){
        REPO_ALREADY_LOADED = langFileYAML["REPO_ALREADY_LOADED"].as<std::string>("");
    } else {
        return -23;
    }
    if(langFileYAML["ADD_REPO"]){
        ADD_REPO = langFileYAML["ADD_REPO"].as<std::string>("");
    } else {
        return -24;
    }
    if(langFileYAML["DOWNLOAD"]){
        DOWNLOAD = langFileYAML["DOWNLOAD"].as<std::string>("");
    } else {
        return -25;
    }
    if(langFileYAML["SEARCH"]){
        SEARCH = langFileYAML["SEARCH"].as<std::string>("");
    } else {
        return -26;
    }
    if(langFileYAML["SPACE"]){
        SPACE = langFileYAML["SPACE"].as<std::string>("");
    } else {
        return -27;
    }
    if(langFileYAML["FOR_FW"]){
        FOR_FW = langFileYAML["FOR_FW"].as<std::string>("");
    } else {
        return -28;
    }
    if(langFileYAML["ONLY_UPDATES"]){
        ONLY_UPDATES = langFileYAML["ONLY_UPDATES"].as<std::string>("");
    } else {
        return -29;
    }
    if(langFileYAML["ORDER"]){
        ORDER = langFileYAML["ORDER"].as<std::string>("");
    } else {
        return -30;
    }
    if(langFileYAML["FILTER"]){
        FILTER = langFileYAML["FILTER"].as<std::string>("");
    } else {
        return -31;
    }
    if(langFileYAML["ALPHABETICALLY"]){
        ALPHABETICALLY = langFileYAML["ALPHABETICALLY"].as<std::string>("");
    } else {
        return -32;
    }
    if(langFileYAML["ASCENDING"]){
        ASCENDING = langFileYAML["ASCENDING"].as<std::string>("");
    } else {
        return -33;
    }
    if(langFileYAML["DESCENDING"]){
        DESCENDING = langFileYAML["DESCENDING"].as<std::string>("");
    } else {
        return -34;
    }
    if(langFileYAML["LANGUAGE"]){
        LANGUAGE = langFileYAML["LANGUAGE"].as<std::string>("");
    } else {
        return -35;
    }
    if(langFileYAML["NOTIFICATIONS"]){
        NOTIFICATIONS = langFileYAML["NOTIFICATIONS"].as<std::string>("");
    } else {
        return -36;
    }
    if(langFileYAML["ADDED_TO_DOWNLOADS"]){
        ADDED_TO_DOWNLOADS = langFileYAML["ADDED_TO_DOWNLOADS"].as<std::string>("");
    } else {
        return -37;
    }
    if(langFileYAML["INSTALL_DIRECTLY_PS4"]){
        INSTALL_DIRECTLY_PS4 = langFileYAML["INSTALL_DIRECTLY_PS4"].as<std::string>("");
    } else {
        return -38;
    }
    if(langFileYAML["INSTALL_DIRECTLY_PS4_DESC"]){
        INSTALL_DIRECTLY_PS4_DESC = langFileYAML["INSTALL_DIRECTLY_PS4_DESC"].as<std::string>("");
    } else {
        return -39;
    }

    return 0;
}

LANG::LANG() {
    loadDefLang();
}

int LANG::termLang() {
    delete LANG::mainLang;
    mainLang= nullptr;
    return 0;
}

