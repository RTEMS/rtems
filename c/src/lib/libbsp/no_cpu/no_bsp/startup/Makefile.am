## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

VPATH = @srcdir@:@srcdir@/../../../shared

PGM = ${ARCH}/startup.rel

## C source names
C_FILES = bspclean.c bsplibc.c bsppost.c bspstart.c main.c bootcard.c sbrk.c \
    setvec.c gnatinstallhandler.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): ${OBJS}
	$(make-rel)

$(PROJECT_RELEASE)/lib/linkcmds: linkcmds
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES = $(PROJECT_RELEASE)/lib/linkcmds

all-local: ${ARCH} $(PGM) $(TMPINSTALL_FILES)

EXTRA_DIST = bspclean.c bspstart.c main.c setvec.c linkcmds

include $(top_srcdir)/../../../../../../automake/local.am
