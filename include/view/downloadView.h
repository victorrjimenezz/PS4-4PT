//
// Created by Víctor Jiménez Rugama on 12/24/21.
//
#ifndef CYDI4_DOWNLOADVIEW_H
#define CYDI4_DOWNLOADVIEW_H
#ifndef GRAPHICS_USES_FONT
#define GRAPHICS_USES_FONT 1
#endif

#include "subView.h"
#include "../utils/AppGraphics.h"

#include <vector>
#include <yaml-cpp/yaml.h>

class PNG;
class download;
class drwav_int16;

class downloadView : public subView {
    enum SelectedOption {
        INSTALL, UNINSTALL, REMOVE
    };
    struct downloadRectangle {
        int x;
        int y;
        int width;
        int height;
        int iconPosY;
    };
private:

    PNG * installIcon;
    PNG * installIconSelected;
    PNG * downloadIcon;
    PNG * downloadIconSelected;
    int installIconX;

    PNG * uninstallIcon;
    PNG * uninstallIconSelected;
    int uninstallIconX;

    PNG * deleteIcon;
    PNG * deleteIconSelected;
    int deleteIconX;

    PNG * pauseIcon;
    PNG * pauseIconSelected;

    int deleteDownload(download * dld);

    drwav_int16  * deleteWav;
    size_t deleteWavCount;

    int repoIconX;
    const static int downloadsPerPage = 5;
    int currPage;
    int selected;
    int rectangleBaseHeight;
    int rectangleDivisorHeight;
    Scene2D * mainScene;
    download * currDownloads[downloadsPerPage];
    std::vector<download*> downloadList;
    downloadRectangle downloadRectangles[downloadsPerPage];
    int downloadDateX;
    int packageTypeX;
    YAML::Node downloadsYAML;
    subView * child;
    Color bgColor{};
    Color textColor{};
    Color selectedColor{};
    FT_Face fontLarge{};
    FT_Face fontMedium{};
    FT_Face fontSmall{};
    SelectedOption option;

    bool isUpdating;
    const int frameWidth = 0;
    const int frameHeight = 0;
    const int viewWidth = 0;
    const int viewHeight = 0;
    void fillPage();
    int loadDownloadList();
public:
    static downloadView *downloadManager;
    downloadView(Scene2D* mainScene, FT_Face fontLarge, FT_Face fontMedium, FT_Face fontSmall, int frameWidth, int frameHeight);
    void addDownload(download * newDownload);
    void updateView();
    void pressX();
    void pressCircle();
    void pressTriangle();
    void pressSquare();
    void arrowUp();
    void arrowDown();
    void arrowRight();
    void arrowLeft();
    void deleteChild();
    subView * getParent();
    subView * getChild();
    bool isActive();
    ~downloadView();
};
#endif //CYDI4_DOWNLOADVIEW_H
