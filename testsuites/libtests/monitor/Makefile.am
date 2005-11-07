##
## $Id$
##

TEST = monitor

MANAGERS = dpmem event io msg mp part region sem signal timer rate_monotonic \
    ext

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c system.h

DOCTYPES = scn
DOCS = #$(DOCTYPES:%=$(TEST).%)

SRCS = $(DOCS)
OBJS = $(lib_a_OBJECTS)

PRINT_SRCS = $(DOCS)

PGM = ${ARCH}/$(TEST).exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am
include $(top_srcdir)/libtests.am

#
# (OPTIONAL) Add local stuff here using +=
#

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
