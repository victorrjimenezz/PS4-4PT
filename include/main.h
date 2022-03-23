//
// Created by Víctor Jiménez Rugama on 2/18/22.
//

#ifndef INC_4PT_MAIN_H
#define INC_4PT_MAIN_H
#pragma once
class settings;
class LANG;
class packageSearch;
class subView;
class downloadView;
class repositoryView;
class AudioManager;
settings * getMainSettings();
LANG * getMainLang();
subView * getSubViewAt(int index);
downloadView * getDownloadManager();
AudioManager * getMainAudioManager();
repositoryView * getMainRepositoryView();
packageSearch * getMainPackageSearch();


#endif //INC_4PT_MAIN_H
