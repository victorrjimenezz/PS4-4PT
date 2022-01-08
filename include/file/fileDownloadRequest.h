//
// Created by Víctor Jiménez Rugama on 12/31/21.
//

#ifndef CYDI4_FILEDOWNLOADREQUEST_H
#define CYDI4_FILEDOWNLOADREQUEST_H
#include <atomic>
#include <string>

class download;
class fileDownloadRequest {
private:
    static int libhttpCtxId;
    const char *eoh = "\r\n\r\n";

    std::string sourceURL;
    std::string destinationPath;

    int requestID;
    int connectionID;

    uint64_t    downloadedBytes;
    uint64_t    fileSizeBytes;
    double    fileSizeMB;
    bool failed;
    bool downloading;
    bool finished;
    bool canceled;


    int parseFileSize();
    int downloadLoop();
    int downloadError(const char * message);
    int downloadError(const char * message, int statusCode);

public:
    fileDownloadRequest(const char * sourceURL, const char * destinationPath);
    static void setLibhttpCtxId(int libhttpCtxId);
    static int getLibhttpCtxId();
    double getDownloadedPercent();
    double getDownloadedInMb() const;
    double getTotalSizeInMb() const;
    int initDownload();
    int initDownload(download * download);
    bool hasFailed() const;
    void cancelDownload();
    bool isDownloading() const;
    bool hasFinished() const;

};
#endif //CYDI4_FILEDOWNLOADREQUEST_H
