# Package metadata.
TITLE       := 4PT
VERSION     := 2.5
TITLE_ID    := PAPT00250

CONTENT_ID  := IV0000-PAPT00250_00-PSAAPTPKG0000000
PROJNAME    := 4PT

# Libraries linked into the ELF.
LIBS        :=  -lc -lkernel -lc++ -lSceAudioOut -lSceSysmodule -lSceSysCore -lSceSystemService -lSceUserService -lSceAppInstUtil -lSceBgft -lSceVideoOut -lSceFreeType -lScePad -lyaml-cpp -lSceNetCtl -lSceNet -lSceSsl -lSceHttp -lSceCommonDialog -lSceMsgDialog -ljbc

# Additional compile flags.
EXTRAFLAGS  := -DGRAPHICS_USES_FONT

# Asset and module directories.
ASSETS    := $(shell find assets -type f -name '*.*')
LIBMODULES        := $(shell find sce_module -type f -name '*.*')

# You likely won't need to touch anything below this point.

# Root vars
TOOLCHAIN   := $(OO_PS4_TOOLCHAIN)
PROJDIR     := $(CURDIR)/src
COMMONDIR   := $(TOOLCHAIN)/samples/_common
INTDIR      := $(CURDIR)/build

# Define objects to build
CFILES      := $(wildcard $(PROJDIR)/*.c)
CPPFILES    := $(wildcard $(PROJDIR)/*.cpp)
COMMONFILES := $(wildcard $(COMMONDIR)/*.cpp)
OBJS        :=  $(patsubst $(PROJDIR)/%.c, $(INTDIR)/%.o, $(CFILES)) $(patsubst $(PROJDIR)/%.cpp, $(INTDIR)/%.o, $(CPPFILES)) #$(patsubst $(COMMONDIR)/%.cpp, $(INTDIR)/%.o, $(COMMONFILES))

VIEWDIR = /view
#Add View Dir
CFILES      += $(wildcard $(PROJDIR)$(VIEWDIR)/*.c)
CPPFILES    += $(wildcard $(PROJDIR)$(VIEWDIR)/*.cpp)
OBJS        += $(patsubst $(PROJDIR)$(VIEWDIR)/%.cpp, $(INTDIR)$(VIEWDIR)/%.o, $(CPPFILES)) $(patsubst $(PROJDIR)$(VIEWDIR)/%.c, $(INTDIR)$(VIEWDIR)/%.o, $(CFILES))

FILEDIR = /file
#Add File Dir
CFILES      += $(wildcard $(PROJDIR)$(FILEDIR)/*.c)
CPPFILES    += $(wildcard $(PROJDIR)$(FILEDIR)/*.cpp)
OBJS        += $(patsubst $(PROJDIR)$(FILEDIR)/%.cpp, $(INTDIR)$(FILEDIR)/%.o, $(CPPFILES)) $(patsubst $(PROJDIR)$(FILEDIR)/%.c, $(INTDIR)$(FILEDIR)/%.o, $(CFILES))

UTILSDIR = /utils
#Add UTILS Dir
CFILES      += $(wildcard $(PROJDIR)$(UTILSDIR)/*.c)
CPPFILES    += $(wildcard $(PROJDIR)$(UTILSDIR)/*.cpp)
OBJS        += $(patsubst $(PROJDIR)$(UTILSDIR)/%.cpp, $(INTDIR)$(UTILSDIR)/%.o, $(CPPFILES)) $(patsubst $(PROJDIR)$(UTILSDIR)/%.c, $(INTDIR)$(UTILSDIR)/%.o, $(CFILES))

REPOSITORYDIR = /repository
#Add REPO Dir
CFILES      += $(wildcard $(PROJDIR)$(REPOSITORYDIR)/*.c)
CPPFILES    += $(wildcard $(PROJDIR)$(REPOSITORYDIR)/*.cpp)
OBJS        += $(patsubst $(PROJDIR)$(REPOSITORYDIR)/%.cpp, $(INTDIR)$(REPOSITORYDIR)/%.o, $(CPPFILES)) $(patsubst $(PROJDIR)$(REPOSITORYDIR)/%.c, $(INTDIR)$(REPOSITORYDIR)/%.o, $(CFILES))


# Create the intermediate directory incase it doesn't already exist.
_unused     := $(shell mkdir -p $(INTDIR)) $(shell mkdir -p $(INTDIR)$(FILEDIR)/) $(shell mkdir -p $(INTDIR)$(UTILSDIR)/) $(shell mkdir -p $(INTDIR)$(VIEWDIR)/) $(shell mkdir -p $(INTDIR)$(REPOSITORYDIR)/) $(shell mkdir -p $(INTDIR)$(YAMLDIR)/)


OUTS        := $(INTDIR)$(FILEDIR)/*.o  $(INTDIR)$(REPOSITORYDIR)/*.o $(INTDIR)$(UTILSDIR)/*.o $(INTDIR)$(VIEWDIR)/*.o
# Define final C/C++ flags
CFLAGS      := --target=x86_64-pc-freebsd12-elf -fPIC -funwind-tables -c $(EXTRAFLAGS) -isysroot $(TOOLCHAIN) -isystem $(TOOLCHAIN)/include
CXXFLAGS    := $(CFLAGS) -isystem $(TOOLCHAIN)/include/c++/v1
LDFLAGS     := -m elf_x86_64 -pie --script $(TOOLCHAIN)/link.x --eh-frame-hdr -L$(TOOLCHAIN)/lib $(LIBS) $(TOOLCHAIN)/lib/crt1.o

# Check for linux vs macOS and account for clang/ld path
UNAME_S     := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
		CC      := clang
		CCX     := clang++
		LD      := ld.lld
		CDIR    := linux
endif
ifeq ($(UNAME_S),Darwin)
		CC      := /usr/local/opt/llvm@9/bin/clang
		CCX     := /usr/local/opt/llvm@9/bin/clang++
		LD      := /usr/local/opt/llvm@9/bin/ld.lld
		CDIR    := macos
endif

all: $(CONTENT_ID).pkg sce_sys/param.sfo
	rm -r $(INTDIR)
	rm -r pkg.gp4
	rm -r sce_sys/param.sfo
	rm -r eboot.bin

$(CONTENT_ID).pkg: pkg.gp4
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core pkg_build $< .

pkg.gp4: eboot.bin sce_sys/about/right.sprx sce_sys/param.sfo sce_sys/icon0.png sce_sys/pic0.png sce_sys/pic1.png $(LIBMODULES) $(ASSETS)
	$(TOOLCHAIN)/bin/$(CDIR)/create-gp4 -o $@ -c $(CONTENT_ID) $^
sce_sys/param.sfo: Makefile
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_new $@
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ APP_TYPE --type Integer --maxsize 4 --value 1 
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ APP_VER --type Utf8 --maxsize 8 --value '$(VERSION)'
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ ATTRIBUTE --type Integer --maxsize 4 --value 0  
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ CATEGORY --type Utf8 --maxsize 4 --value 'gd'  
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ CONTENT_ID --type Utf8 --maxsize 48 --value '$(CONTENT_ID)'
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ DOWNLOAD_DATA_SIZE --type Integer --maxsize 4 --value 0 
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ SYSTEM_VER --type Integer --maxsize 4 --value 0  
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ TITLE --type Utf8 --maxsize 128 --value '$(TITLE)'
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ TITLE_ID --type Utf8 --maxsize 12 --value '$(TITLE_ID)'
	$(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core sfo_setentry $@ VERSION --type Utf8 --maxsize 8 --value '$(VERSION)'

eboot.bin: $(INTDIR) $(OBJS)
	$(LD) $(OUTS) $(INTDIR)/*.o -o $(INTDIR)/$(PROJNAME).elf $(LDFLAGS)
	$(TOOLCHAIN)/bin/$(CDIR)/create-eboot -in=$(INTDIR)/$(PROJNAME).elf
	$(TOOLCHAIN)/bin/$(CDIR)/create-fself $(INTDIR)/$(PROJNAME).oelf eboot.bin --paid 0x3800000000000011

$(INTDIR)/%.o: $(PROJDIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

$(INTDIR)/%.o: $(PROJDIR)/%.cpp
	$(CCX) $(CXXFLAGS) -o $@ $<

#$(INTDIR)/%.o: $(COMMONDIR)/%.c
#	$(CCX) $(CXXFLAGS) -o $@ $<

#$(INTDIR)/%.o: $(COMMONDIR)/%.cpp
#	$(CCX) $(CXXFLAGS) -o $@ $<

clean:
	rm -rf $(CONTENT_ID).pkg sce_sys/param.sfo build eboot.bin
