//
// Created by Víctor Jiménez Rugama on 2/4/22.
//
#include "../../include/view/filterView.h"
#include "../../include/utils/PNG.h"
#include "../../include/repository/package.h"
#include "../../include/utils/LANG.h"


filterView::filterView(Scene2D *mainScene, FT_Face font, int x, int y, int width, int height) : font(font), positionsLOWER() {
    this->mainScene = mainScene;
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->isOnFilterView = false;
    this->changed = true;
    this->order = ASCENDANT;
    this->currentRow = LOWER;
    this->currentSort = ALPHABET;
    this->selected = 0;

    this->titleX = x +FILTER_TITLE_OFFSET;
    this->sortByY = y - height / 5 +FILTER_CHECKBOX_HEIGHT;
    this->filterY = y + height / 6 +FILTER_CHECKBOX_HEIGHT;

    this->textColor = {0,0,0};
    this->checkboxColor = {255,255,255};
    this->checkboxEnabledColor = {255,0,0};

    //SET SORT TYPES STRING
    SortStr[0] = LANG::mainLang->ALPHABETICALLY;
    SortStr[1] = LANG::mainLang->SIZE;
    SortStr[2] = LANG::mainLang->VERSION;

    //SET ORDER BY STRING
    SortOrderStr[0] = LANG::mainLang->ASCENDING;
    SortOrderStr[1] = LANG::mainLang->DESCENDING;

    for(bool & i : enabled)
        i = true;

    for(int i =0; i<PKG_FILTER_AMOUNT-1; i++) {
        positionsLOWER[i].xBox = x + (i+1) * (width) / (PKG_FILTER_AMOUNT + 2);;
        positionsLOWER[i].yBox = y + height / 6;

        positionsLOWER[i].xText = positionsLOWER[i].xBox + FILTER_CHECKBOX_WIDTH + FILTER_CHAR_OFFSET;
        positionsLOWER[i].yText = y + height / 6 + FILTER_CHECKBOX_HEIGHT;
    }
    {
        positionsLOWER[PKG_FILTER_AMOUNT - 1].xBox = FILTER_CHAR_OFFSET+x + (PKG_FILTER_AMOUNT) * (width) / (PKG_FILTER_AMOUNT + 2);
        positionsLOWER[PKG_FILTER_AMOUNT - 1].yBox = y + height / 6;

        positionsLOWER[PKG_FILTER_AMOUNT - 1].xText = positionsLOWER[PKG_FILTER_AMOUNT - 1].xBox + FILTER_CHECKBOX_WIDTH + FILTER_CHAR_OFFSET;
        positionsLOWER[PKG_FILTER_AMOUNT - 1].yText = y + height / 6 + FILTER_CHECKBOX_HEIGHT;

        enabled[PKG_FILTER_AMOUNT-1] = false;
    }

    for(int i =0; i<PKG_SORT_AMOUNT; i++) {
        positionsUPPER[i].xBox = x + (i+1) * (width) / (PKG_SORT_AMOUNT + 2);
        positionsUPPER[i].yBox = y - height / 5;

        positionsUPPER[i].xText = positionsUPPER[i].xBox + FILTER_CHECKBOX_WIDTH + FILTER_CHAR_OFFSET;
        positionsUPPER[i].yText = y - height / 5 + FILTER_CHECKBOX_HEIGHT;
    }
    {
        int i = PKG_SORT_AMOUNT - 1;
        OrderPosition.xBox = 0;
        OrderPosition.yBox = 0;

        OrderPosition.xText = positionsUPPER[i].xBox + (width) / (PKG_SORT_AMOUNT + 2);
        OrderPosition.yText = y - height / 5 + FILTER_CHECKBOX_HEIGHT;
    }
    checkboxBorder = new PNG(DATA_PATH "assets/images/filters/boxSelected.png", FILTER_CHECKBOX_WIDTH, FILTER_CHECKBOX_HEIGHT);
    checkboxCross = new PNG(DATA_PATH "assets/images/filters/cross.png",FILTER_CHECKBOX_WIDTH,FILTER_CHECKBOX_HEIGHT);
    checkboxSelectedCross = new PNG(DATA_PATH "assets/images/filters/crossSelected.png",FILTER_CHECKBOX_WIDTH,FILTER_CHECKBOX_HEIGHT);

}

bool filterView::hasChanged() {
    bool oldChanged = changed;
    changed = false;
    return oldChanged;
}

