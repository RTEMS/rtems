##
## $Id$
##


VPATH = @srcdir@:@srcdir@/../../../shared

PGM = $(ARCH)/startup.rel

C_FILES = bspclean.c bsplibc.c bsppost.c bspstart.c bootcard.c main.c sbrk.c \
    setvec.c inittlb.c gnatinstallhandler.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

S_FILES = idtmem.S idttlb.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.o)

OBJS = $(C_O_FILES) $(S_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

$(PROJECT_RELEASE)/lib/ghlinkcmds: ghlinkcmds
	$(INSTALL_DATA) $< $@

$(PROJECT_RELEASE)/lib/linkcmds: linkcmds
	$(INSTALL_DATA) $< $@

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile
TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/ghlinkcmds \
    $(PROJECT_RELEASE)/lib/linkcmds

all-local: $(ARCH) $(OBJS) $(PGM) $(TMPINSTALL_FILES)

.PRECIOUS: $(PGM)

EXTRA_DIST = bspclean.c bspstart.c ghlinkcmds idtmem.S idttlb.S inittlb.c \
    linkcmds setvec.c

include $(top_srcdir)/../../../../../../automake/local.am
