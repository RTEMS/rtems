ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro)
endif
 
export TOPDIR	:=	$(CURDIR)

export DSWIFI_MAJOR	:= 0
export DSWIFI_MINOR	:= 3
export DSWIFI_REVISION	:= 4

VERSION	:=	$(DSWIFI_MAJOR).$(DSWIFI_MINOR).$(DSWIFI_REVISION)
 
.PHONEY: release debug clean all

all: include/dswifi_version.h release debug 

include/dswifi_version.h : makefile
	@echo "#ifndef _dswifi_version_h_" > $@
	@echo "#define _dswifi_version_h_" >> $@
	@echo >> $@
	@echo "#define DSWIFI_MAJOR    $(DSWIFI_MAJOR)" >> $@
	@echo "#define DSWIFI_MINOR    $(DSWIFI_MINOR)" >> $@
	@echo "#define DSWIFI_REVISION $(DSWIFI_REVISION)" >> $@
	@echo >> $@
	@echo '#define DSWIFI_VERSION "'$(DSWIFI_MAJOR).$(DSWIFI_MINOR).$(DSWIFI_REVISION)'"' >> $@
	@echo >> $@
	@echo "#endif // _dswifi_version_h_" >> $@


#-------------------------------------------------------------------------------
release: lib
#-------------------------------------------------------------------------------
	$(MAKE) -C arm9 BUILD=release
	$(MAKE) -C arm7 BUILD=release
 
#-------------------------------------------------------------------------------
debug: lib
#-------------------------------------------------------------------------------
	$(MAKE) -C arm9 BUILD=debug
	$(MAKE) -C arm7 BUILD=debug

#-------------------------------------------------------------------------------
lib:
#-------------------------------------------------------------------------------
	mkdir lib
 
#-------------------------------------------------------------------------------
clean:
#-------------------------------------------------------------------------------
	@$(MAKE) -C arm9 clean
	@$(MAKE) -C arm7 clean

#-------------------------------------------------------------------------------
dist-src: clean
#-------------------------------------------------------------------------------
	@tar --exclude=*CVS* -cvjf dswifi-src-$(VERSION).tar.bz2 arm7 arm9 common include makefile dswifi_license.txt 

#-------------------------------------------------------------------------------
dist-bin: all
#-------------------------------------------------------------------------------
	@tar --exclude=*CVS* -cvjf dswifi-$(VERSION).tar.bz2 include lib dswifi_license.txt

dist: dist-bin dist-src

#-------------------------------------------------------------------------------
install: dist-bin
#-------------------------------------------------------------------------------
	bzip2 -cd dswifi-$(VERSION).tar.bz2 | tar -xv -C $(DEVKITPRO)/libnds

