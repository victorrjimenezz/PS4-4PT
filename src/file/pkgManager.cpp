#include <cstdarg>
#include "../../_common/notifi.h"
#include "../../include/utils/logger.h"
#include "../../include/file/download.h"

//Include Orbis Dependencies
#include <orbis/AppInstUtil.h>
#include <orbis/Bgft.h>


int PKG_ERROR(const char* name, int ret){
    notifi(NULL,"Install Failed with codeHEX: %containerX Int: %ifrom Function %s", ret, ret, name);
    return ret;
}

uint8_t pkginstall(download * download) {
    char title_id[16];
    int  is_app, ret = -1;
    int  task_id = -1;
    char buffer[255];
    const char * path = download->getPath();

    ret = sceAppInstUtilGetTitleIdFromPkg(path, title_id, &is_app);
    if (ret)
        return PKG_ERROR("sceAppInstUtilGetTitleIdFromPkg", ret);

    snprintf(buffer, 254, "%s via 4PT", title_id);
    OrbisBgftDownloadParamEx download_params;
    memset(&download_params, 0, sizeof(download_params));
    download_params.params.entitlementType = 5;
    download_params.params.id = title_id;
    download_params.params.contentUrl = path;
    download_params.params.contentName = buffer;
    download_params.params.iconPath = download->getIconPath();
    download_params.params.playgoScenarioId = "0";
    download_params.params.option = ORBIS_BGFT_TASK_OPT_DELETE_AFTER_UPLOAD;
    download_params.slot = 0;


    retry:
    ret = sceBgftServiceIntDownloadRegisterTaskByStorageEx(&download_params, &task_id);

    if(ret == 0x80990088) {
        ret = sceAppInstUtilAppUnInstall(&title_id[0]);

        if(ret != 0)
            return PKG_ERROR("sceAppInstUtilAppUnInstall", ret);

        goto retry;
    }
    else
    if(ret)
        return PKG_ERROR("sceBgftServiceIntDownloadRegisterTaskByStorageEx", ret);

    ret = sceBgftServiceDownloadStartTask(task_id);
    if(ret)
        return PKG_ERROR("sceBgftDownloadStartTask", ret);
    return 0;
}