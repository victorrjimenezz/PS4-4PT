#ifndef ELF_H
#define ELF_H

#include "elf64.h"
#include "elf_common.h"

#include "../../../../../../../../opt/OpenOrbis-PS4-Toolchain/include/stdint.h"

// Swapped
#define SELF_MAGIC 0x1D3D154F
#define ELF_MAGIC 0x464C457F

typedef struct {
  uint32_t props;
  uint32_t reserved;
  uint64_t offset;
  uint64_t file_size;
  uint64_t memory_size;
} SelfEntry;

// SELF Header from: https://www.psdevwiki.com/ps4/SELF_File_Format#SELF_Header_Structure
typedef struct {
  uint32_t magic; /* File magic. */

  // uint32_t unknown;        /* Always 00 01 01 12. */
  uint8_t version;
  uint8_t mode;
  uint8_t endian;
  uint8_t attr;

  unsigned char content_type; /* 1 on Self, 4 on PUP Entry. */
  unsigned char program_type; /* 0x0 PUP, 0x8 NPDRM Application, 0x9 PLUGIN, 0xC Kernel, 0xE Security Module, 0xF Secure Kernel */
  uint16_t padding;           /* Padding. */
  uint16_t header_size;       /* Header size. */
  uint16_t signature_size;    /* Metadata size? */
  uint64_t self_size;         /* Size of SELF. */
  uint16_t num_of_segments;   /* Number of Segments, 1 Kernel, 2 SL and Modules, 4 Kernel ELFs, 6 .selfs, 2 .sdll, 6 .sprx, 6 ShellCore, 6 eboot.bin, 2 sexe. */
  uint16_t flags;             /* Always 0x22. */
  uint32_t reserved;          /* Reserved. */
} SelfHeader;

// SCE Header from: https://www.psdevwiki.com/ps4/SELF_File_Format#SCE_Special
typedef struct {
  uint64_t program_authority_id;
  uint64_t program_type;
  uint64_t app_version;
  uint64_t fw_version;
  unsigned char digest[0x20];
} SceHeader;

#endif
