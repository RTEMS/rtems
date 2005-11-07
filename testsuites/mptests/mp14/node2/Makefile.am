##
## $Id$
##

NODE = 2
TEST = mp14
PGM = ${ARCH}/$(TEST)-node$(NODE).exe

MANAGERS = io mp event partition semaphore timer message

noinst_LIBRARIES = lib.a
lib_a_SOURCES = ../exit.c ../init.c ../delay.c ../evtask1.c ../evtmtask.c \
    ../pttask1.c ../smtask1.c ../msgtask1.c system.h

DOCTYPES = doc scn
DOCS = $(DOCTYPES:%=$(TEST).%)

SRCS = $(DOCS)
OBJS = $(lib_a_OBJECTS)

PRINT_SRCS = $(DOCS)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/amcompile.am
include $(top_srcdir)/../automake/leaf.am
include $(top_srcdir)/mptests.am

AM_CPPFLAGS += -DNODE_NUMBER=$(NODE) -I$(srcdir)/..
AM_CPPFLAGS += -DMAX_LONG_TEST_DURATION=$(MAX_LONG_TEST_DURATION)

if HAS_MP

#
# (OPTIONAL) Add local stuff here using +=
#

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all-local: $(TMPINSTALL_FILES)
else
all-local:
endif

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
