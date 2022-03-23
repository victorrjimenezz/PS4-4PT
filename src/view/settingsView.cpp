//
// Created by Víctor Jiménez Rugama on 2/8/22.
//
#include "../../include/view/settingsView.h"
#include "../../include/base.h"
#include "../../include/file/fileManager.h"
#include "../../include/utils/PNG.h"
#include "../../include/utils/logger.h"
#include "../../include/utils/LANG.h"
#include "../../include/utils/settings.h"
#include "../../include/main.h"


settingsView::settingsView(Scene2D *mainScene, FT_Face fontLarge, FT_Face fontMedium, FT_Face fontSmall, int frameWidth, int frameHeight) : fontLarge(fontLarge), fontMedium(fontMedium), fontSmall(fontSmall){
    this->mainScene = mainScene;
    this->frameWidth = frameWidth;
    this->frameHeight = frameWidth;
    flagTextPosX = frameWidth * SETTINGS_VIEW_FLAG_TEXT_X_PAD;
    textPosY = frameHeight * SETTINGS_VIEW_FLAG_Y_PAD;
    flagPosY = textPosY + SETTINGS_VIEW_FLAG_Y_PAD_IMAGE;

    int flagWidth = (int)((double)frameWidth * SETTINGS_VIEW_FLAG_WIDTH);
    int flagHeight = (int)((double)2/3 * flagWidth);
    for(auto language : lsDir(LANG_PATH)){
        flag tempFlag;
        std::string lang = language.substr(0,language.find_last_of('.'));
        tempFlag.lang = lang;
        lang = LANG_FLAG_PATH+lang+".png";
        tempFlag.flagIcon = new PNG(lang.c_str(),flagWidth,flagHeight);
        flags.emplace_back(tempFlag);
    }

    textColor = {0,0,0};

    selectedBorder = new PNG(LANG_FLAG_PATH "boxSelectedThin.png",flagWidth,flagHeight);
    selected = 0;

    langText = getMainLang()->LANGUAGE;

    flagBaseX = flagTextPosX + (int)(double)((double)langText.size() * (double)SETTINGS_VIEW_FLAG_CHAR_TEXT_OFFSET);
    flagSpacingX = (frameWidth-flagBaseX) / (flags.size()+1);




    checkboxColor = {255, 255, 255};
    checkboxEnabledColor = {255,0,0};
    notificationsXText = flagTextPosX;
    notificationsYText = frameHeight * SETTINGS_VIEW_NOTIFICATIONS_Y_PAD*2;
    notificationsY = notificationsYText + SETTINGS_VIEW_FLAG_Y_PAD_IMAGE;
    int notificationsOptionsDivider = getMainSettings()->getNotificationsOptions();
    notifications1X = notificationsXText + frameWidth/notificationsOptionsDivider * 0.1;
    notifications2X = notificationsXText + 1*frameWidth/notificationsOptionsDivider;
    notifications3X = notificationsXText + 2*frameWidth/notificationsOptionsDivider;
    checkboxBorder = new PNG(DATA_PATH "assets/images/filters/boxSelected.png", SETTINGS_CHECKBOX_WIDTH, SETTINGS_CHECKBOX_HEIGHT);
    checkboxCross = new PNG(DATA_PATH "assets/images/filters/cross.png",SETTINGS_CHECKBOX_WIDTH,SETTINGS_CHECKBOX_HEIGHT);
    checkboxSelectedCross = new PNG(DATA_PATH "assets/images/filters/crossSelected.png",SETTINGS_CHECKBOX_WIDTH,SETTINGS_CHECKBOX_HEIGHT);


    settingsSecondRowX = flagTextPosX;
    settingsSecondRowY = frameHeight * SETTINGS_VIEW_NOTIFICATIONS_Y_PAD*1.25;


    currentRow = LANGROW;

}
void settingsView::updateView(){
    int selectedTemp = selected, currFlag = 0;
    std::string messageString;
    rows currRowTemp = currentRow;
    mainScene->DrawText((char *) langText.c_str(), fontMedium, flagTextPosX, textPosY, textColor, textColor);

    for(const auto& flag : flags){
        flag.flagIcon->Draw(mainScene, flagBaseX + currFlag++ * flagSpacingX, flagPosY);
    }

    if(currRowTemp == LANGROW){
        int selectedFlagX = flagBaseX + selectedTemp * flagSpacingX;
        selectedBorder->Draw(mainScene, selectedFlagX, flagPosY);
    }


    mainScene->DrawText((char * ) getMainLang()->NOTIFICATIONS.c_str(),fontMedium,notificationsXText,notificationsYText,textColor,textColor);

    bool tempBool = getMainSettings()->isAddedToDownloadsNotification();
    {
        mainScene->DrawText((char *) getMainLang()->ADDED_TO_DOWNLOADS.c_str(), fontSmall, notifications1X+SETTINGS_CHECKBOX_WIDTH+5, notificationsY+SETTINGS_CHECKBOX_HEIGHT*0.9, textColor,
                            textColor);
        mainScene->DrawRectangle(notifications1X, notificationsY, SETTINGS_CHECKBOX_WIDTH, SETTINGS_CHECKBOX_HEIGHT,
                                 checkboxColor);
        if (selectedTemp == 0 && currRowTemp==NOTIFICATIONROW) {
            if (tempBool)
                mainScene->DrawRectangle(notifications1X, notificationsY, SETTINGS_CHECKBOX_WIDTH, SETTINGS_CHECKBOX_HEIGHT,
                                         checkboxEnabledColor);
            else
                checkboxSelectedCross->Draw(mainScene, notifications1X, notificationsY);
        } else if (tempBool)
            checkboxCross->Draw(mainScene, notifications1X, notificationsY);
        checkboxBorder->Draw(mainScene, notifications1X, notificationsY);
    }

    tempBool = getMainSettings()->isFinishedDownloadingNotification();
    {
        mainScene->DrawText((char *) getMainLang()->FINISHED_DOWNLOADING.c_str(), fontSmall, notifications2X+SETTINGS_CHECKBOX_WIDTH+5, notificationsY+SETTINGS_CHECKBOX_HEIGHT*0.9, textColor,
                            textColor);
        mainScene->DrawRectangle(notifications2X, notificationsY, SETTINGS_CHECKBOX_WIDTH, SETTINGS_CHECKBOX_HEIGHT,
                                 checkboxColor);
        if (selectedTemp == 1 && currRowTemp == NOTIFICATIONROW) {
            if (tempBool)
                mainScene->DrawRectangle(notifications2X, notificationsY, SETTINGS_CHECKBOX_WIDTH, SETTINGS_CHECKBOX_HEIGHT,
                                         checkboxEnabledColor);
            else
                checkboxSelectedCross->Draw(mainScene, notifications2X, notificationsY);
        } else if (tempBool)
            checkboxCross->Draw(mainScene, notifications2X, notificationsY);
        checkboxBorder->Draw(mainScene, notifications2X, notificationsY);
    }

    tempBool = getMainSettings()->isFailedDownloadingNotification();
    {
        mainScene->DrawText((char *) getMainLang()->ERROR_WHEN_DOWNLOADING.c_str(), fontSmall, notifications3X+SETTINGS_CHECKBOX_WIDTH+5, notificationsY+SETTINGS_CHECKBOX_HEIGHT*0.9, textColor,
                            textColor);
        mainScene->DrawRectangle(notifications3X, notificationsY, SETTINGS_CHECKBOX_WIDTH, SETTINGS_CHECKBOX_HEIGHT,
                                 checkboxColor);
        if (selectedTemp == 2 && currRowTemp == NOTIFICATIONROW) {
            if (tempBool)
                mainScene->DrawRectangle(notifications3X, notificationsY, SETTINGS_CHECKBOX_WIDTH, SETTINGS_CHECKBOX_HEIGHT,
                                         checkboxEnabledColor);
            else
                checkboxSelectedCross->Draw(mainScene, notifications3X, notificationsY);
        } else if (tempBool)
            checkboxCross->Draw(mainScene, notifications3X, notificationsY);
        checkboxBorder->Draw(mainScene, notifications3X, notificationsY);
    }

    tempBool = getMainSettings()->shouldInstallDirectlyPS4();
    {
        messageString = getMainLang()->INSTALL_DIRECTLY_PS4 + ":\n\n";
        messageString += getMainLang()->INSTALL_DIRECTLY_PS4_DESC;
        mainScene->DrawText((char *) messageString.c_str(), fontSmall, settingsSecondRowX+SETTINGS_CHECKBOX_WIDTH+5, settingsSecondRowY+SETTINGS_CHECKBOX_HEIGHT*0.9, textColor,
                            textColor);
        mainScene->DrawRectangle(settingsSecondRowX, settingsSecondRowY, SETTINGS_CHECKBOX_WIDTH, SETTINGS_CHECKBOX_HEIGHT,
                                 checkboxColor);
        if (currRowTemp == SECONDSETTINGSROW) {
            if (tempBool)
                mainScene->DrawRectangle(settingsSecondRowX, settingsSecondRowY, SETTINGS_CHECKBOX_WIDTH, SETTINGS_CHECKBOX_HEIGHT,
                                         checkboxEnabledColor);
            else
                checkboxSelectedCross->Draw(mainScene, settingsSecondRowX, settingsSecondRowY);
        } else if (tempBool)
            checkboxCross->Draw(mainScene, settingsSecondRowX, settingsSecondRowY);
        checkboxBorder->Draw(mainScene, settingsSecondRowX, settingsSecondRowY);
    }

}
void settingsView::pressX(){
    if(getMainSettings()== nullptr)
        return;
    int selectedTEMP = selected;
    switch(currentRow){
        case LANGROW:
            getMainSettings()->setCurrLang(flags.at(selectedTEMP).lang);
            break;
        case SECONDSETTINGSROW:
            getMainSettings()->setInstallDirectlyPS4(!getMainSettings()->shouldInstallDirectlyPS4());
            break;
        case NOTIFICATIONROW:
            if(selected==0)
                getMainSettings()->setAddedToDownloadsNotification(!getMainSettings()->isAddedToDownloadsNotification());
            else if(selected == 1)
                getMainSettings()->setFinishedDownloadingNotification(!getMainSettings()->isFinishedDownloadingNotification());
            else if(selected == 2)
                getMainSettings()->setFailedDownloadingNotification(!getMainSettings()->isFailedDownloadingNotification());
            break;
    }
}
void settingsView::pressCircle(){ }
void settingsView::pressTriangle(){ }
void settingsView::pressSquare(){ }
void settingsView::arrowUp(){
    switch(currentRow){
        case SECONDSETTINGSROW:
            currentRow = LANGROW;
            selected = selected>=flags.size()? flags.size()-1 : selected;
            break;
        case NOTIFICATIONROW:
            currentRow = SECONDSETTINGSROW;
            break;
        case LANGROW:
        default:
            break;
    }

}
void settingsView::arrowDown(){

    switch(currentRow){
        case LANGROW:
            currentRow = SECONDSETTINGSROW;
            break;
        case SECONDSETTINGSROW:
            currentRow = NOTIFICATIONROW;
            selected = selected>=getMainSettings()->getNotificationsOptions() ? getMainSettings()->getNotificationsOptions()-1 : selected;
            break;
        case NOTIFICATIONROW:
        default:
            break;
    }
}

void settingsView::arrowRight(){
    switch(currentRow){
        case LANGROW:
            selected = (selected+1)%flags.size();
            break;
        case NOTIFICATIONROW:
            selected = (selected+1)%getMainSettings()->getNotificationsOptions();
            break;
        case SECONDSETTINGSROW:
        default:
            break;
    }
}
void settingsView::arrowLeft(){
    switch(currentRow){
        case LANGROW:
            selected = selected ==0 ? flags.size()-1 : selected-1;
            break;
        case NOTIFICATIONROW:
            selected = selected ==0 ? getMainSettings()->getNotificationsOptions()-1: selected-1;
            break;
        case SECONDSETTINGSROW:
        default:
            break;
    }
}

bool settingsView::isActive(){
    return true;
}

void settingsView::langChanged(){
    langText = getMainLang()->LANGUAGE;
}
subView * settingsView::getParent(){
    return nullptr;
}
subView * settingsView::getChild(){
    return nullptr;
}

settingsView::~settingsView() {
    for(const auto& flag : flags){
        delete flag.flagIcon;
    }
    flags.clear();
    delete selectedBorder;
    delete checkboxBorder;
    delete checkboxCross;
    delete checkboxSelectedCross;
}