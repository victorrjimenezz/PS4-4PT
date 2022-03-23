//
// Created by Víctor Jiménez Rugama on 2/2/22.
//
#include "../../include/view/terminalDialogView.h"
#include "../../include/utils/PNG.h"
#include "../../include/base.h"
#include "../../include/utils/logger.h"

terminalDialogView * terminalDialogView::mainTerminalDialogView = nullptr;
terminalDialogView::terminalDialogView(subView * parentView, Scene2D *mainView, int viewWidth, int viewHeight, FT_Face font) : font(font), repoURL() {
    this->mainView = mainView;
    this->viewWidth = viewWidth;
    this->viewHeight = viewHeight;
    viewSize= viewWidth * viewHeight;
    textColor = {255,255,255};

    this->basexPos = viewWidth * (1-TERMINAL_WIDTH)/2;
    this->baseyPos = viewHeight * (1-TERMINAL_HEIGHT)/2;

    int width = viewWidth*TERMINAL_WIDTH;
    int height = viewHeight*TERMINAL_HEIGHT;

    this->lineXPAD = basexPos*(1+TERMINAL_TEXT_X_PAD);
    this->lineSeparator = height/ TERMINAL_LINES;

    this->maxyPos = baseyPos+height-lineSeparator;

    active = false;
    this->parentView = parentView;
    mainTerminalDialogView = this;

    std::string terminalPNGDir = DATA_PATH;
    terminalPNGDir+="assets/images/terminal.png";
    terminal = new PNG(terminalPNGDir.c_str(),width,height);

    lastInstanceFrameBuffer = (uint32_t *)malloc(sizeof(uint32_t) * viewSize);

    generateDarkenedImage();
}
void terminalDialogView::generateDarkenedImage(){
    for(int i = 0; i < viewSize; i++) {
        uint32_t * pixel = &lastInstanceFrameBuffer[i];
        uint8_t col;

        col = pixel[0];
        uint8_t r = col;
        if(col != 0)
            r = col-(100*(col/255));

        col = pixel[1];
        uint8_t g = col;
        if(col != 0)
            g = col-(100*(col/255));

        col = pixel[2];
        uint8_t b = col;
        if(col != 0)
            b = col-(100*(col/255));

        uint8_t a = 0xFF;

        uint32_t newColor = /*0x80000000 +*/ (a << 24) + (r << 16) + (g << 8) + b;
        *pixel = newColor;
    }
}
void terminalDialogView::updateView() {
    mainView->overWriteFrameBuffer(lastInstanceFrameBuffer, viewSize);
}

terminalDialogView::~terminalDialogView() {
    free(lastInstanceFrameBuffer);
    delete (terminal);
}

void terminalDialogView::newPage() {
    terminal->Draw(lastInstanceFrameBuffer,basexPos,baseyPos,viewWidth,viewHeight);
    this->currentLineHeight = baseyPos+2*lineSeparator;
}

void terminalDialogView::writeLineRecursive(const char *line) {
    std::string lineStr = line;
    if(currentLineHeight > maxyPos)
        newPage();
    Scene2D::DrawText(lastInstanceFrameBuffer,(char *) lineStr.substr(0,MAX_TERMINAL_CHARS).c_str(), font, lineXPAD, currentLineHeight,
                       textColor, textColor,viewWidth,viewHeight);
    this->currentLineHeight += lineSeparator;
    if(lineStr.size() > MAX_TERMINAL_CHARS)
        writeLineRecursive(lineStr.substr(MAX_TERMINAL_CHARS,2*MAX_TERMINAL_CHARS).c_str());
}
void terminalDialogView::writeLine(const char *line) {
    std::unique_lock<std::mutex> lock(writeLock);
    writeLineRecursive(line);

}
int terminalDialogView::closeTerminalDialogView(){
    active = false;
    repoURL = "";
    return active;
}

void terminalDialogView::pressCircle() {
    closeTerminalDialogView();
}

bool terminalDialogView::isActive() {
    return active;
}

subView *terminalDialogView::getParent() {
    return parentView;
}

subView *terminalDialogView::getChild() {
    return nullptr;
}

int terminalDialogView::openTerminalDialogView(const char * repoURLNew) {
    if (active)
        return 0;
    memcpy(&lastInstanceFrameBuffer[0], &mainView->getCurrentFrameBuffer()[0], sizeof(uint32_t) * viewSize);
    generateDarkenedImage();
    if (strcasecmp(repoURL.c_str(),repoURLNew) != 0) {
        newPage();
        this->repoURL = repoURLNew;
    }
    active = true;
    return active;
}

bool terminalDialogView::isForRepo(const char *forRepoURL) {
    if(strcasecmp(repoURL.c_str(),forRepoURL) == 0)
        return active && !repoURL.empty();
    return false;
}
