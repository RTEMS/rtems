## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

VPATH = @srcdir@:@srcdir@/../../shared:@srcdir@/../../../shared

PGM = ${ARCH}/startup.rel

## C source names
C_FILES = hw_init.c bsplibc.c bsppost.c bspstart.c bspclean.c sbrk.c \
    bootcard.c main.c gnatinstallhandler.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PROJECT_RELEASE)/lib/linkcmds: linkcmds
	$(INSTALL_DATA) $< $@
 
PREINSTALL_FILES += $(PROJECT_RELEASE)/lib/linkcmds

AM_CPPFLAGS += -DHZ=$(HZ)

$(PGM): ${OBJS}
	$(make-rel)

all-local: ${ARCH} $(PREINSTALL_FILES) $(PGM)

EXTRA_DIST = $(C_FILES) linkcmds linkcmds.rom linkcmds.ram

include $(top_srcdir)/../../../../../../automake/local.am
