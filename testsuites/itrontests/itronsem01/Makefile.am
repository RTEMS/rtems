##
## $Id$
##

TEST = itronsem01
PGM = ${ARCH}/$(TEST).exe

MANAGERS = all

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c

lib_a_SOURCES += system.h

DOCTYPES = doc scn
DOCS = $(DOCTYPES:%=$(TEST).%)

OBJS = $(lib_a_OBJECTS)


include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am
include $(top_srcdir)/itrontests.am

#
# (OPTIONAL) Add local stuff here using +=
#

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
