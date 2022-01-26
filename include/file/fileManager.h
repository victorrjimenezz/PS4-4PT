//
// Created by Víctor Jiménez Rugama on 12/23/21.
//

#include <unistd.h>
#include <vector>

/*
 * 0 -> Does not Exist
 * >0 fileSize
 */
uint64_t getFileSize(const char * file);

std::vector<std::string> lsDir(const char * path);
/*
 * 1-> Exists
 * 0-> Does not exist
 */
int fileExists(const char * file);

/*
 * 1-> Exists
 * 0-> Does not exist
 */
int folderExists(const char * file);

/*
 * Success <=> 0
 */
int mkDir(const char * path);

/*
 * Success <=> 0
 */
int removeDir(const char * dir);

/*
 * Success <=> 0
 */
int moveFile(const char * file, const char * toFile);

/*
 * Success <=> 0
 */
int copyFile(const char * file, const char * toFile);

/*
 * Success <=> 0
 */
int removeFile(const char * file);