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
#include <yaml-cpp/yaml.h>

int32_t LANG::systemLanguage = 0;
LANG * LANG::mainLang = nullptr;

int LANG::changeLangTo(std::string lang){
    int loadLangRet;
    std::string langFile = LANG_PATH;
    try {
        langFile = langFile+lang+".yml";
        loadLangRet = loadLangFrom(langFile);
    } catch(const YAML::ParserException& ex){
        LOG << ex.what();
        loadLangRet= -1;
    }
    if(loadLangRet<0)
        loadDefLang();
    return loadLangRet;
}
int LANG::loadLang(){
    int ret = 1;
    sceSystemServiceParamGetInt(ORBIS_SYSTEM_SERVICE_PARAM_ID_LANG, &systemLanguage);

    switch (systemLanguage){
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
    FAILED_TO_DOWNLOAD_PKG_FROM = ("Failed to Download PKG From:\n");
    FINISHED_DOWNLOADING = ("Finished downloading:\n");
    ERROR_WHEN_DOWNLOADING = ("Error when downloading:\n");
    ERROR_WHEN_INSTALLING_APP = ("Error when installing app:\n");
    GAME_CONTENT = ("Game Content");
    PATCH = ("Patch");
    HAS_FAILED = ("Has Failed");
    DOWNLOADED = ("Downloaded");
    PENDING_DOWNLOAD = ("Pending download...");
    HAS_FINISHED = ("Has finished");
    TYPE = ("Type: ");
    SOURCE = ("Source: ");
    DOWNLOAD_DATE = ("Download Date: ");
    VERSION = ("Version: ");
    WELCOME_MESSAGE = ("Welcome .user. to 4PT\n ") + VERSION;
    DEVELOPED_BY = ("Developed by: ");
    PKG_DIRECT_DOWNLOAD = ("PKG Direct Download:");
    INVALID_URL = ("INVALID URL");
    UPDATE_AVAILABLE = ("Update Available");
    INSTALLED = ("Installed");
    SIZE = ("Size: ");
    NO_REPO_FOUND_AT = ("No repository found at: ");
    REPO_ALREADY_LOADED = ("Repository already loaded!");
    ADD_REPO = ("ADD REPO");
    DOWNLOAD = ("DOWNLOAD");
    SEARCH = ("SEARCH");
    SPACE = ("SPACE");
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
        return -1;
    }

    if(langFileYAML["ERROR_WHEN_DOWNLOADING"]){
        ERROR_WHEN_DOWNLOADING = langFileYAML["ERROR_WHEN_DOWNLOADING"].as<std::string>("");
    } else {
        return -1;
    }

    if(langFileYAML["ERROR_WHEN_INSTALLING_APP"]){
        ERROR_WHEN_INSTALLING_APP = langFileYAML["ERROR_WHEN_INSTALLING_APP"].as<std::string>("");
    } else {
        return -1;
    }
    if(langFileYAML["GAME_CONTENT"]){
        GAME_CONTENT = langFileYAML["GAME_CONTENT"].as<std::string>("");
    } else {
        return -1;
    }

    if(langFileYAML["PATCH"]){
        PATCH = langFileYAML["PATCH"].as<std::string>("");
    } else {
        return -1;
    }
    if(langFileYAML["HAS_FAILED"]){
        HAS_FAILED = langFileYAML["HAS_FAILED"].as<std::string>("");
    } else {
        return -1;
    }
    if(langFileYAML["DOWNLOADED"]){
        DOWNLOADED = langFileYAML["DOWNLOADED"].as<std::string>("");
    } else {
        return -1;
    }
    if(langFileYAML["PENDING_DOWNLOAD"]){
        PENDING_DOWNLOAD = langFileYAML["PENDING_DOWNLOAD"].as<std::string>("");
    } else {
        return -1;
    }
    if(langFileYAML["HAS_FINISHED"]){
        HAS_FINISHED = langFileYAML["HAS_FINISHED"].as<std::string>("");
    } else {
        return -1;
    }
    if(langFileYAML["TYPE"]){
        TYPE = langFileYAML["TYPE"].as<std::string>("");
    } else {
        return -1;
    }
    if(langFileYAML["SOURCE"]){
        SOURCE = langFileYAML["SOURCE"].as<std::string>("");
    } else {
        return -1;
    }
    if(langFileYAML["DOWNLOAD_DATE"]){
        DOWNLOAD_DATE = langFileYAML["DOWNLOAD_DATE"].as<std::string>("");
    } else {
        return -1;
    }
    if(langFileYAML["VERSION"]){
        VERSION = langFileYAML["VERSION"].as<std::string>("");
    } else {
        return -1;
    }
    if(langFileYAML["WELCOME_MESSAGE"]){
        WELCOME_MESSAGE = langFileYAML["WELCOME_MESSAGE"].as<std::string>("") + VERSION;
    } else {
        return -1;
    }
    if(langFileYAML["DEVELOPED_BY"]){
        DEVELOPED_BY = langFileYAML["DEVELOPED_BY"].as<std::string>("");
    } else {
        return -1;
    }
    if(langFileYAML["PKG_DIRECT_DOWNLOAD"]){
        PKG_DIRECT_DOWNLOAD = langFileYAML["PKG_DIRECT_DOWNLOAD"].as<std::string>("");
    } else {
        return -1;
    }
    if(langFileYAML["INVALID_URL"]){
        INVALID_URL = langFileYAML["INVALID_URL"].as<std::string>("");
    } else {
        return -1;
    }
    if(langFileYAML["UPDATE_AVAILABLE"]){
        UPDATE_AVAILABLE = langFileYAML["UPDATE_AVAILABLE"].as<std::string>("");
    } else {
        return -1;
    }
    if(langFileYAML["INSTALLED"]){
        INSTALLED = langFileYAML["INSTALLED"].as<std::string>("");
    } else {
        return -1;
    }
    if(langFileYAML["SIZE"]){
        SIZE = langFileYAML["SIZE"].as<std::string>("");
    } else {
        return -1;
    }
    if(langFileYAML["NO_REPO_FOUND_AT"]){
        NO_REPO_FOUND_AT = langFileYAML["NO_REPO_FOUND_AT"].as<std::string>("");
    } else {
        return -1;
    }
    if(langFileYAML["REPO_ALREADY_LOADED"]){
        REPO_ALREADY_LOADED = langFileYAML["REPO_ALREADY_LOADED"].as<std::string>("");
    } else {
        return -1;
    }
    if(langFileYAML["ADD_REPO"]){
        ADD_REPO = langFileYAML["ADD_REPO"].as<std::string>("");
    } else {
        return -1;
    }
    if(langFileYAML["DOWNLOAD"]){
        DOWNLOAD = langFileYAML["DOWNLOAD"].as<std::string>("");
    } else {
        return -1;
    }
    if(langFileYAML["SEARCH"]){
        SEARCH = langFileYAML["SEARCH"].as<std::string>("");
    } else {
        return -1;
    }
    if(langFileYAML["SPACE"]){
        SPACE = langFileYAML["SPACE"].as<std::string>("");
    } else {
        return -1;
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

