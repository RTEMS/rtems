## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/lib.am

SUBDIRS = rtems

AM_CPPFLAGS = -DCPU_SYNC_IO $(LIBC_DEFINES)

# C source names, if any, go here -- minus the .c
C_FILES = cpu.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

REL = $(ARCH)/rtems-cpu.rel

$(REL): $(C_O_FILES)
	$(make-rel)

all-local: $(ARCH) $(REL) $(TMPINSTALL_FILES)

EXTRA_DIST = \
cpu.c

include $(top_srcdir)/../../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../../automake/local.am
