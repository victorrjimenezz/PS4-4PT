//
// Created by Víctor Jiménez Rugama on 12/26/21.
//

#ifndef GRAPHICS_USES_FONT
#define GRAPHICS_USES_FONT 1
#endif

#include "../../include/utils/AppGraphics.h"
#include "../../include/view/keyboardInput.h"
#include "../../include/utils/logger.h"

#include <string>
#include <locale>
#include <codecvt>


keyboardInput::keyboardInput(Scene2D * mainScene, FT_Face font, int x, int y, int width, int height, const char * keyboardTitle, const char * placeHolderText, OrbisImeType keyboardType, OrbisButtonLabel buttonLabel) {
    this->mainScene = mainScene;
    text = placeHolderText;
    this->font = font;

    this->borderColor = {180,180,180};
    this->bgColor = {255,255,255};
    this->textColor = {0,0,0};

    this->containerX = x;
    this->containerY=y-height;
    this->containerWidth = width;
    this->containerHeight = height;

    this->boxWidth = width * (1 - KEYBOARD_HORIZONTAL_BORDER_SIZE);
    this->boxHeight = height  * (1 - 2*KEYBOARD_VERTICAL_BORDER_SIZE);

    int borderSize=(width-boxWidth)/2;
    this->boxX = x+borderSize;

    this->yInputBox = y - height + height*KEYBOARD_VERTICAL_BORDER_SIZE;

    this->xText = boxX+borderSize+boxWidth*KEYBOARD_X_PAD;
    this->yInputText = y - containerHeight/2+ height*KEYBOARD_VERTICAL_BORDER_SIZE;

    this->entered = false;


    memset(&buffer[0],0,sizeof(buffer));

    memset(&param, 0, sizeof(OrbisImeDialogSetting));

    std::string title = keyboardTitle;
    text = placeHolderText;
    // UTF8 -> UTF16
    title_u16 = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(title.data());
    std::u16string initial_text_u16 = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(text.data());
    memcpy(&buffer[0],initial_text_u16.c_str(),sizeof(char16_t) * initial_text_u16.size());

    param.type = ORBIS_TYPE_BASIC_LATIN;

    param.option = 0;
    param.userId = 0xFE;
    param.horizontalAlignment = ORBIS_H_CENTER;
    param.verticalAlignment = ORBIS_V_CENTER;
    param.type = keyboardType;

    param.title = (const uint16_t *)title_u16.c_str();
    param.maxTextLength = KEYBOARD_MAX_INPUT_LENGTH;
    param.inputTextBuffer = buffer;
    param.enterLabel = buttonLabel;

    memset(&result, 0, sizeof(OrbisDialogResult));

    status = ORBIS_DIALOG_STATUS_NONE;
}

void keyboardInput::updateView() {
    if(active()) {
        status = sceImeDialogGetStatus();
        sceImeDialogGetResult(&result);
        if (status != ORBIS_DIALOG_STATUS_RUNNING){
            if (result.endstatus == ORBIS_DIALOG_OK) {
                std::u16string buffer_u16 = (char16_t *) buffer;
                text = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(buffer_u16.data());
                entered = true;
            }
            disableKeyboard();
        }
    }

    mainScene->DrawRectangle(containerX, containerY, containerWidth, containerHeight, borderColor);
    mainScene->DrawRectangle(boxX, yInputBox, boxWidth, boxHeight, bgColor);
    mainScene->DrawText((char*)text.c_str(),font,xText,yInputText,textColor,textColor);
}


void keyboardInput::enableKeyboard() {
    if(status != ORBIS_DIALOG_STATUS_RUNNING) {
        entered = false;
        memset(&result, 0, sizeof(OrbisDialogResult));
        sceImeDialogInit(&param, NULL);
        status = ORBIS_DIALOG_STATUS_RUNNING;
    }
}
void keyboardInput::disableKeyboard() {
    sceImeDialogTerm();
    status = ORBIS_DIALOG_STATUS_NONE;
    std::u16string oldText = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(text.data());
    memset(&buffer[0],0,sizeof(buffer));
    memcpy(&buffer[0],oldText.c_str(),sizeof(char16_t)*oldText.size());
}
bool keyboardInput::hasEntered() const {
    return entered;
}
std::string keyboardInput::readText() {
    entered = false;
    return text;
}

keyboardInput::~keyboardInput() {
    disableKeyboard();
}

bool keyboardInput::active() {
    return status == ORBIS_DIALOG_STATUS_RUNNING;
}



