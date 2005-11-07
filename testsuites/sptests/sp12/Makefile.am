##
## $Id$
##

TEST = sp12

MANAGERS = io semaphore

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c task1.c task2.c task3.c task4.c task5.c pridrv.c \
    pritask.c system.h

DOCTYPES = scn doc
DOCS = $(DOCTYPES:%=$(TEST).%)

SRCS =
OBJS = $(lib_a_OBJECTS)

PRINT_SRCS = $(DOCS)

PGM = ${ARCH}/$(TEST).exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am
include $(top_srcdir)/sptests.am

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
