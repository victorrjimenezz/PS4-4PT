//
// Created by Víctor Jiménez Rugama on 1/14/22.
//

#ifndef APT_PKGINFO_H
#define APT_PKGINFO_H

#include <cstdlib>
#include <string>

class PNG;
class SFO;
class PKGInfo {
private:
    static constexpr int PARAM_SFO_ID = 0x1000;
    static constexpr int PARAM_ICON0_ID = 0x1200;

    bool isLocal;
    uint64_t pkgSize;
    PNG * icon;
    SFO * sfoFile;
    std::string TITLE_ID;
    std::string TITLE;
    std::string url;
    std::string PKG_SFO_TYPE;
    double PKG_APP_VERSION = -1;
    double SYSTEM_VERSION = -1;
    void loadData(uint8_t * data, uint64_t data_size);
    uint8_t * loadTable(uint64_t tableOffset, uint64_t tableSize);
    uint8_t * loadSFO(uint64_t param_sfo_offset, uint64_t param_sfo_size);
    uint8_t * loadIcon(uint64_t icon0_png_offset, uint64_t icon0_size);
public:
    explicit PKGInfo(const char * url, bool isLocal = false);
    explicit PKGInfo(uint8_t * data, uint64_t data_size);
    const char * getTitleID();
    const char * getTitle();
    const char * getType();
    uint64_t getPkgSize();
    PNG * getIconCopy();
    double getVersion() const;
    double getSystemVersion() const;
    ~PKGInfo();
};
#endif //APT_PKGINFO_H
