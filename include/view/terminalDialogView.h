//
// Created by Víctor Jiménez Rugama on 2/2/22.
//

#ifndef INC_4PT_TERMINALDIALOGVIEW_H
#define INC_4PT_TERMINALDIALOGVIEW_H
#include "../../include/utils/AppGraphics.h"
#include "../../include/view/subView.h"

#include <string>
#include <mutex>

class subView;
class PNG;
class terminalDialogView : public subView {
private:
    std::mutex writeLock;
    int active;
    int lineSeparator;
    int basexPos;
    int baseyPos;
    int maxyPos;
    int lineXPAD;
    int currentLineHeight;
    FT_Face font;
    subView * parentView;
    uint32_t * lastInstanceFrameBuffer;
    Color textColor{};
    int viewWidth;
    int viewHeight;
    int viewSize;
    std::string repoURL;
    Scene2D * mainView;
    PNG * terminal;
    void generateDarkenedImage();
    void newPage();
    void writeLineRecursive(const char * line);
public:
    static terminalDialogView * mainTerminalDialogView;
    explicit terminalDialogView(subView * parentView, Scene2D * mainView,const int screenWidth,const int screenHeight, FT_Face font);
    int openTerminalDialogView(const char * repoURLNew);
    int closeTerminalDialogView();
    void writeLine(const char * line);
    void updateView();
    void pressX(){};
    void pressCircle();
    void pressTriangle(){};
    void pressSquare(){};
    void arrowUp(){};
    void arrowDown(){};
    void arrowRight(){};
    void arrowLeft(){};
    subView * getParent();
    subView * getChild();
    void deleteChild(){};
    bool isActive();
    void langChanged(){};
    ~terminalDialogView();
    bool isForRepo(const char * forRepoURL);
};
#endif //INC_4PT_TERMINALDIALOGVIEW_H
