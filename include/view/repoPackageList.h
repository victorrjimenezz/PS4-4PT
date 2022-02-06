//
// Created by Víctor Jiménez Rugama on 12/24/21.
//

#ifndef CYDI4_REPOPACKAGELIST_H
#define CYDI4_REPOPACKAGELIST_H
#ifndef GRAPHICS_USES_FONT
#define GRAPHICS_USES_FONT 1
#endif

#include "subView.h"
#include "../utils/AppGraphics.h"
#include "filterView.h"

#include <vector>
#include <mutex>

class package;
class repository;
class keyboardInput;
class repoPackageList : public subView {
    struct packageRectangle {
        int x;
        int y;
        int width;
        int height;
    };
private:
    std::mutex filterMtx;
    static filterView::sort currentSort;
    static filterView::sortOrder currentSortOrder;
    static bool packageCompare(const std::shared_ptr<package> &lhs, const std::shared_ptr<package> &rhs);
    int repoIconX;

    const static int packagesPerPage = 5;
    int currPage;
    int selected;
    int rectangleBaseHeight;
    int rectangleDivisorHeight;
    Scene2D * mainScene;
    std::shared_ptr<package> currPackages[packagesPerPage];
    std::vector<std::shared_ptr<package>> * packageList;
    std::vector<std::shared_ptr<package>> displayPackageList;
    packageRectangle packageRectangles[packagesPerPage];
    Color bgColor{};
    int packageTypeX;
    Color textColor{};
    Color updateTextColor{};
    Color selectedColor{};
    FT_Face fontLarge{};
    FT_Face fontMedium{};
    FT_Face fontSmall{};
    repository * repo;
    keyboardInput * keyboardInput;
    filterView * filterView;

    subView * parent;
    subView * child;


    bool active;
    const int viewWidth = 0;
    const int viewHeight = 0;
    void fillPage();
    void filterPackages(const char * name);
    bool meetsCriteria(std::shared_ptr<package> &sharedPtr);
public:
    repoPackageList(Scene2D* mainScene, FT_Face fontLarge, FT_Face fontMedium, FT_Face fontSmall, int frameWidth, int frameHeight, repository * repository, subView * parent);
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
    void setActive();
    void deleteChild();
    void langChanged();
    subView * getParent();
    subView * getChild();
    ~repoPackageList();

};
#endif //CYDI4_REPOPACKAGELIST_H
