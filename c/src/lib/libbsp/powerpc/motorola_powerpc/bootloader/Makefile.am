##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

VPATH = @srcdir@:@srcdir@/../../../shared:@srcdir@/../../shared/console:@srcdir@/../../shared/bootloader

LIB = $(ARCH)/bootloader.o
LDS = ppcboot.lds

C_FILES = misc.c pci.c zlib.c mm.c em86.c polled_io.c lib.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

H_FILES = bootldr.h zlib.h pci.h

S_FILES = head.S exception.S em86real.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.o)

OBJS = $(S_O_FILES) $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

## FIXME:
## ASFLAGS = -mrelocatable
CFLAGS_DEBUG_V = $(CFLAGS_OPTIMIZE_V)

AM_CPPFLAGS += -D__BOOT__ -DDEBUG -mrelocatable
AM_CFLAGS += -msoft-float -mstrict-align -fno-builtin -Wall -mmultiple \
    -mstring -O2 -fomit-frame-pointer -ffixed-r13 -mno-sdata

CLEANFILES += bootloader.o

#
# CAUTION :
#
# As we use very specific compilation options in this directory
# we shall not use any other code. This includes the newlib libc.a
# as well as other code located in .o files in mcp750 directory.
#
# NEVER remove lib.c. You have been warned...
#

$(LIB) : $(OBJS) 
	$(LD) -r -o $(ARCH)/bootloader.o $(OBJS)
	$(NM)  $(ARCH)/bootloader.o | grep ' U '
	echo "Every symbol listed should be defined in @srcdir@/ppcboot.lds"

$(PROJECT_RELEASE)/lib/bootloader.o : $(LIB)
	$(INSTALL_DATA) $< $@

$(PROJECT_RELEASE)/lib/ppcboot.lds : $(LDS)
	$(INSTALL_DATA) $< $@

all-local: $(ARCH) $(PROJECT_RELEASE)/lib/bootloader.o $(PROJECT_RELEASE)/lib/ppcboot.lds

include $(top_srcdir)/../../../../../../automake/local.am
