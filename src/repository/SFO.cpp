//
// Created by Víctor Jiménez Rugama on 1/14/22.
//
#include "../../include/repository/SFO.h"
#include "../../include/base.h"
#include <vector>
#include <iostream>


SFO::SFO(const char *fromFile) {
    std::vector<uint8_t> dataVector;
    FILE * file = fopen(fromFile,"rb");

    uint8_t byte;
    while(fread(&byte, sizeof byte, 1, file) == 1){
        dataVector.emplace_back(byte);
    }

    fclose(file);

    auto * data = (uint8_t *) malloc(dataVector.size()* sizeof(uint8_t));

    for(int i = 0; i<dataVector.size(); i++)
        data[i]=dataVector[i];
    loadSFOFromData(data);
    free(data);
}

SFO::SFO(uint8_t *data) {
    loadSFOFromData(data);
}

void SFO::loadSFOFromData(uint8_t * data){
    uint32_t entrycountRAW;
    uint32_t tableOffsetRAW;

    memcpy(&entrycountRAW, &data[SFO_ENTRY_COUNT], sizeof(uint32_t));
    entryCount = LE32(entrycountRAW);
    memcpy(&tableOffsetRAW, &data[SFO_KEY_TABLE_OFFSET], sizeof(uint32_t));
    uint32_t keytableOffset = LE32(tableOffsetRAW);
    memcpy(&tableOffsetRAW, &data[SFO_VALUE_TABLE_OFFSET], sizeof(uint32_t));
    uint32_t valueTableOffset = LE32(tableOffsetRAW);
    const char* key_table = (const char*)data + keytableOffset;
    const uint8_t* value_table = (const uint8_t*)data + valueTableOffset;


    uint16_t key_Offset; // 0x00
    uint16_t format; //0x02
    uint32_t value_size; //0x04
    uint32_t max_size; //0x08
    uint32_t value_offset; //0x0C

    uint64_t currPos = SIZEOF_SFO_HEADER;
    entries = (SFOentry *) malloc(entryCount*sizeof(SFOentry));
    for (int i = 0; i < entryCount; ++i) {
        memcpy(&key_Offset,&data[currPos], sizeof(uint16_t));
        memcpy(&format,&data[currPos+SFO_ENTRY_FORMAT], sizeof(uint16_t));
        memcpy(&value_size, &data[currPos + SFO_ENTRY_VALUE_SIZE], sizeof(uint32_t));
        memcpy(&max_size, &data[currPos + SFO_ENTRY_MAX_SIZE], sizeof(uint32_t));
        memcpy(&value_offset, &data[currPos + SFO_ENTRY_VALUE_OFFSET], sizeof(uint32_t));

        entries[i].key = strdup(key_table+LE16(key_Offset));
        entries[i].value = (uint8_t *) malloc(max_size);
        memcpy(entries[i].value,&value_table[value_offset],value_size);
        currPos+=SIZEOF_SFO_TABLE_ENTRY;
    }
}

uint8_t * SFO::getEntry(const char * entry){
    uint8_t * value = nullptr;
    for(int i =0; i<entryCount; i++) {
        SFOentry currEntry = entries[i];
        if (strcmp(currEntry.key, entry) == 0){
            value = currEntry.value;
        }

    }
    return value;
}

SFO::~SFO() {
    for (int i = 0; i < entryCount; ++i)
        free(entries[i].value);
    free(entries);
}
