#include "../../include/utils/utils.h"

#include "../../include/jbUtils/elf.h"
#include "../../include/jbUtils/libjbc.h"

#include <orbis/SystemService.h>
#include <orbis/libkernel.h>

#include <cstddef>
#include <fcntl.h>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sstream>

// Store firmware so we only have to get it once
uint16_t g_Firmware;

// Variables for (un)jailbreaking
jbc_cred g_Cred;
jbc_cred g_RootCreds;

//Find substr in String case insensitive;
bool findStringIC(const std::string & strHaystack, const std::string & strNeedle) {
    auto it = std::search(
            strHaystack.begin(), strHaystack.end(),
            strNeedle.begin(),   strNeedle.end(),
            [](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
    );
    return (it != strHaystack.end() );
}


int CharFromUtf8(unsigned int* out_char, const char* in_text, const char* in_text_end)
{
    unsigned int c = (unsigned int)-1;
    const unsigned char* str = (const unsigned char*)in_text;
    if (!(*str & 0x80)) {
        c = (unsigned int)(*str++);
        *out_char = c;
        return 1;
    }
    if ((*str & 0xe0) == 0xc0) {
        *out_char = 0xFFFD;
        if (in_text_end && in_text_end - (const char*)str < 2) return 1;
        if (*str < 0xc2) return 2;
        c = (unsigned int)((*str++ & 0x1f) << 6);
        if ((*str & 0xc0) != 0x80) return 2;
        c += (*str++ & 0x3f);
        *out_char = c;
        return 2;
    }
    if ((*str & 0xf0) == 0xe0) {
        *out_char = 0xFFFD;
        if (in_text_end && in_text_end - (const char*)str < 3) return 1;
        if (*str == 0xe0 && (str[1] < 0xa0 || str[1] > 0xbf)) return 3;
        if (*str == 0xed && str[1] > 0x9f) return 3;
        c = (unsigned int)((*str++ & 0x0f) << 12);
        if ((*str & 0xc0) != 0x80) return 3;
        c += (unsigned int)((*str++ & 0x3f) << 6);
        if ((*str & 0xc0) != 0x80) return 3;
        c += (*str++ & 0x3f);
        *out_char = c;
        return 3;
    }
    if ((*str & 0xf8) == 0xf0) {
        *out_char = 0xFFFD;
        if (in_text_end && in_text_end - (const char*)str < 4) return 1;
        if (*str > 0xf4) return 4;
        if (*str == 0xf0 && (str[1] < 0x90 || str[1] > 0xbf)) return 4;
        if (*str == 0xf4 && str[1] > 0x8f) return 4;
        c = (unsigned int)((*str++ & 0x07) << 18);
        if ((*str & 0xc0) != 0x80) return 4;
        c += (unsigned int)((*str++ & 0x3f) << 12);
        if ((*str & 0xc0) != 0x80) return 4;
        c += (unsigned int)((*str++ & 0x3f) << 6);
        if ((*str & 0xc0) != 0x80) return 4;
        c += (*str++ & 0x3f);
        if ((c & 0xFFFFF800) == 0xD800) return 4;
        *out_char = c;
        return 4;
    }
    *out_char = 0;
    return 0;
}


std::string genRandom(const int len) {
    static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return tmp_s;
}

std::string genDate(){
    auto now = std::chrono::system_clock::now();
    time_t currentTime = std::chrono::system_clock::to_time_t(now);
    tm currentLocalTime = *localtime(&currentTime);

    std::ostringstream buffer;
    buffer << currentLocalTime.tm_year+1900;
    buffer << "/" <<currentLocalTime.tm_mon+1;
    buffer << "/" << currentLocalTime.tm_mday;
    buffer << " " << currentLocalTime.tm_hour;
    buffer << ":" << currentLocalTime.tm_min;

    return buffer.str();
}

// Verify jailbreak
bool is_jailbroken() {
    FILE *s_FilePointer = fopen("/user/.jailbreak", "w");
    if (!s_FilePointer) {
        return false;
    }

    fclose(s_FilePointer);
    remove("/user/.jailbreak");
    return true;
}

// Jailbreaks creds
void jailbreak() {
    if (is_jailbroken()) {
        return;
    }

    jbc_get_cred(&g_Cred);
    g_RootCreds = g_Cred;
    jbc_jailbreak_cred(&g_RootCreds);
    jbc_set_cred(&g_RootCreds);
}

// Restores original creds
void unjailbreak() {
    if (!is_jailbroken()) {
        return;
    }

    jbc_set_cred(&g_Cred);
}

uint16_t get_firmware() {
    // Return early if this has already been run
    if (g_Firmware) {
        return g_Firmware;
    }

    char sandbox_path[33]; // `/XXXXXXXXXX/common/lib/libc.sprx` [Char count of 32 + nullterm]
    snprintf(sandbox_path, sizeof(sandbox_path), "/%s/common/lib/libc.sprx", sceKernelGetFsSandboxRandomWord());
    int fd = open(sandbox_path, O_RDONLY, 0);
    if (fd < 0) {
        // Assume it's currently jailbroken
        fd = open("/system/common/lib/libc.sprx", O_RDONLY, 0);
        if (fd < 0) {
            // It's really broken
            return 0;
        }
    }

    // Read SELF header from file
    lseek(fd, 0, SEEK_SET);
    SelfHeader self_header;
    if (read(fd, &self_header, sizeof(self_header)) != sizeof(self_header)) {
        return 0;
    }

    // Calculate ELF header offset from the number of SELF segments
    uint64_t elf_header_offset = sizeof(self_header) + self_header.num_of_segments * sizeof(SelfEntry);

    // Read ELF header from file
    lseek(fd, elf_header_offset, SEEK_SET);
    Elf64_Ehdr elf_header;
    if (read(fd, &elf_header, sizeof(elf_header)) != sizeof(elf_header)) {
        return 0;
    }

    // Calculate SCE header offset from number of ELF entries
    uint64_t sce_header_offset = elf_header_offset + elf_header.e_ehsize + elf_header.e_phnum * elf_header.e_phentsize;

    // Align
    while (sce_header_offset % 0x10 != 0) {
        sce_header_offset++;
    }

    // Read SCE header
    lseek(fd, sce_header_offset, SEEK_SET);
    SceHeader sce_header;
    if (read(fd, &sce_header, sizeof(sce_header)) != sizeof(sce_header)) {
        return 0;
    }

    close(fd);

    // Format and return
    char string_fw[5] = {0}; // "0000\0"
    snprintf(string_fw, sizeof(string_fw), "%02lx%02lx", (sce_header.fw_version >> (5 * 8)) & 0xFF, (sce_header.fw_version >> (4 * 8)) & 0xFF);

    uint16_t ret = atoi(string_fw); // Numerical representation of the firmware version. ex: 505 for 5.05, 702 for 7.02, etc

    g_Firmware = ret;
    return ret;
}

uint64_t get_xfast() {
    uint16_t s_Firmware = get_firmware();
    uint64_t s_Xfast = 0x0;

    if (s_Firmware == 350) {
        s_Xfast = 0x003A1AD0;
    } else if (s_Firmware == 355) {
        s_Xfast = 0x003A1F10;
    } else if (s_Firmware == 370) {
        s_Xfast = 0x003A2000;
    } else if (s_Firmware >= 400 && s_Firmware <= 401) {
        s_Xfast = 0x0030EA00;
    } else if (s_Firmware == 405) {
        s_Xfast = 0x0030EB30;
    } else if (s_Firmware >= 406 && s_Firmware <= 407) {
        s_Xfast = 0x0030EB40;
    } else if (s_Firmware >= 450 && s_Firmware <= 455) {
        s_Xfast = 0x003095D0;
    } else if (s_Firmware == 470) {
        s_Xfast = 0x0030B840;
    } else if (s_Firmware >= 471 && s_Firmware <= 474) {
        s_Xfast = 0x0030B7D0;
    } else if (s_Firmware >= 500) {
        s_Xfast = 0x000001C0;
    }

    return s_Xfast;
}