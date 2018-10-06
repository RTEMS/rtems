## NOTE: This Makefile.am does not apply RTEMS's compilation rules.

AUTOMAKE_OPTIONS = no-exeext

noinst_PROGRAMS = bootloader

## IMPORTANT: head.S must be first, T. Straumann 12/17/2001
bootloader_SOURCES =
bootloader_SOURCES += ../../../../../../../bsps/powerpc/motorola_powerpc/bootloader/head.S
bootloader_SOURCES += ../../../../../../../bsps/powerpc/motorola_powerpc/bootloader/exception.S
bootloader_SOURCES += ../../../../../../../bsps/powerpc/motorola_powerpc/bootloader/em86real.S
bootloader_SOURCES += ../../../../../../../bsps/powerpc/motorola_powerpc/bootloader/misc.c
bootloader_SOURCES += ../../../../../../../bsps/powerpc/motorola_powerpc/bootloader/pci.c
bootloader_SOURCES += ../../../../../../../bsps/powerpc/motorola_powerpc/bootloader/zlib.c
bootloader_SOURCES += ../../../../../../../bsps/powerpc/motorola_powerpc/bootloader/mm.c
bootloader_SOURCES += ../../../../../../../bsps/powerpc/motorola_powerpc/bootloader/em86.c
bootloader_SOURCES += ../../../../../../../bsps/powerpc/motorola_powerpc/console/polled_io.c
bootloader_SOURCES += ../../../../../../../bsps/powerpc/motorola_powerpc/bootloader/lib.c

# ----
# Remove references to EABI when compiling bootloader
BOOTLOADER_CPU_CFLAGS=$(subst -msdata=eabi,,$(subst -meabi,,$(CPU_CFLAGS)))
AM_CPPFLAGS = -D__BOOT__ @RTEMS_CPPFLAGS@ @RTEMS_BSP_CPPFLAGS@
AM_CFLAGS = -mrelocatable -msoft-float \
    -mstrict-align -fno-builtin -Wall -mmultiple -mstring -O2 \
    -fomit-frame-pointer -ffixed-r13 -mno-sdata $(BOOTLOADER_CPU_CFLAGS)
AM_CCASFLAGS = $(AM_CPPFLAGS) \
    -mrelocatable -DASM $(BOOTLOADER_CPU_CFLAGS)

#
# CAUTION :
#
# As we use very specific compilation options in this directory
# we shall not use any other code. This includes the newlib libc.a
# as well as other code located in .$(OBJEXT) files in mcp750 directory.
#
# NEVER remove lib.c. You have been warned...
#
bootloader$(EXEEXT): $(bootloader_OBJECTS)
	$(LD) -r -o $@ $(bootloader_OBJECTS)
	$(NM)  $@ | grep ' U '
	@echo "Every symbol listed should be defined in @srcdir@/ppcboot.lds"
bootloader.$(OBJEXT): bootloader$(EXEEXT)
	cp $< $@

project_lib_DATA = bootloader.$(OBJEXT)
project_lib_DATA += ../../../../../../../bsps/powerpc/motorola_powerpc/bootloader/ppcboot.lds

all-local: $(TMPINSTALL_FILES)

TMPINSTALL_FILES =

$(PROJECT_LIB)/$(dirstamp):
	@$(MKDIR_P) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)
TMPINSTALL_FILES += $(PROJECT_LIB)/$(dirstamp)

$(PROJECT_LIB)/bootloader.$(OBJEXT): bootloader.$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/bootloader.$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/bootloader.$(OBJEXT)

$(PROJECT_LIB)/ppcboot.lds: ../../../../../../../bsps/powerpc/motorola_powerpc/bootloader/ppcboot.lds $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/ppcboot.lds
TMPINSTALL_FILES += $(PROJECT_LIB)/ppcboot.lds

CLEANFILES = $(TMPINSTALL_FILES)

