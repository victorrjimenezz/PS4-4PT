//
// Created by Víctor Jiménez Rugama on 12/26/21.
//

#ifndef CYDI4_BASE_H
#define CYDI4_BASE_H

//APP VERSION
#define APP_VERSION 2.30
#define APP_TITLEID "PAPT00230"
#define MAIN_URL "https://www.4pt-project.com/"


//default data path
#define DATA_PATH "/mnt/sandbox/" APP_TITLEID "_000/app0/"
#define STORED_PATH "/user/app/" APP_TITLEID "/"
#define INSTALL_PATH "/user/app/"
#define DOWNLOAD_PATH INSTALL_PATH
#define REPO_PATH "repositories/"
#define LANG_PATH DATA_PATH "assets/lang/"

#define LOGS_PATH "logs/"
#define LOGO_PATH "/mnt/sandbox/" APP_TITLEID "_000/app0/assets/images/logo.png"

#define UPDATES_FILE "updates.yml"
#define UPDATES_FILE_URL MAIN_URL UPDATES_FILE

#define DOWNLOADS_FILE "downloads.yml"
#define DOWNLOADS_PATH "/user/app/" APP_TITLEID "/" DOWNLOADS_FILE

//PACKAGES
#define PKG_EXTENSION ".pkg"
#define PKG_TYPE_AMOUNT 7

//Networking
#define ONE_MB 10e5
#define USER_AGENT "PS4-4PT/2.30" //"Mozilla/5.0 (PLAYSTATION 4; 1.00)"
#define HTTP_SUCCESS 1
#define BGFT_HEAP_SIZE (1 * 1024 * 1024)
#define HTTP_HEAP_SIZE (1024 * 1024)
#define SSL_HEAP_SIZE (4*128 * 1024)
#define NET_HEAP_SIZE   (1 * 1024 * 1024)

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

//DIVISORS
#define RECTANGLEDIVISORHEIGHT 0.05
#define OPEN_ICON_POS 0.7
#define UPDATE_ICON_POS 0.8
#define INSTALL_ICON_POS 0.75
#define UNINSTALL_ICON_POS 0.85
#define DOWNLOAD_DELETE_ICON_POS 0.95
#define DELETE_ICON_POS 0.9
#define PKGLIST_TYPE_POS 0.7
#define PACKAGE_TYPE_POS 0.4
#define DOWNLOAD_DATE_POS 0.6
#define REPO_X_POS 0.1
#define REPO_ICON_POS 0.2

//SEARCH
#define CURR_PAGE_X 0.953
#define CURR_PAGE_Y 0.5

//String
#define DOWNLOAD_CHARACTER_LIMIT 18
#define DOWNLOAD_NAME_CHARACTER_LIMIT 18
#define PKGLIST_CHARACTER_LIMIT 30

//IMG INFORMATION
#define ICON_DEFAULT_WIDTH 100
#define ICON_DEFAULT_HEIGHT 100

#define DOWNLOAD_OPTION_ICON_WIDTH 75
#define DOWNLOAD_OPTION_ICON_HEIGHT 75

//BINARY OPERATIONS
#define LE16(x) (x)
#define LE32(x) (x)

#define SWAP32(x) \
	((uint32_t)( \
		(((uint32_t)(x) & UINT32_C(0x000000FF)) << 24) | \
		(((uint32_t)(x) & UINT32_C(0x0000FF00)) <<  8) | \
		(((uint32_t)(x) & UINT32_C(0x00FF0000)) >>  8) | \
		(((uint32_t)(x) & UINT32_C(0xFF000000)) >> 24) \
	))
#define SWAP64(x) \
	((uint64_t)( \
		(uint64_t)(((uint64_t)(x) & UINT64_C(0x00000000000000FF)) << 56) | \
		(uint64_t)(((uint64_t)(x) & UINT64_C(0x000000000000FF00)) << 40) | \
		(uint64_t)(((uint64_t)(x) & UINT64_C(0x0000000000FF0000)) << 24) | \
		(uint64_t)(((uint64_t)(x) & UINT64_C(0x00000000FF000000)) <<  8) | \
		(uint64_t)(((uint64_t)(x) & UINT64_C(0x000000FF00000000)) >>  8) | \
		(uint64_t)(((uint64_t)(x) & UINT64_C(0x0000FF0000000000)) >> 24) | \
		(uint64_t)(((uint64_t)(x) & UINT64_C(0x00FF000000000000)) >> 40) | \
		(uint64_t)(((uint64_t)(x) & UINT64_C(0xFF00000000000000)) >> 56) \
	))

#define BE32(x) SWAP32(x)
#define BE64(x) SWAP64(x)

//PACKAGE FILE INFO
#define PKG_INITIAL_BUFFER_SIZE 16000
#define PKG_SIZE_OFFSET 0x430
#define PKG_CONTENT_ID_SIZE 0x24
#define SIZEOF_PKG_TABLE_ENTRY 0x20
#define PKG_ENTRY_COUNT 0x10
#define PKG_ENTRY_TABLE_OFFSET 0x18
#define PKG_ENTRY_OFFSET 0x10
#define PKG_ENTRY_SIZE 0x14

//SFO FILE
#define SFO_ENTRY_COUNT 0x10
#define SFO_KEY_TABLE_OFFSET 0x08
#define SFO_VALUE_TABLE_OFFSET 0x0C

#define SFO_ENTRY_FORMAT 0x02
#define SFO_ENTRY_VALUE_SIZE 0x04
#define SFO_ENTRY_MAX_SIZE 0x08
#define SFO_ENTRY_VALUE_OFFSET 0x0C

#define SIZEOF_SFO_TABLE_ENTRY 0x10
#define SIZEOF_SFO_HEADER 0x14

//ORBIS INFORMATION
#define ORBIS_APPINSTUTIL_APP_ALREADY_INSTALLED 0x80990088
#endif //CYDI4_BASE_H
