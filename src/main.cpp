//C++ Libs
#include <sstream>
#include <iostream>
#include <thread>

//Orbis Library Definitions
#ifndef GRAPHICS_USES_FONT
#define GRAPHICS_USES_FONT 1
#endif

//Include Headers
#include "../include/utils/utils.h"
#include "../include/file/fileManager.h"
#include "../include/file/fileDownloadRequest.h"
#include "../include/view/mainView.h"
#include "../include/utils/logger.h"
#include "../include/utils/Updater.h"
#include "../include/utils/dialog.h"
#include "../include/utils/notifi.h"
#include "../include/utils/LANG.h"
#include "../include/utils/AudioManager.h"
#include "../include/utils/threadPool.h"

//Load Orbis dependencies
#include <orbis/Sysmodule.h>
#include <orbis/AppInstUtil.h>
#include <orbis/Bgft.h>
#include <orbis/UserService.h>
#include <orbis/SystemService.h>
#include <orbis/Ssl.h>
#include <orbis/Http.h>
#include <orbis/Net.h>
#include <orbis/NetCtl.h>
#include <orbis/CommonDialog.h>
#include <orbis/MsgDialog.h>

#include <csignal>


void SignalHandler(int signal);

void SignalInit();

std::stringstream debugLogStream;
mainView * mainView = nullptr;

int libSslId = -1;
int libNetMemId = -1;

int initializeApp();
int loadModules();
int startProcesses();
int bgftInit();
int networkInit();
int mkDirs();
std::string unique_log_name();
int openLogger();
int checkForUpdate();
int migrate();

void exitApp();
void stopProcesses();
void networkShutDown();
void unloadModules();
void closeLogger();

int main() {
    SignalInit();
    atexit(&exitApp);
    at_quick_exit(&exitApp);

    bool isFirstRun = false;
    int ret = initializeApp();

    if(ret < 0){
        notifi(NULL,"ERROR WHILE INITIALIZING APP!");
        LOG << "ERROR WHILE INITIALIZING APP!" << "\n";
        exit(0);
    } else if(ret == 1)
        isFirstRun = true;


    mainView = new class mainView(isFirstRun);

    LOG << "Initialized Main view" << "\n";

    sceSystemServiceHideSplashScreen();

    LOG << "Checking for Update";

    checkForUpdate();
    LOG << "Initialized APP!";

    LOG << "LOADING LANG";

    LANG::mainLang->loadLang();

    LOG << "LOADED LANG";

    try {
        // Draw loop
        while (mainView->updateView() >= 0) continue;
    } catch (const std::exception &exception) {
        LOG << "FATAL ERROR:\n" << exception.what();
    }

    delete mainView;
    exitApp();
    return 0;
}
std::string unique_log_name(){
    auto now = std::chrono::system_clock::now();
    time_t currentTime = std::chrono::system_clock::to_time_t(now);
    tm currentLocalTime = *localtime(&currentTime);

    std::ostringstream buffer;
    buffer << currentLocalTime.tm_year+1900;
    buffer << "-" <<currentLocalTime.tm_mon+1;
    buffer << "-" << currentLocalTime.tm_mday;
    buffer << "_" << currentLocalTime.tm_hour;
    buffer << "." << currentLocalTime.tm_min;
    buffer << "." << currentLocalTime.tm_sec;
    buffer << ".log";

    return buffer.str();
}

