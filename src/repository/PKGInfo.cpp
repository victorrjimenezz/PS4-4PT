//
// Created by Víctor Jiménez Rugama on 1/14/22.
//
#include "../../include/repository/PKGInfo.h"
#include "../../include/base.h"
#include "../../include/repository/SFO.h"
#include "../../include/file/fileDownloadRequest.h"
#include "../../include/utils/logger.h"
#include "../../include/utils/PNG.h"
#include "../../include/file/fileManager.h"
#include <string>

PKGInfo::PKGInfo(const char * fromURL, bool isLocal) {
    url = fromURL;
    this->isLocal = isLocal;
    TITLE_ID = "";
    TITLE = "";
    PKG_APP_VERSION = -1;
    icon = nullptr;
    sfoFile = nullptr;
    if(isLocal && getFileSize(fromURL) < PKG_INITIAL_BUFFER_SIZE)
        return;
    auto * data = (uint8_t *) malloc(PKG_INITIAL_BUFFER_SIZE);
    if(isLocal) {
        FILE * file = fopen(fromURL,"rb");
        if(file == nullptr) {
            fclose(file);
            goto err;
        }
        fread(data,PKG_INITIAL_BUFFER_SIZE,1,file);
        fclose(file);
    } else {
        if(fileDownloadRequest::downloadBytes(url.c_str(),data, 0, PKG_INITIAL_BUFFER_SIZE)<0)
            goto err;
    }
    loadData(data, PKG_INITIAL_BUFFER_SIZE);
    err:
    free(data);
}
PKGInfo::PKGInfo(uint8_t * data, uint64_t data_size) {
    url = "";
    PKGInfo::loadData(data,data_size);
}
void PKGInfo::loadData(uint8_t * data, uint64_t data_size){
    bool neededMargin = false;
    uint32_t entrycountRAW;
    memcpy(&entrycountRAW, &data[PKG_ENTRY_COUNT], sizeof(uint32_t));
    size_t entry_count = BE32(entrycountRAW);

    uint32_t tableOffsetRAW;
    memcpy(&tableOffsetRAW, &data[PKG_ENTRY_TABLE_OFFSET], sizeof(uint32_t));
    uint32_t tableOffset = BE32(tableOffsetRAW);

    uint64_t tableSize = entry_count * SIZEOF_PKG_TABLE_ENTRY;

    memcpy(&pkgSize, &data[PKG_SIZE_OFFSET], sizeof(uint64_t));
    pkgSize = BE64(pkgSize);

    uint32_t param_sfo_offset = 0;
    uint32_t param_sfo_size = 0;

    uint32_t icon0_png_offset = 0;
    uint32_t icon_size = 0;

    uint8_t * entry_table_data;
    if(tableOffset+tableSize < data_size) {
        entry_table_data = &data[tableOffset];
    }else {
        entry_table_data = loadTable(tableOffset, tableSize);
        if(entry_table_data == nullptr)
            return;
        neededMargin = true;
    }

    uint64_t currPos = 0x0;
    uint32_t id;
    uint32_t offset;
    uint32_t size;
    for (int i = 0; i < entry_count; ++i) {

        memcpy(&id,&entry_table_data[currPos], sizeof(uint32_t));
        memcpy(&offset,&entry_table_data[currPos+PKG_ENTRY_OFFSET], sizeof(uint32_t));
        memcpy(&size,&entry_table_data[currPos+PKG_ENTRY_SIZE], sizeof(uint32_t));

        switch (BE32(id)) {
            //SFO
            case PARAM_SFO_ID:
                param_sfo_offset = BE32(offset);
                param_sfo_size = BE32(size);
                break;
                //ICON
            case PARAM_ICON0_ID:
                icon0_png_offset = BE32(offset);
                icon_size = BE32(size);
                break;
            default:
                goto next;
        }
        next:
        currPos+=SIZEOF_PKG_TABLE_ENTRY;
    }

    if(param_sfo_offset > 0 && param_sfo_size > 0){
        uint8_t * sfo = loadSFO(param_sfo_offset,param_sfo_size);
        if(sfo != nullptr) {
            sfoFile = new SFO(sfo);
            char * sfoParam = (char*)sfoFile->getEntry("TITLE_ID");
            if(sfoParam != nullptr)
                TITLE_ID = std::string(sfoParam);

            sfoParam = (char*)sfoFile->getEntry("TITLE");
            if(sfoParam != nullptr)
                TITLE = std::string(sfoParam);

            sfoParam = (char*)sfoFile->getEntry("APP_VER");
            if(sfoParam != nullptr)
                PKG_APP_VERSION = std::stod(sfoParam);

            sfoParam = (char*)sfoFile->getEntry("CATEGORY");
            if(sfoParam != nullptr)
                PKG_SFO_TYPE = std::string(sfoParam);
            free(sfo);
        }
    }

    if(icon0_png_offset > 0 && icon_size > 0){
        uint8_t * iconAddr = loadIcon(icon0_png_offset,icon_size);
        if(iconAddr != nullptr) {
            icon = new PNG(iconAddr, icon_size, ICON_DEFAULT_WIDTH, ICON_DEFAULT_HEIGHT);
            free(iconAddr);
        }

    }

    if(neededMargin)
        free(entry_table_data);


}
const char * PKGInfo::getTitleID(){
    return TITLE_ID.c_str();
}
const char * PKGInfo::getTitle(){
    return TITLE.c_str();
}
const char * PKGInfo::getType(){
    return PKG_SFO_TYPE.c_str();
}
double PKGInfo::getVersion() const {
    return PKG_APP_VERSION;
}

