//C++ Libs
#include <sstream>
#include <iostream>
#include <thread>

//Orbis Library Definitions
#ifndef GRAPHICS_USES_FONT
#define GRAPHICS_USES_FONT 1
#endif
#define BGFT_HEAP_SIZE (1 * 1024 * 1024)
#define HTTP_HEAP_SIZE (1024 * 1024)
#define SSL_HEAP_SIZE (128 * 1024)
#define NET_HEAP_SIZE   (1 * 1024 * 1024)

//Common Headers
#include "../_common/graphics.h"
#include "../_common/log.h"
#include "../_common/notifi.h"

//Include Headers
#include "../include/utils/utils.h"
#include "../include/file/fileManager.h"
#include "../include/file/fileDownloadRequest.h"
#include "../include/view/mainView.h"
#include "../include/utils/logger.h"

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

std::stringstream debugLogStream;

int initializeApp();
int loadModules();
int startProcesses();
int bgftInit();
int networkInit();
int mkDirs();
std::string unique_log_name();
int openLogger();


void exitApp();
void stopProcesses();
void networkShutDown();
void unloadModules();
void closeLogger();

int main() {
    atexit(&exitApp);
    bool isFirstRun = false;
    int ret = initializeApp();
    if(ret < 0){
        notifi(NULL,"ERROR WHILE INITIALIZING APP!");
        LOG << "ERROR WHILE INITIALIZING APP!" << "\n";
        sceSystemServiceLoadExec("exit",NULL);
    } else if(ret == 1)
        isFirstRun = true;

    mainView mainView(isFirstRun);
    LOG << "Initialized Main view" << "\n";

    // Draw loop
    for (;;) {
        mainView.updateView();
    }

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

    if(!is_jailbroken())
        return -1;

    ret = mkDirs();
    if(ret<0)
        return -1;

    if(openLogger() < 0)
        return -1;

    if(loadModules()< 0)
        return -1;
    LOG << "Loaded Modules" << "\n";

    if(startProcesses()< 0)
        return -1;
    LOG << "Started Processes" << "\n";

    srand (time(NULL));
    LOG << "Set srand" << "\n";

    return ret;
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
    int libNetMemId = sceNetPoolCreate("4PT", NET_HEAP_SIZE, 0);
    if (libNetMemId < 0) {
        LOG << "sceNetPoolCreate failed:";
        return libNetMemId;
    }
    LOG << "Created Network Pool";

    /*int libSslId = sceSslInit(SSL_HEAP_SIZE);
    if(libSslId < 0) {
        LOG << "Ssl Initialization Failed;";
        return libSslId;
    }
    LOG << "Initialized Ssl at " << libSslId;*/

    int libhttpCtxId = sceHttpInit(libNetMemId, 0, HTTP_HEAP_SIZE);
    if(libhttpCtxId<0){
        LOG << "Http Initialization Failed";
        return libhttpCtxId;
    }

    LOG << "Initialized HTTP";
    fileDownloadRequest::setLibhttpCtxId(libhttpCtxId);
    return 0;
}
int openLogger() {
    std::string logPath = STORED_PATH;
    logPath+=LOGS_PATH;
    logPath+=unique_log_name();
    return logger::init(logPath.c_str());
}
int mkDirs(){
    int ret = 0;
    std::string repoPath = STORED_PATH;
    repoPath+=REPO_PATH;

    std::string logsPath = STORED_PATH;
    logsPath+=LOGS_PATH;
    if(!folderExists(repoPath.c_str())) {
        ret = 1;
        if (mkDir(repoPath.c_str()) != 0)
            return -1;
    }
    if(!folderExists(logsPath.c_str())) {
        if (mkDir(logsPath.c_str()) != 0)
            return -1;
        if(ret == 1)
            ret = 1;
    }
    return ret;
}

void exitApp(){
    stopProcesses();
    unloadModules();
    closeLogger();
    unjailbreak();
}
void stopProcesses() {
    sceAppInstUtilTerminate();
    sceBgftServiceIntTerm();
    sceUserServiceTerminate();
    networkShutDown();
}
void networkShutDown() {
    sceHttpTerm(fileDownloadRequest::getLibhttpCtxId());
    sceNetPoolDestroy();
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

