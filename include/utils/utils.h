#include "../jbUtils/libjbc.h"
#include <string>

extern jbc_cred g_Cred;
extern jbc_cred g_RootCreds;

bool findStringIC(const std::string & strHaystack, const std::string & strNeedle);
std::string genRandom(int len);
std::string genDate();

bool is_jailbroken();
void jailbreak();
void unjailbreak();

uint16_t get_firmware();
uint64_t get_xfast();