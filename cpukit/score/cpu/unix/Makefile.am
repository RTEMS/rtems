##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/lib.am

SUBDIRS = rtems

AM_CPPFLAGS = -DCPU_SYNC_IO $(LIBC_DEFINES)

C_FILES = cpu.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

REL = $(ARCH)/rtems-cpu.rel

rtems_cpu_rel_OBJECTS = $(C_O_FILES)

$(REL): $(rtems_cpu_rel_OBJECTS)
	$(make-rel)

all-local: $(ARCH) $(rtems_cpu_rel_OBJECTS) $(REL)

.PRECIOUS: $(REL)

EXTRA_DIST = cpu.c

include $(top_srcdir)/../../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../../automake/local.am
