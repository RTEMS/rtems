##
## $Id$
##

## NOTE: This Makefile.am applies automake compilation rules and does not
## apply RTEMS's compilation rules.

AUTOMAKE_OPTIONS = no-exeext

noinst_PROGRAMS = bootloader

## IMPORTANT: head.S must be first, T. Straumann 12/17/2001
bootloader_SOURCES = $(top_srcdir)/../../powerpc/shared/bootloader/head.S \
    $(top_srcdir)/../../powerpc/shared/bootloader/exception.S \
    $(top_srcdir)/../../powerpc/shared/bootloader/em86real.S \
    $(top_srcdir)/../../powerpc/shared/bootloader/misc.c \
    $(top_srcdir)/../../powerpc/shared/bootloader/pci.c \
    $(top_srcdir)/../../powerpc/shared/bootloader/zlib.c \
    $(top_srcdir)/../../powerpc/shared/bootloader/mm.c \
    $(top_srcdir)/../../powerpc/shared/bootloader/em86.c \
    $(top_srcdir)/../../powerpc/shared/console/polled_io.c \
    $(top_srcdir)/../../powerpc/shared/bootloader/lib.c \
    $(top_srcdir)/../../powerpc/shared/bootloader/bootldr.h \
    $(top_srcdir)/../../powerpc/shared/bootloader/zlib.h \
    $(top_srcdir)/../../powerpc/shared/bootloader/pci.h \
    $(top_srcdir)/../../powerpc/shared/console/keyboard.h

NM = @NM@
LD = @LD@

# ----
DEFAULT_INCLUDES =

# Remove references to EABI when compiling bootloader
BOOTLOADER_CPU_FLAGS=$(subst -msdata=eabi,,$(subst -meabi,,$(CPU_CFLAGS)))
AM_CPPFLAGS = -D__BOOT__ -DDEBUG
AM_CFLAGS = $(GCC_SPECS) -specs bsp_specs -qrtems -mrelocatable -msoft-float \
    -mstrict-align -fno-builtin -Wall -mmultiple -mstring -O2 \
    -fomit-frame-pointer -ffixed-r13 -mno-sdata $(BOOTLOADER_CPU_CFLAGS)
AM_CCASFLAGS = $(AM_CPPFLAGS) $(GCC_SPECS) -specs bsp_specs -qrtems \
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
project_lib_DATA += ../../shared/bootloader/ppcboot.lds

# FIXME: Tmp-install stuff to make mcp750.cfg's make-exe happy.
# As mcp750.cfg's make-exe doesn't work outside of the source-tree.
# We might consider to use this directory directly for in-source-tree
# building, instead.

all-local: $(TMPINSTALL_FILES)

TMPINSTALL_FILES =

$(PROJECT_LIB)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)

$(PROJECT_LIB)/bootloader.$(OBJEXT): bootloader.$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/bootloader.$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/bootloader.$(OBJEXT)

$(PROJECT_LIB)/ppcboot.lds: ../../shared/bootloader/ppcboot.lds $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/ppcboot.lds
TMPINSTALL_FILES += $(PROJECT_LIB)/ppcboot.lds

include $(top_srcdir)/../../../../automake/local.am