PKGInfo::~PKGInfo() {
    delete icon;
    delete sfoFile;
}

PNG *PKGInfo::getIconCopy() {
    if(icon == nullptr)
        return nullptr;
    return new PNG(icon);
}

uint8_t *PKGInfo::loadTable(uint64_t tableOffset, uint64_t tableSize) {
    auto * table = (uint8_t *) malloc(tableSize);
    if(isLocal) {
        FILE * file = fopen(url.c_str(),"rb");
        if(file == nullptr) {
            fclose(file);
            goto err;
        }
        fseek(file, (long)tableOffset, SEEK_CUR);
        fread(table,tableSize,1,file);
        fclose(file);
    } else if(fileDownloadRequest::downloadBytes(url.c_str(),table, tableOffset,  tableOffset+tableSize) < 0) {
        LOG << "COULD NOT DOWNLOAD BYTES";
        goto err;
    }

    return table;
    err:
    free(table);
    return nullptr;
}

uint8_t *PKGInfo::loadIcon(uint64_t icon0_png_offset, uint64_t icon0_size) {
    uint8_t * iconPtr = (uint8_t *) malloc(icon0_size);
    if(isLocal){
        FILE * file = fopen(url.c_str(),"rb");
        if(file == NULL) {
            fclose(file);
            goto err;
        }
        fseek(file, (long)icon0_png_offset, SEEK_CUR);
        fread(iconPtr,icon0_size,1,file);
        fclose(file);
    } else if(fileDownloadRequest::downloadBytes(url.c_str(),iconPtr, icon0_png_offset,  icon0_png_offset+icon0_size) < 0) {
        LOG << "COULD NOT DOWNLOAD BYTES";
        goto err;
    }

    return iconPtr;
    err:
    free(iconPtr);
    iconPtr = nullptr;
    return iconPtr;
}


uint8_t *PKGInfo::loadSFO(uint64_t param_sfo_offset, uint64_t param_sfo_size) {
    auto * SFO = (uint8_t *) malloc(param_sfo_size);
    if(isLocal){
        FILE * file = fopen(url.c_str(),"rb");
        if(file == nullptr) {
            fclose(file);
            goto err;
        }
        fseek(file, (long)param_sfo_offset, SEEK_CUR);
        fread(SFO,param_sfo_size,1,file);
        fclose(file);
    } else if(fileDownloadRequest::downloadBytes(url.c_str(),SFO, param_sfo_offset,  param_sfo_offset+param_sfo_size) < 0) {
        LOG << "COULD NOT DOWNLOAD BYTES";
        goto err;
    }


    return SFO;
    err:
    free(SFO);
    return nullptr;
}

uint64_t PKGInfo::getPkgSize() {
    return pkgSize;
}
