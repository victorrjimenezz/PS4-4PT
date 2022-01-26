//
// Created by Víctor Jiménez Rugama on 1/14/22.
//

#ifndef TESTPROJECT_SFO_H
#define TESTPROJECT_SFO_H
#include <cstdlib>
#include <cstdint>

class SFO {
    struct SFOentry{
        char * key; // 0x00
        uint8_t * value; //0x04
    };
private:
    uint32_t entryCount;
    SFOentry * entries;
    void loadSFOFromData(uint8_t * data);
public:
    uint8_t * getEntry(const char * entry);
    explicit SFO(const char * fromFile);
    explicit SFO(uint8_t * data);
    ~SFO();
};
#endif //TESTPROJECT_SFO_H
