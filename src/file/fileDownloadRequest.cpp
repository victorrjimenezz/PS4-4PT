//
// Created by Víctor Jiménez Rugama on 12/31/21.
//
#include "../../include/file/fileDownloadRequest.h"
#include "../../include/base.h"
#include "../../include/utils/logger.h"
#include "../../include/file/fileManager.h"
#include "../../include/file/download.h"
#include "../../_common/notifi.h"

#include <orbis/Http.h>

int fileDownloadRequest::libhttpCtxId;
void fileDownloadRequest::setLibhttpCtxId(int libhttpCtxId) {
    fileDownloadRequest::libhttpCtxId = libhttpCtxId;
}

int fileDownloadRequest::getLibhttpCtxId() {
    return libhttpCtxId;
}
fileDownloadRequest::fileDownloadRequest(const char * sourceURL, const char * destinationPath) {
    this->sourceURL = sourceURL;
    this->destinationPath = destinationPath;

    requestID = 0;
    connectionID = 0;

    downloadedBytes = 0;
    fileSizeBytes = 0;
    fileSizeMB = 0;

    failed = false;
    downloading = false;
    finished = false;
    canceled = false;
}

double fileDownloadRequest::getDownloadedPercent() {
    return ((long double)downloadedBytes/(long double)fileSizeBytes) * 100;
}

double fileDownloadRequest::getDownloadedInMb() const {
    return downloadedBytes / ONE_MB;
}

double fileDownloadRequest::getTotalSizeInMb() const {
    return fileSizeMB;
}


bool fileDownloadRequest::hasFailed() const {
    return failed;
}

bool fileDownloadRequest::isDownloading() const {
    return downloading;
}

bool fileDownloadRequest::hasFinished() const {
    return finished;
}

int fileDownloadRequest::downloadError(const char *message) {
    LOG << message;
    if(fileExists(destinationPath.c_str()))
        removeFile(destinationPath.c_str());
    failed = true;
    downloading = false;
    finished = false;
    return -1;
}

int fileDownloadRequest::downloadError(const char *message, int statusCode) {
    std::string errMessage = message;
    errMessage+= std::to_string(statusCode);
    return downloadError(errMessage.c_str());
}

int fileDownloadRequest::initDownload(download * download) {
    int ret;
    if(downloading) {
        downloadError("ALREADY DOWNLOADING");
        goto err;
    }else if(finished) {
        downloadError("ALREADY FINISHED DOWNLOADING");
        goto err;
    }else if(libhttpCtxId == 0){
        download->setFailed(true);
        downloadError("ERROR: Http not initialized");
        goto err;
    }
    downloading = true;
    ret = parseFileSize();
    if(ret < 0) {
        download->setFailed(true);
        downloadError("Error when parsing file size");
        goto err;
    }
    ret = downloadLoop();
    if(ret < 0) {
        download->setFailed(true);
        downloadError("Error when downloading file");
    } else if(ret == 0) {
        download->setFinished();
        download->setFailed(false);
    }
    err:
    return ret;
}
void fileDownloadRequest::cancelDownload() {
    canceled = true;
}
int fileDownloadRequest::initDownload() {
    int ret;
    if(downloading) {
        downloadError("ALREADY DOWNLOADING");
        goto err;
    }else if(finished) {
        downloadError("ALREADY FINISHED DOWNLOADING");
        goto err;
    }else if(libhttpCtxId == 0){
        downloadError("ERROR: Http not initialized");
        goto err;
    }
    downloading = true;
    ret = parseFileSize();
    if(ret < 0) {
        downloadError("Error when parsing file size");
        goto err;
    }
    ret = downloadLoop();
    if(ret < 0)
        downloadError("Error when downloading file");
    err:
    return ret;
}

int fileDownloadRequest::parseFileSize() {
    const char * src = sourceURL.c_str();

    int statusCode = sceHttpCreateTemplate(libhttpCtxId, USER_AGENT, ORBIS_HTTP_VERSION_1_1, 1);
    if (statusCode < 0)
        return downloadError("Error on sceHttpCreateTemplate()", statusCode);


    int tempID = statusCode;
    statusCode = sceHttpCreateConnectionWithURL(tempID, src, KEEPALIVE);
    if (statusCode < 0)
        return downloadError("Error on sceHttpCreateConnectionWithURL()", statusCode);

    connectionID = statusCode;

    statusCode = sceHttpCreateRequestWithURL(connectionID, ORBIS_METHOD_GET, src, 0);
    if (statusCode < 0)
        return downloadError("Error on sceHttpCreateRequestWithURL()", statusCode);

    requestID = statusCode;
    statusCode = sceHttpSendRequest(requestID, nullptr, 0);
    if (statusCode < 0) {
        LOG << "Failed when sending request" << statusCode;
        return downloadError("Error on sceHttpSendRequest()", statusCode);
    }

    int ret = sceHttpGetStatusCode(requestID, &statusCode);
    if (ret < 0 || statusCode != 200) {
        if(statusCode == 404)
            return downloadError("404 ERROR NOT FOUND");
        else
            return downloadError("UNKNOWN HTTP ERROR", statusCode);
    }
    int contentLengthType;
    ret = sceHttpGetResponseContentLength(requestID, &contentLengthType, &fileSizeBytes);
    if (ret < 0)
        return downloadError("Error on sceHttpGetContentLength()",ret);

    fileSizeMB = fileSizeBytes/ONE_MB;
    return 0;
}

int fileDownloadRequest::downloadLoop() {
    FILE * file = fopen(destinationPath.c_str(), "ab");

    if(file == NULL){
        downloadError("File could not opened.");
        return -1;
    }

    std::ofstream out;
    out.open(destinationPath, std::ios::binary);
    char buffer[255];
    uint64_t read = sceHttpReadData(requestID, buffer, sizeof(buffer)), data_offset = 0;
    auto it = std::search(buffer, buffer + read, eoh, eoh + 4);
    if (it != buffer + read) {   // if header end found:
        data_offset = it - buffer + 4;      // skip it
        downloadedBytes -= data_offset;
    }
    while (downloadedBytes < fileSizeBytes && read > 0 && !canceled) {
        uint64_t left = fileSizeBytes - downloadedBytes;
        if (downloadedBytes + read > fileSizeBytes) {
            downloadedBytes += left;
            out.write(buffer - data_offset, left);
        } else {
            out.write(buffer + data_offset, read - data_offset); // write, ignoring before the offset
            downloadedBytes += read;
        }
        read = sceHttpReadData(requestID, buffer, sizeof(buffer));
    }
    out.close();
    fclose(file);
    if(canceled) {
        downloading=false;
    } else if(downloadedBytes != fileSizeBytes) {
        return downloadError("There was an error when downloading the file.");
    } else {
        downloading = false;
        finished = true;
    }
    return 0;
}




