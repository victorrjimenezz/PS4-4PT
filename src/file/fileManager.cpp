//
// Created by Víctor Jiménez Rugama on 12/23/21.
//
#include "../../include/file/fileManager.h"
#include "../../include/utils/logger.h"

//Std Includes
#include <unistd.h>
#include <cstdio>
#include <ftw.h>
#include <dirent.h>

std::vector<std::string> lsDir(const char * dirPath){
    std::vector<std::string> files;
    if (auto dir = opendir(dirPath)) {
        while (auto f = readdir(dir)) {
            if (f->d_name[0] == '.')
                continue; // Skip everything that starts with a dot
            files.emplace_back(f->d_name);
            //LOG << "File: " << std::string(f->d_name);
        }
        closedir(dir);
    }
    return files;
}
int moveFile(const char * file, const char * toFile){
    return rename(file,toFile);
}

int copyFile(const char * file, const char * toFile){
    int srcfd, dstfd;
    struct stat srcstat{};
    uint8_t buf[1024*8];
    int len;

    if(stat(file, &srcstat) != 0) {
        return -1;
    }

    if((srcfd = open(file, O_RDONLY)) < 0) {
        return -1;
    }

    int flags = O_CREAT | O_WRONLY | O_TRUNC;
    int mode = srcstat.st_mode;
    if((dstfd = open(toFile, flags, mode)) < 0) {
        return -1;
    }

    int rc = 0;
    while((len = read(srcfd, buf, 1024*8)) > 0) {
        if(write(dstfd, buf, len) != len) {
            rc = -1;
            break;
        }
    }
    close(srcfd);
    close(dstfd);
    return 0;
}

int removeFile(const char * file){
    if(fileExists(file))
        return unlink(file);
    else
        return 1;
}

/* Call unlink or rmdir on the path, as appropriate. */
int rm(const char *path, const struct stat *s, int flag, struct FTW *f)
{
    int status;
    int (*rm_func)(const char *);
    (void)s;
    (void)f;
    rm_func = flag == FTW_DP ? rmdir : unlink;
    if( status = rm_func(path), status != 0 )
        LOG << "Error when deleting: " << path;
    //else
    //    LOG << " Deleted: " << path;

    return status;
}

int removeDir(const char * dir){
    if(nftw(dir, rm, 64, FTW_DEPTH))
        return EXIT_FAILURE;
    return 0;
}

int fileExists(const char * file){
    if( access( file, F_OK ) == 0 )
        return 1;
    return 0;
}

int folderExists(const char * path){
    return fileExists(path);
}

int mkDir(const char * path){
    return mkdir(path,0777);
}

uint64_t getFileSize(const char *file) {
    if(!fileExists(file))
        return 0;
    struct stat stat_buf{};
    if(stat(file, &stat_buf) != 0) {
        return -1;
    }
    return stat_buf.st_size;

}
