##
## $Id$
##

PGM = $(ARCH)/mp14-node1.exe

MANAGERS = io mp event partition semaphore timer message

noinst_LIBRARIES = lib.a
lib_a_SOURCES = ../exit.c ../init.c ../delay.c ../evtask1.c ../evtmtask.c \
    ../pttask1.c ../smtask1.c ../msgtask1.c ../system.h

testsdir = $(libdir)/tests
scndir = $(testsdir)
dist_scn_DATA = mp14-node1.scn
docdir = $(testsdir)
dist_doc_DATA = mp14-node1.doc

OBJS = $(lib_a_OBJECTS)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

$(PGM): ${ARCH}/$(dirstamp)

AM_CPPFLAGS += -DNODE_NUMBER=1 -I$(srcdir)/..
AM_CPPFLAGS += -DMAX_LONG_TEST_DURATION=$(MAX_LONG_TEST_DURATION)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

tests_SCRIPTS = $(PGM)

$(PGM): $(OBJS) $(LINK_FILES)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
