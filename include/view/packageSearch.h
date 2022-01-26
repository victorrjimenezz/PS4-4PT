//
// Created by Víctor Jiménez Rugama on 12/24/21.
//

#ifndef CYDI4_PACKAGESEARCH_H
#define CYDI4_PACKAGESEARCH_H
#ifndef GRAPHICS_USES_FONT
#define GRAPHICS_USES_FONT 1
#endif

#include "subView.h"
#include "../utils/AppGraphics.h"

#include <vector>
#include <unordered_map>
class package;
class repository;
class keyboardInput;
class packageSearch : public subView {
    struct packageRectangle {
        int x;
        int y;
        int width;
        int height;
    };
private:
    int repoIconX;
    const static int packagesPerPage = 5;
    int currPage;
    int selected;
    int packageTypeX;
    int rectangleBaseHeight;
    int rectangleDivisorHeight;
    Scene2D * mainScene;
    std::shared_ptr<package> currPackages[packagesPerPage];
    std::shared_ptr<std::vector<repository*>> repositoryList;
    std::vector<std::shared_ptr<package>> packageList;
    std::vector<std::shared_ptr<package>> displayPackageList;
    packageRectangle packageRectangles[packagesPerPage];
    Color bgColor{};
    Color textColor{};
    Color updateTextColor{};
    Color selectedColor{};
    FT_Face fontLarge{};
    FT_Face fontMedium{};
    FT_Face fontSmall{};

    keyboardInput * keyboardInput;

    bool isOnKeyboard;
    const int viewWidth = 0;
    const int viewHeight = 0;
    void fillPage();
    void filterPackages(const char * name);
public:
    static packageSearch * mainPackageSearch;
    packageSearch(Scene2D* mainScene, FT_Face fontLarge, FT_Face fontMedium, FT_Face fontSmall, int frameWidth, int frameHeight);
    void updateView();
    void pressX();
    void pressCircle();
    void pressTriangle();
    void pressSquare();
    void arrowUp();
    void arrowDown();
    void arrowRight();
    void arrowLeft();
    bool isActive();
    void deleteChild();
    subView * getParent();
    subView * getChild();
    ~packageSearch();
    void updatePackages();
};
#endif //CYDI4_PACKAGESEARCH_H
