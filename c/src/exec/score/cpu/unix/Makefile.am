##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I ../../../../../../aclocal

include  $(top_srcdir)/../../../../../../automake/multilib.am
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

SUBDIRS = rtems

AM_CPPFLAGS += -DCPU_SYNC_IO $(LIBC_DEFINES)

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
