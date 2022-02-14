//
// Created by Víctor Jiménez Rugama on 12/31/21.
//

#ifndef CYDI4_FILEDOWNLOADREQUEST_H
#define CYDI4_FILEDOWNLOADREQUEST_H
#include <atomic>
#include <string>
#include <chrono>
#include <regex>

class download;
class fileDownloadRequest {
private:
    static int libhttpCtxId;
    static const char *eoh;
    static std::regex getUrlRegex();
    static std::string encodeURL(const std::string& url);

    std::string sourceURL;
    std::string destinationPath;

    int requestID = -69;
    int connectionID = -69;
    int templateID = -69;

    std::chrono::high_resolution_clock::time_point lastCheckpointTime;
    uint64_t    lastCheckPointBytes;
    double averageSpeed;

    uint64_t    downloadedBytes;
    uint64_t    fileSizeBytes;
    double    fileSizeMB;
    bool failed;
    bool downloading;
    bool finished;
    bool paused;


    int parseFileSize();
    int downloadLoop();
    int downloadError(const char * message);
    int downloadError(const char * message, int statusCode);
    static int skipSslCallback(int libsslId, unsigned int verifyErr, void * const sslCert[], int certNum, void *userArg);
    int initRequest();
    int termRequest();

public:
    fileDownloadRequest(const char * sourceURL, const char * destinationPath, uint64_t startByte = 0, uint64_t fileSize = 0);
    int static downloadBytes(const char * url, uint8_t * data, uint64_t startByte, uint64_t endByte);
    bool static verifyURL(const char * url, bool isEncoded = false);

    static void setLibhttpCtxId(int libhttpCtxId);
    static int getLibhttpCtxId();
    double getDownloadedPercent();
    double getDownloadedInMb() const;
    double getTotalSizeInMb() const;
    int initDownload(download * download = nullptr);
    bool hasFailed() const;
    void pauseDownload();
    bool isDownloading() const;
    bool hasFinished() const;

    double getDownloadSpeedInMB();
    double getTimeLeftInMinutes();

};
#endif //CYDI4_FILEDOWNLOADREQUEST_H
