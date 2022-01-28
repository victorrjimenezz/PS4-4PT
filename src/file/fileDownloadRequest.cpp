//
// Created by Víctor Jiménez Rugama on 12/31/21.
//
#include "../../include/file/fileDownloadRequest.h"
#include "../../include/base.h"
#include "../../include/utils/logger.h"
#include "../../include/file/fileManager.h"
#include "../../include/file/download.h"
#include "../../include/utils/notifi.h"

#include <orbis/Http.h>
#include <sstream>
#include <regex>

const char * fileDownloadRequest::eoh = "\r\n\r\n";

int fileDownloadRequest::skipSslCallback(int libsslId, unsigned int verifyErr, void * const sslCert[], int certNum, void *userArg) {
    //LOG << "sslCtx= " << libsslId << " " <<verifyErr;
    return HTTP_SUCCESS;
}


int fileDownloadRequest::libhttpCtxId;
void fileDownloadRequest::setLibhttpCtxId(int libhttpCtxId) {
    fileDownloadRequest::libhttpCtxId = libhttpCtxId;
}

int fileDownloadRequest::getLibhttpCtxId() {
    return libhttpCtxId;
}
fileDownloadRequest::fileDownloadRequest(const char * sourceURL, const char * destinationPath, uint64_t startByte, uint64_t fileSize) {
    this->sourceURL = sourceURL;
    this->destinationPath = destinationPath;

    requestID = 0;
    connectionID = 0;

    downloadedBytes = startByte;
    fileSizeBytes = fileSize;
    if(downloadedBytes==0 && fileExists(destinationPath))
        if(removeFile(destinationPath) <0)
            LOG << "Could not delete file at: " << destinationPath;

    fileSizeMB = fileSize /ONE_MB;

    failed = false;
    downloading = false;
    finished = false;
    paused = false;
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
    termRequest();
    return -1;
}

int fileDownloadRequest::downloadError(const char *message, int statusCode) {
    std::stringstream errorCode;
    errorCode << "ErrorCode: " << std::hex << statusCode << std::endl;
    std::string errMessage = message;
    errMessage+=errorCode.str();
    return downloadError(errMessage.c_str());
}

int fileDownloadRequest::initDownload(download * download) {
    int ret = 0;
    if(!verifyURL(sourceURL.c_str())){
        std::string message = "INVALID URL AT ";
        message+=sourceURL;
        ret = downloadError(message.c_str());
        goto err;
    } else if(downloading) {
        ret = downloadError("ALREADY DOWNLOADING");
        goto err;
    }else if(finished) {
        ret = downloadError("ALREADY FINISHED DOWNLOADING");
        goto err;
    }else if(libhttpCtxId == 0){
        download->setFailed(true);
        ret = downloadError("ERROR: Http not initialized");
        goto err;
    } else if(initRequest() < 0){
        ret = downloadError("ERROR: COULD NOT INIT HTTP REQUEST");
        goto err;
    }
    downloading = true;
    paused = false;
    if(fileSizeBytes ==0) {
        ret = parseFileSize();
        if (ret < 0) {
            if (download != nullptr)
                download->setFailed(true);
            ret = downloadError("Error when parsing file size");
            goto err;
        }
    }
    ret = downloadLoop();
    if(ret < 0) {
        if(download != nullptr)
            download->setFailed(true);
        ret = downloadError("Error when downloading file");
    } else if(ret == 0 && download != nullptr && fileSizeBytes == downloadedBytes) {
        download->setFinished();
        download->setFailed(false);
    }
    err:
    termRequest();
    return ret;
}
void fileDownloadRequest::pauseDownload() {
    paused = true;
    downloading =false;
}

int fileDownloadRequest::initRequest() {
    const char * src = sourceURL.c_str();
    std::string byteRange = "bytes="+ std::to_string(downloadedBytes)+"-";

    int statusCode = sceHttpCreateTemplate(libhttpCtxId, USER_AGENT, ORBIS_HTTP_VERSION_1_1, 1);
    if (statusCode < 0)
        return downloadError("Error on sceHttpCreateTemplate()", statusCode);

    templateID = statusCode;

    statusCode = sceHttpsSetSslCallback(templateID, skipSslCallback, NULL);
    if (statusCode < 0) {
        return downloadError("sceHttpsSetSslCallback() Code:", statusCode);
    }

    sceHttpsDisableOption(templateID, 0x01);
    statusCode = sceHttpCreateConnectionWithURL(templateID, src, HTTP_SUCCESS);
    if (statusCode < 0)
        return downloadError("Error on sceHttpCreateConnectionWithURL()", statusCode);

    connectionID = statusCode;

    requestID = sceHttpCreateRequestWithURL(connectionID, ORBIS_METHOD_GET, src, 0);
    if (statusCode < 0)
        return downloadError("Error on sceHttpCreateRequestWithURL()", statusCode);


    sceHttpAddRequestHeader(requestID,"Range",byteRange.c_str(),0);

    statusCode = sceHttpSendRequest(requestID, nullptr, 0);
    if (statusCode < 0) {
        return downloadError("Error on sceHttpSendRequest()", statusCode);
    }

    int ret = sceHttpGetStatusCode(requestID, &statusCode);
    if (ret < 0 || (statusCode != 200 && statusCode != 206)) {
        if(statusCode == 404)
            return downloadError("404 ERROR NOT FOUND");
        else
            return downloadError("UNKNOWN HTTP ERROR", statusCode);
    }
    return 0;
}

