//
// Created by Víctor Jiménez Rugama on 12/26/21.
//

#ifndef CYDI4_KEYBOARDINPUT_H
#define CYDI4_KEYBOARDINPUT_H

#ifndef GRAPHICS_USES_FONT
#define GRAPHICS_USES_FONT 1
#endif

#include "../utils/AppGraphics.h"
#include "../base.h"

#include <string>
#include <orbis/ImeDialog.h>

class keyboardInput {
private:
    Color borderColor{};
    Color bgColor{};
    Color textColor{};

    OrbisDialogStatus status;
    OrbisDialogResult result;
    OrbisImeDialogSetting param;
    uint16_t buffer[KEYBOARD_MAX_INPUT_LENGTH];

    bool entered;

    Scene2D * mainScene;
    int containerX;
    int containerY;
    int containerWidth;
    int yInputBox;

    int boxX;
    int boxHeight;
    int boxWidth;

    int xText;
    int yInputText;

    int containerHeight;

    FT_Face font{};

    std::string text;
    std::u16string title_u16;
    void disableKeyboard();
public:
    keyboardInput(Scene2D * mainScene, FT_Face font, int x, int y, int width, int height, const char * keyboardTitle = "Title", const char * placeHolderText = "Input Text...", OrbisImeType keyboardType = ORBIS_TYPE_BASIC_LATIN, OrbisButtonLabel buttonLabel = ORBIS_BUTTON_LABEL_SEARCH);
    void updateView();
    void enableKeyboard();
    bool hasEntered() const;
    std::string readText();
    bool active();
    ~keyboardInput();
};
#endif //CYDI4_KEYBOARDINPUT_H
