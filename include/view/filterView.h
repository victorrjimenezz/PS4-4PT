//
// Created by Víctor Jiménez Rugama on 2/4/22.
//

#ifndef INC_4PT_FILTERVIEW_H
#define INC_4PT_FILTERVIEW_H

#include "../../include/utils/AppGraphics.h"
#include "../base.h"

#include <vector>
#include <string>

class PNG;
class filterView {
public:
    enum sort{ALPHABET,SIZE,VERSION};
    enum sortOrder{ASCENDANT,DESCENDANT};
private:
    std::string SortStr[PKG_SORT_AMOUNT];
    std::string SortOrderStr[2];
    enum row{UPPER,LOWER};
    row currentRow;
    sort currentSort;
    sortOrder order;
    bool isOnFilterView;
    int selected;

    PNG * checkboxBorder;
    PNG * checkboxCross;
    PNG * checkboxSelectedCross;

    struct position{int xText, yText, xBox, yBox;};

    position OrderPosition;

    int titleX;
    int sortByY;
    int filterY;

    bool enabled[PKG_FILTER_AMOUNT];
    position positionsLOWER[PKG_FILTER_AMOUNT];

    position positionsUPPER[PKG_SORT_AMOUNT];

    Color textColor{};
    Color checkboxColor{};
    Color checkboxEnabledColor{};

    int x, y, width, height;

    Scene2D * mainScene;
    FT_Face font;
    bool changed;
public:
    explicit filterView(Scene2D * mainScene, FT_Face font, int x, int y, int width, int height);
    bool hasChanged();
    void prevOption();
    void nextOption();
    void updateView();
    void pressX();
    void arrowUP();
    void arrowDown();
    void enableFilterView();
    void disableFilterView();
    bool active();
    bool isEnabled(int type);
    bool onlyUpdates();
    sort getCurrentSort();
    sortOrder getCurrentSortOrder();
    ~filterView();

};
#endif //INC_4PT_FILTERVIEW_H
