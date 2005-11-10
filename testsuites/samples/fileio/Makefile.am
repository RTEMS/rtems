##
##  Makefile.am,v 1.9 2002/08/11 05:18:59 ralf Exp
##

TEST = fileio
PGM = ${ARCH}/$(TEST).exe

MANAGERS = io message semaphore

if LIBCHIP
noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c system.h
OBJS = $(lib_a_OBJECTS)
endif

DOCS = $(TEST).doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am
include $(top_srcdir)/sample.am

#
# (OPTIONAL) Add local stuff here using +=
#

if LIBCHIP
all-local: $(TMPINSTALL_FILES)

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)
endif

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
