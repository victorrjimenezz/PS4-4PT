//
// Created by Víctor Jiménez Rugama on 12/26/21.
//

#ifndef CYDI4_BASE_H
#define CYDI4_BASE_H
//default data path
#define DATA_PATH "/mnt/sandbox/PAPT00069_000/app0/"
#define STORED_PATH "/user/app/PAPT00069/"
#define INSTALL_PATH "/user/app/"
#define DOWNLOAD_PATH INSTALL_PATH
#define REPO_PATH "repositories/"
#define LOGS_PATH "logs/"
#define DOWNLOADS_PATH "/user/app/PAPT00069/downloads.yml"

//PACKAGES
#define PKG_TYPE_AMOUNT 7

//Networking
#define ONE_MB 10e5
#define USER_AGENT "PS4-4PT/1.00"
#define KEEPALIVE 1

// Font information
#define FONT_SIZE_LARGE  128
#define FONT_SIZE_MEDIUM_LARGE   100
#define FONT_SIZE_MEDIUM   64
#define FONT_SIZE_SMALL   32

//Amount of Views Information
#define VIEWS 4

// Screen Dimensions
#define FRAME_WIDTH     1920
#define FRAME_HEIGHT    1080
#define FRAME_DEPTH        4

//KeyboardInput
#define DICT_SIZE 26
#define DICT "abcdefghijklmnopqrstuvwxyz"
#define SPEC_DICT_SIZE 14
#define SPEC_DICT "1234567890.:-/"
#define KEYBOARD_VERTICAL_BORDER_SIZE 0.05
#define KEYBOARD_HORIZONTAL_BORDER_SIZE 0.01
#define KEYBOARD_X_POS 0.05
#define KEYBOARD_X_PAD 0.01


//TABVIEW:
#define TABVIEWSIZE 0.1

//TOPVIEW:
#define TOPVIEWSIZE 0.2

//HOMEVIEW
#define HOMEVIEW_LOGO_X 0.5
#define HOMEVIEW_LOGO_Y 0.025
#define HOMEVIEW_LOGO_WIDTH 0.15
#define HOMEVIEW_LOGO_HEIGHT 0.2

#define HOMEVIEW_KEYBOARD_X 0.1
#define HOMEVIEW_KEYBOARD_Y 0.75
#define HOMEVIEW_KEYBOARD_WIDTH 0.7
#define HOMEVIEW_KEYBOARD_HEIGHT 0.15

#define LOGO_PATH "/mnt/sandbox/PAPT00069_000/app0/assets/images/logo.png"

//DIVISORS
#define RECTANGLEDIVISORHEIGHT 0.05
#define OPEN_ICON_POS 0.7
#define UPDATE_ICON_POS 0.8
#define DELETE_ICON_POS 0.9
#define PKGLIST_TYPE_POS 0.7
#define PACKAGE_TYPE_POS 0.4
#define DOWNLOAD_DATE_POS 0.6
#define REPO_X_POS 0.1
#define REPO_ICON_POS 0.2

//String
#define DOWNLOAD_CHARACTER_LIMIT 18
#define DOWNLOAD_NAME_CHARACTER_LIMIT 18
#define PKGLIST_CHARACTER_LIMIT 30

//IMG INFORMATION
#define ICON_DEFAULT_WIDTH 100
#define ICON_DEFAULT_HEIGHT 100

//APP VERSION
#define APP_VERSION "1.00"

#endif //CYDI4_BASE_H
