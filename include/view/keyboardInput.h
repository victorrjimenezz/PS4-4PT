//
// Created by Víctor Jiménez Rugama on 12/26/21.
//

#ifndef CYDI4_KEYBOARDINPUT_H
#define CYDI4_KEYBOARDINPUT_H

#ifndef GRAPHICS_USES_FONT
#define GRAPHICS_USES_FONT 1
#endif

#include "../../_common/graphics.h"
#include "../base.h"

class keyboardInput {
private:
    Color borderColor{};
    Color bgColor{};
    Color textColor{};
    Color selectedTextColor{};
    bool keyBoardIsSelected;
    bool isOnUpperRow;
    bool entered;
    bool changed;

    bool hasEnter;

    const char * specialKey;
    const char * del = "DEL";
    const char * enter;
    const char * space = "SPACE";
    int delPos;
    int specialPos;
    Scene2D * mainScene;
    int containerX;
    int containerY;
    int containerWidth;
    int yInputBox;
    int yInputText;
    int xText;

    int boxX;
    int boxHeight;
    int boxWidth;

    int containerHeight;
    int yKeyboardBox;
    int yKeyboardTextUpper;
    int yKeyboardTextLower;

    FT_Face font{};

    char * upperDict[DICT_SIZE];
    char * lowerDict[SPEC_DICT_SIZE];
    int dictPosition[DICT_SIZE];
    int specDictPosition[DICT_SIZE];
    int currChar;
    std::string text;
    char getCurrentKey();

public:
    keyboardInput(Scene2D * mainScene, FT_Face font, int x, int y, int width, int height, const char * enterText = "ENTER", const char * placeHolderText = "Input Text...", bool keyBoardIsSelected = false, bool hasEnter = true);
    void updateView();
    void nextKey();
    void previousKey();
    void pressKey();
    void setUpperRow();
    void setLowerRow();
    void deleteChar();
    void selectKeyboard();
    void unSelectKeyboard();
    bool hasEntered() const;
    bool hasChanged();
    const char * readText();
    ~keyboardInput();
};
#endif //CYDI4_KEYBOARDINPUT_H
