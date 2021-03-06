//
// Created by Víctor Jiménez Rugama on 1/1/22.
//
#include "../../include/utils/dialog.h"
#include "../../include/utils/logger.h"
#include <orbis/CommonDialog.h>
#include <orbis/MsgDialog.h>

#include <string>

int popDialog(const char* message) {
    int ret = 0;

    sceMsgDialogInitialize();
    OrbisMsgDialogParam param;

    memset(&param, 0x0, sizeof(OrbisMsgDialogParam) );
    memset(&param.baseParam, 0x0, sizeof(OrbisCommonDialogBaseParam));

    param.baseParam.size = (uint32_t)sizeof(OrbisCommonDialogBaseParam);
    param.baseParam.magic = (uint32_t)( ORBIS_COMMON_DIALOG_MAGIC_NUMBER + (uint64_t)&param.baseParam );

    param.size = sizeof(OrbisMsgDialogParam);
    param.mode = ORBIS_MSG_DIALOG_MODE_USER_MSG;

    OrbisMsgDialogUserMessageParam userMsgParam;
    memset(&userMsgParam, 0, sizeof(OrbisMsgDialogUserMessageParam));
    userMsgParam.msg = message;
    userMsgParam.buttonType = ORBIS_MSG_DIALOG_BUTTON_TYPE_OK;
    param.userMsgParam = &userMsgParam;

    if (sceMsgDialogOpen(&param) < 0) {
        LOG << "Error When Popping Dialog";
        return -1;
    }


    OrbisCommonDialogStatus stat;

    stat = sceMsgDialogUpdateStatus();
    while (stat != ORBIS_COMMON_DIALOG_STATUS_FINISHED)
        stat = sceMsgDialogUpdateStatus();

    OrbisMsgDialogResult result;
    memset(&result, 0, sizeof(result));

    if (0 > sceMsgDialogGetResult(&result))
        ret = -2;

    sceMsgDialogTerminate();
    return ret;
}