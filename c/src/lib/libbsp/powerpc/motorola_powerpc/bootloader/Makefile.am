##
## $Id$
##

## NOTE: This Makefile.am applies automake compilation rules and does not 
## apply RTEMS's compilation rules.

AUTOMAKE_OPTIONS = no-exeext

VPATH = @srcdir@:@srcdir@/../../shared/bootloader:@srcdir@/../../shared/console

noinst_PROGRAMS = bootloader.$(OBJEXT)

## IMPORTANT: head.S must be first, T. Straumann 12/17/2001
bootloader_o_SOURCES = head.S exception.S em86real.S
bootloader_o_SOURCES += misc.c pci.c zlib.c mm.c em86.c polled_io.c lib.c
bootloader_o_SOURCES += bootldr.h zlib.h pci.h keyboard.h

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg

NM = @NM@
LD = @LD@

# ----
DEFAULT_INCLUDES =

AM_CPPFLAGS = -D__BOOT__ -DDEBUG
AM_CFLAGS = \
    $(GCC_SPECS) -specs bsp_specs -qrtems -mrelocatable \
    -msoft-float -mstrict-align -fno-builtin -Wall -mmultiple \
    -mstring -O2 -fomit-frame-pointer -ffixed-r13 -mno-sdata \
    $(CPU_CFLAGS)
AM_CCASFLAGS = $(AM_CPPFLAGS) \
    $(GCC_SPECS) -specs bsp_specs -qrtems -mrelocatable \
    -DASM $(CPU_CFLAGS)

#
# CAUTION :
#
# As we use very specific compilation options in this directory
# we shall not use any other code. This includes the newlib libc.a
# as well as other code located in .$(OBJEXT) files in mcp750 directory.
#
# NEVER remove lib.c. You have been warned...
#
bootloader.$(OBJEXT): $(bootloader_o_OBJECTS)
	$(LD) -r -o $@ $(bootloader_o_OBJECTS)
	$(NM)  $@ | grep ' U '
	@echo "Every symbol listed should be defined in @srcdir@/ppcboot.lds"

ppcbootdir = $(bsplibdir)
ppcboot_DATA = bootloader.$(OBJEXT)
ppcboot_DATA += ../../shared/bootloader/ppcboot.lds

# FIXME: Tmp-install stuff to make mcp750.cfg's make-exe happy.
# As mcp750.cfg's make-exe doesn't work outside of the source-tree. 
# We might consider to use this directory directly for in-source-tree 
# building, instead.
$(PROJECT_RELEASE)/lib/%: %
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES = $(PROJECT_RELEASE)/lib/bootloader.$(OBJEXT) \
    $(PROJECT_RELEASE)/lib/ppcboot.lds

all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