void filterView::updateView() {
    mainScene->DrawText((char * ) LANG::mainLang->ORDER.c_str(), font, titleX, sortByY, textColor, textColor);
    mainScene->DrawText((char * ) LANG::mainLang->FILTER.c_str(),font,titleX,filterY,textColor,textColor);

    for(int i =0; i<PKG_FILTER_AMOUNT-1; i++) {
        position currPosition = positionsLOWER[i];
        mainScene->DrawText((char * ) TypeStr[i],font,currPosition.xText,currPosition.yText,textColor,textColor);
        mainScene->DrawRectangle(currPosition.xBox,currPosition.yBox,FILTER_CHECKBOX_WIDTH,FILTER_CHECKBOX_HEIGHT,checkboxColor);
        if(selected == i && isOnFilterView && currentRow == LOWER) {
            if(enabled[i])
                mainScene->DrawRectangle(currPosition.xBox,currPosition.yBox,FILTER_CHECKBOX_WIDTH,FILTER_CHECKBOX_HEIGHT,checkboxEnabledColor);
            else
                checkboxSelectedCross->Draw(mainScene, currPosition.xBox, currPosition.yBox);
        } else if(enabled[i])
            checkboxCross->Draw(mainScene, currPosition.xBox, currPosition.yBox);
        checkboxBorder->Draw(mainScene, currPosition.xBox, currPosition.yBox);
    }
    {
        int i = PKG_FILTER_AMOUNT-1;
        position currPosition = positionsLOWER[i];
        mainScene->DrawText((char * ) LANG::mainLang->ONLY_UPDATES.c_str(),font,currPosition.xText,currPosition.yText,textColor,textColor);
        mainScene->DrawRectangle(currPosition.xBox,currPosition.yBox,FILTER_CHECKBOX_WIDTH,FILTER_CHECKBOX_HEIGHT,checkboxColor);
        if(selected == i && isOnFilterView && currentRow == LOWER) {
            if(enabled[i])
                mainScene->DrawRectangle(currPosition.xBox,currPosition.yBox,FILTER_CHECKBOX_WIDTH,FILTER_CHECKBOX_HEIGHT,checkboxEnabledColor);
            else
                checkboxSelectedCross->Draw(mainScene, currPosition.xBox, currPosition.yBox);
        } else if(enabled[i])
            checkboxCross->Draw(mainScene, currPosition.xBox, currPosition.yBox);
        checkboxBorder->Draw(mainScene, currPosition.xBox, currPosition.yBox);
    }

    for(int i =0; i<PKG_SORT_AMOUNT; i++) {
        position currPosition = positionsUPPER[i];
        mainScene->DrawText((char * ) SortStr[i].c_str(),font,currPosition.xText,currPosition.yText,textColor,textColor);
        mainScene->DrawRectangle(currPosition.xBox,currPosition.yBox,FILTER_CHECKBOX_WIDTH,FILTER_CHECKBOX_HEIGHT,checkboxColor);
        if(selected == i && isOnFilterView && currentRow == UPPER) {
            if(currentSort == i)
                mainScene->DrawRectangle(currPosition.xBox,currPosition.yBox,FILTER_CHECKBOX_WIDTH,FILTER_CHECKBOX_HEIGHT,checkboxEnabledColor);
            else
                checkboxSelectedCross->Draw(mainScene, currPosition.xBox, currPosition.yBox);
        } else if(currentSort == i)
            checkboxCross->Draw(mainScene, currPosition.xBox, currPosition.yBox);
        checkboxBorder->Draw(mainScene, currPosition.xBox, currPosition.yBox);
    }
    {
        Color currColor = selected == PKG_SORT_AMOUNT && isOnFilterView && currentRow == UPPER ? checkboxEnabledColor : textColor;
        switch(order) {
            case ASCENDANT:
                mainScene->DrawText((char *) SortOrderStr[0].c_str(), font, OrderPosition.xText,
                                    OrderPosition.yText, currColor, currColor);
                break;
            case DESCENDANT:
                mainScene->DrawText((char *) SortOrderStr[1].c_str(), font, OrderPosition.xText,
                                    OrderPosition.yText, currColor, currColor);
                break;
        }
    }

}

filterView::~filterView() {
    delete checkboxSelectedCross;
    delete checkboxBorder;
    delete checkboxCross;

}

void filterView::prevOption() {
    switch(currentRow) {
        case UPPER:
            if (--selected < 0)
                selected = PKG_SORT_AMOUNT;
            break;
        case LOWER:
            if (--selected < 0)
                selected = PKG_FILTER_AMOUNT - 1;
            break;
        default:
            break;
    }
}

void filterView::nextOption() {
    switch(currentRow) {
        case UPPER:
            selected++;
            selected%=PKG_SORT_AMOUNT+1;
            break;
        case LOWER:
            selected++;
            selected%=PKG_FILTER_AMOUNT;
            break;
        default:
            break;
    }
}

void filterView::pressX() {
    switch(currentRow) {
        case UPPER:
            if(selected < PKG_SORT_AMOUNT) {
                if (currentSort != selected) {
                    currentSort = static_cast<sort>(selected);
                    changed = true;
                }
            } else {
                switch (order) {
                case ASCENDANT:
                    order = DESCENDANT;
                    break;
                case DESCENDANT:
                    order = ASCENDANT;
                    break;
                }
                changed = true;
            }
            break;
        case LOWER:
            enabled[selected] = !enabled[selected];
            changed = true;
            break;
        default:
            break;
    }
}

void filterView::enableFilterView(){
    isOnFilterView = true;
}
void filterView::disableFilterView() {
    isOnFilterView = false;
}

bool filterView::active() {
    return isOnFilterView;
}

bool filterView::isEnabled(int type) {
    return enabled[type];
}

bool filterView::onlyUpdates() {
    return enabled[PKG_FILTER_AMOUNT-1];
}

void filterView::arrowDown() {
    if(currentRow==LOWER)
        return;
    selected /= (int)((double)selected/(double)PKG_SORT_AMOUNT * (double)PKG_FILTER_AMOUNT);
    currentRow = LOWER;
}

void filterView::arrowUP() {
    if(currentRow==UPPER)
        return;
    selected /= (int)((double)selected/(double)PKG_FILTER_AMOUNT * (double)PKG_SORT_AMOUNT);
    currentRow = UPPER;
}

filterView::sort filterView::getCurrentSort() {
    return currentSort;
}

filterView::sortOrder filterView::getCurrentSortOrder() {
    return order;
}