int initializeApp() {
    int ret;
    jailbreak();

    if(!is_jailbroken()) {
        notifi(NULL,"ERROR: COULD NOT GAIN PERMISSIONS");
        return -1;
    }

    if(seteuid(0)) {
        notifi(NULL,"ERROR: Unable to acquire root permissions");
        return -1;
    }

    ret = mkDirs();
    if(ret<0) {
        return -1;
    }

    if(openLogger() < 0) {
        notifi(NULL,"ERROR: COULD NOT START LOGGER");
        return -1;
    }

    LOG << "Started Logger";

    if(loadModules()< 0) {
        LOG << "ERROR: COULD NOT LOAD MODULES";
        return -1;
    }
    LOG << "Loaded Modules" << "\n";

    if(startProcesses()< 0) {
        LOG << "ERROR: COULD NOT START PROCESSES";
        return -1;
    }
    LOG << "Started Processes" << "\n";

    srand (time(NULL));
    LOG << "Set srand" << "\n";

    if(AudioManager::initAudioManager() <0)
        LOG << "Could not initialize Audio Manager";


    LOG << "Starting Lang";
    LANG::initLang();
    LOG <<"Started Lang";

    if(migrate() < 0)
        LOG << "Could not migrate files";

    if(threadPool::init() < 0)
        LOG << "Could not init Thread Pool";

    return ret;
}

int checkForUpdate(){
    int ret = Updater::checkForUpdate();
    if(ret == -2)
        popDialog("Failed to load Update file");
    else if(ret == -3)
        popDialog("Failed to install update");
    else if(ret == -4) {
        popDialog("Failed to uninstall old app version.\nPlease uninstall it manually");
        sceSystemServiceLoadExec("exit",NULL);
    }
    return 0;
}

int loadModules() {
    if(sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_SYSTEM_SERVICE)<0)
        return -1;
    if(sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_APP_INST_UTIL)<0)
        return -1;
    if(sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_BGFT)<0)
        return -1;
    if(sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_USER_SERVICE)<0)
        return -1;
    if(sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_NETCTL)<0)
        return -1;
    if(sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_NET)<0)
        return -1;
    if(sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_HTTP)<0)
        return -1;
    if(sceSysmoduleLoadModule(ORBIS_SYSMODULE_MESSAGE_DIALOG) < 0)
        return -1;
    if(sceSysmoduleLoadModuleInternal(ORBIS_SYSMODULE_INTERNAL_SSL))
        return -1;
    return 0;
}

int startProcesses() {
    if(sceAppInstUtilInitialize())
        return -1;
    int ret = bgftInit();
    if(ret!=0)
        return ret;
    ret = networkInit();
    if(ret!=0)
        return ret;
    sceCommonDialogInitialize();
    sceMsgDialogInitialize();
    return 0;
}

int bgftInit(){
    OrbisBgftInitParams bgftInitParams;
    memset(&bgftInitParams, 0, sizeof(bgftInitParams));
    {
        bgftInitParams.heapSize = BGFT_HEAP_SIZE;
        bgftInitParams.heap = (uint8_t*)malloc(bgftInitParams.heapSize);
        if (!bgftInitParams.heap) {
            notifi(NULL,"No memory for BGFT heap.");
            return -1;
        }
        memset(bgftInitParams.heap, 0, bgftInitParams.heapSize);
    }
    if(sceBgftServiceIntInit(&bgftInitParams))
        return -1;

    if(sceUserServiceInitialize(NULL))
        return -1;
    return 0;

}

