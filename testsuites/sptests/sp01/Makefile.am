##
## $Id$
##

TEST = sp01

MANAGERS = io

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c task1.c system.h

DOCTYPES = scn doc
DOCS = $(DOCTYPES:%=$(TEST).%)

SRCS =
OBJS = $(lib_a_OBJECTS)

PRINT_SRCS = $(DOCS)

PGM = ${ARCH}/$(TEST).exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/amcompile.am
include $(top_srcdir)/../automake/leaf.am
include $(top_srcdir)/sptests.am

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