int fileDownloadRequest::termRequest() {
    if(requestID!=-69) {
        sceHttpDeleteRequest(requestID);
        requestID = -69;
    }
    if(connectionID!=-69) {
        sceHttpDeleteConnection(connectionID);
        connectionID=-69;
    }
    if(templateID!=-69) {
        sceHttpDeleteTemplate(templateID);
        templateID=-69;
    }
    return 0;
}

int fileDownloadRequest::downloadBytes(const char * url, uint8_t * data, uint64_t startByte, uint64_t endByte) {
    if(startByte > endByte) {
        LOG << "ERROR STARTBYTE>ENDBYTE";
        return -1;
    } else if(!verifyURL(url)) {
        LOG << "INVALID URL " << url;
        return -2;
    }
    int ret, statusCode, tempID, connID=-69, reqID=-69;
    uint64_t sizeBytes = endByte-startByte, read = 1, dldBytes = 0;
    std::string byteRange;
    std::stringstream errorStream;

    tempID = sceHttpCreateTemplate(libhttpCtxId, USER_AGENT, ORBIS_HTTP_VERSION_1_1, HTTP_SUCCESS);
    if (tempID < 0) {
        errorStream << "StatusCode: " << std::hex << tempID << std::endl;
        LOG << "ERROR WHEN CREATING TEMPLATE"  << errorStream.str();
        ret = -1;
        goto err;
    }

    ret = sceHttpsSetSslCallback(tempID, skipSslCallback, NULL);
    if (ret < 0) {
        errorStream << "StatusCode: " << std::hex << ret << std::endl;
        LOG << "sceHttpsSetSslCallback() " << errorStream.str();
        ret = -1;
        goto err;
    }

    connID = sceHttpCreateConnectionWithURL(tempID, url, HTTP_SUCCESS);
    if (connID < 0){
        LOG << "Failed when creating CONNECTION " << url;
        ret = -1;
        goto err;
    }

    reqID = sceHttpCreateRequestWithURL(connID, ORBIS_METHOD_GET, url, 0);
    if (reqID < 0) {
        LOG << "Failed when creating request " << url;
        ret = -1;
        goto err;
    }

    byteRange = "bytes=" +std::to_string(startByte) + "-" + std::to_string(endByte);
    sceHttpAddRequestHeader(reqID,"Range",byteRange.c_str(),0);
    statusCode = sceHttpSendRequest(reqID, nullptr, 0);

    if (statusCode < 0) {
        LOG << "URL IS " << url;
        errorStream << "StatusCode: " << std::hex << statusCode << std::endl;
        LOG << "Failed when sending request" << errorStream.str();
        ret = -1;
        goto err;
    }

    ret = sceHttpGetStatusCode(reqID, &statusCode);
    if (ret < 0 || statusCode != 206) {
        if(ret<0){
            errorStream << "StatusCode: " << std::hex << statusCode << std::endl;
            LOG << "ERROR WITH CODE" << errorStream.str();
        } else {
            LOG << "ERROR WITH CODE: " << statusCode;
            if (statusCode == 404)
                LOG << "ERROR 404, FILE NOT FOUND AT " << url;
        }
        ret = -1;
        goto err;
    }

    char buffer[1024];

    while (dldBytes < sizeBytes && read > 0) {
        read = sceHttpReadData(reqID, &buffer[0], sizeof(buffer));
        uint64_t left = sizeBytes-dldBytes;
        if (dldBytes + read > sizeBytes) {
            memcpy(&data[dldBytes],&buffer[0],left);
            dldBytes += left;
        } else {
            memcpy(&data[dldBytes],&buffer[0],read);
            dldBytes += read;
        }
    }

    if(read < 0) {
        LOG << "READ ERROR";
        ret = -1;
    } else
        ret = 0;
    err:
    if(reqID!=-69)
        sceHttpDeleteRequest(reqID);
    if(connID!=-69)
        sceHttpDeleteConnection(connID);
    if(tempID!=-69)
        sceHttpDeleteTemplate(tempID);

    return ret;
}

int fileDownloadRequest::parseFileSize() {
    int contentLengthType, ret;
    ret = sceHttpGetResponseContentLength(requestID, &contentLengthType, &fileSizeBytes);
    if (ret < 0)
        return downloadError("Error on sceHttpGetContentLength()",ret);

    fileSizeMB = fileSizeBytes/ONE_MB;
    return 0;
}

int fileDownloadRequest::downloadLoop() {
    std::ofstream out;
    out.open(destinationPath, std::ios::binary | std::ios::app);
    if(out.fail())
        return downloadError("File could not be opened");

    char buffer[255];
    uint64_t read = sceHttpReadData(requestID, buffer, sizeof(buffer)), data_offset = 0;
    auto it = std::search(buffer, buffer + read, eoh, eoh + 4);
    if (it != buffer + read) {   // if header end found:
        data_offset = it - buffer + 4;      // skip it
        downloadedBytes -= data_offset;
    }
    while (downloadedBytes < fileSizeBytes && read > 0 && !paused) {
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
    if(paused) {
        downloading=false;
        finished = downloadedBytes == fileSizeBytes;
    } else if(downloadedBytes != fileSizeBytes) {
        return downloadError("There was an error when downloading the file.");
    } else {
        downloading = false;
        finished = true;
    }

    return 0;
}

bool fileDownloadRequest::verifyURL(const char *url) {
    return std::regex_match(url, getUrlRegex());
}

std::regex fileDownloadRequest::getUrlRegex() {
    return std::regex(R"(^https?:\/\/(.+\..{2,10}|localhost|(?:\d{1,3}\.){3}\d{1,3})\/?.*?$)");
}