int networkInit() {
    int ret = sceNetCtlInit();
    if (ret) {
        LOG << "sceNetCtlInit failed: ";
        return -1;
    }
    LOG << "Initialized NetworkCtl";
    ret = sceNetInit();
    if (ret) {
        LOG << "sceNetInit failed: ";
        return ret;
    }

    LOG << "Initialized Network";
    libNetMemId = sceNetPoolCreate("4PT", NET_HEAP_SIZE, 0);
    if (libNetMemId < 0) {
        LOG << "sceNetPoolCreate failed:";
        return libNetMemId;
    }
    LOG << "Created Network Pool";

    libSslId = sceSslInit(SSL_HEAP_SIZE);
    if(libSslId < 0) {
        LOG << "Ssl Initialization Failed;";
        return libSslId;
    }
    LOG << "Initialized Ssl at " << libSslId;

    int libhttpCtxId = sceHttpInit(libNetMemId, libSslId, HTTP_HEAP_SIZE);
    if(libhttpCtxId<0){
        LOG << "Http Initialization Failed";
        return libhttpCtxId;
    }

    fileDownloadRequest::setLibhttpCtxId(libhttpCtxId);
    LOG << "Initialized HTTP";

    return 0;
}
int openLogger() {
    std::string logPath = STORED_PATH;
    logPath+=LOGS_PATH;
    logPath+=unique_log_name();
    return logger::init(logPath.c_str());
}
int migrate() {
    int ret =0;
    if(fileExists(DOWNLOADS_PATH_OLD))
        ret = moveFile(DOWNLOADS_PATH_OLD,DOWNLOADS_PATH);


    if(folderExists(STORED_PATH_OLD REPO_PATH))
        ret = moveFile(STORED_PATH_OLD REPO_PATH,STORED_PATH REPO_PATH);

    return ret;
}
int mkDirs(){
    int ret = 0, tempRet;

    std::stringstream errorCode;
    errorCode << "Could Not Make Dirs\n";

    if(!folderExists(STORED_DATA_PATH)) {
        ret = 1;
        tempRet = mkDir(STORED_DATA_PATH);
        if (tempRet != 0) {
            ret = -1;
            errorCode << std::hex << ret;
            notifi(NULL,errorCode.str().c_str());
            goto err;
        }
    }

    if(!folderExists(STORED_PATH)) {
        ret = 1;
        tempRet = mkDir(STORED_PATH);
        if (tempRet != 0) {
            ret = -1;
            errorCode << std::hex << ret;
            notifi(NULL,errorCode.str().c_str());
            goto err;
        }
    }

    if(!folderExists(STORED_PATH REPO_PATH)) {
        ret = 1;
        tempRet = mkDir(STORED_PATH REPO_PATH);
        if (tempRet != 0) {
            ret = -1;
            errorCode << std::hex << ret;
            notifi(NULL,errorCode.str().c_str());
            goto err;
        }
    }
    if(!folderExists(STORED_PATH LOGS_PATH)) {
        tempRet = mkDir(STORED_PATH LOGS_PATH);
        if (tempRet != 0) {
            ret = -1;
            errorCode << std::hex << ret;
            notifi(NULL,errorCode.str().c_str());
            goto err;
        }
    }
    err:
    return ret;
}

void exitApp(){
    threadPool::term();
    LOG << "Terminated threadPool";

    stopProcesses();
    LOG << "Stopped Processes";

    unloadModules();
    LOG << "Unloaded modules";
    LOG << "Closing logger: Bye Bye!";

    closeLogger();
    unjailbreak();
    sceSystemServiceLoadExec("exit",NULL);
}
void stopProcesses() {
    sceAppInstUtilTerminate();
    sceBgftServiceIntTerm();
    sceUserServiceTerminate();
    networkShutDown();
    AudioManager::mainAudioManager->termAudioManager();
    LANG::termLang();
}
void networkShutDown() {
    sceHttpTerm(fileDownloadRequest::getLibhttpCtxId());
    if(libNetMemId>=0)
        sceNetPoolDestroy(libNetMemId);
    if(libSslId>=0)
        sceSslTerm(libSslId);
    sceNetTerm();
}
void unloadModules() {
    sceSysmoduleUnloadModule(ORBIS_SYSMODULE_MESSAGE_DIALOG);
    sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_SYSTEM_SERVICE);
    sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_APP_INST_UTIL);
    sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_BGFT);
    sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_USER_SERVICE);
    sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_HTTP);
    sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_NET);
    sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_NETCTL);
    sceSysmoduleUnloadModuleInternal(ORBIS_SYSMODULE_INTERNAL_SSL);
}
void closeLogger() {
    logger::closeLogger();
}
void SignalHandler(int signal) {
    LOG << "Received Signal: " << signal;
    delete mainView;
    mainView = nullptr;
    exit(0);
}
void SignalInit(){

    struct sigaction sigIntHandler{};

    sigIntHandler.sa_handler = SignalHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    for(int i = 1; i<=_NSIG; i++)
        sigaction(i,&sigIntHandler,NULL);

}
