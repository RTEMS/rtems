##
## $Id$
##


VPATH = @srcdir@:@srcdir@/../../../shared

PGM = $(ARCH)/startup.rel

#C_PIECES = bspclean bsplibc bsppost bspstart bootcard main sbrk setvec \
#    gnatinstallhandler
C_FILES = bspclean.c bsplibc.c bsppost.c bspstart.c bootcard.c cntrltbl.c \
    exit.c fault.c flttbl.c frmstr.c intrtbl.c kkprintf.c main.c nmi.c \
    nulsystbl.c prcb.c rom_cntrltbl.c rom_ibr.c rom_prcb.c sbrk.c sctns.c \
    setvec.c systbl.c

noinst_HEADERS = asmfault.h asmstub.h cntrltbl.h fault.h faultret.h flttbl.h \
    frmstr.h i960.h ihandler.h intrtbl.h main.h memchnl.h pmc901_memmap.h \
    prcb.h rom_ibr.h rommon.h sctns.h systbl.h time.h

S_FILES = asmfault.S asmstub.S ihandler.S

OBJS = $(C_FILES:%.c=$(ARCH)/%.o) $(S_FILES:%.S=$(ARCH)/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

bsplib_DATA = linkcmds

$(PROJECT_RELEASE)/lib/linkcmds: linkcmds
	$(INSTALL_DATA) $< $@

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/linkcmds

all-local: $(ARCH) $(OBJS) $(PGM) $(TMPINSTALL_FILES)

.PRECIOUS: $(PGM)

EXTRA_DIST = asmfault.S asmfault.h asmstub.S asmstub.h bspstart.c cntrltbl.c \
    cntrltbl.h dram.ld exit.c fault.c fault.h faultret.h flttbl.c flttbl.h \
    frmstr.c frmstr.h i960.h ihandler.S ihandler.h intrtbl.c intrtbl.h \
    kkprintf.c linkcmds main.h memchnl.h nmi.c nulsystbl.c pmc901_memmap.h \
    prcb.c prcb.h rom.ld rom_cntrltbl.c rom_ibr.c rom_ibr.h rom_prcb.c \
    rommon.h rxgen_romld.S sctns.c sctns.h setvec.c systbl.c systbl.h time.h

include $(top_srcdir)/../../../../../../automake/local.am
