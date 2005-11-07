##
##  $Id$
##

SAMPLE = pppd
PGM = ${ARCH}/$(SAMPLE).exe

MANAGERS = io event semaphore

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c pppdapp.c netconfig.h system.h

OBJS = $(lib_a_OBJECTS)

PRINT_SRCS = $(DOCS)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/amcompile.am
include $(top_srcdir)/../automake/leaf.am
include $(top_srcdir)/sample.am

#
# (OPTIONAL) Add local stuff here using +=
#

LD_LIBS = -lpppd

all-local: $(TMPINSTALL_FILES)

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
