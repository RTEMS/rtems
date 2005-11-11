##
## $Id$
##

PGM = $(ARCH)/mp14-node1.exe

MANAGERS = io mp event partition semaphore timer message

noinst_LIBRARIES = lib.a
lib_a_SOURCES = ../exit.c ../init.c ../delay.c ../evtask1.c ../evtmtask.c \
    ../pttask1.c ../smtask1.c ../msgtask1.c ../system.h

DOCS = mp14-node1.doc mp14-node1.scn

OBJS = $(lib_a_OBJECTS)


include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

project_bspdir=$(PROJECT_ROOT)

$(project_bspdir)/tests/$(dirstamp):
	@$(mkdir_p) $(project_bspdir)/tests
	@: > $(project_bspdir)/tests/$(dirstamp)

$(project_bspdir)/tests/mp14--node1.exe: $(PGM) $(project_bspdir)/tests/$(dirstamp)
	$(INSTALL_PROGRAM) $< $@

$(project_bspdir)/tests/screens/mptests/node1/$(dirstamp):
	@$(mkdir_p) $(project_bspdir)/tests/screens/mptests/node1
	@: > $(project_bspdir)/tests/screens/mptests/node1/$(dirstamp)

$(project_bspdir)/tests/screens/mptests/node1/mp14-node1.scn: mp14-node1.scn $(project_bspdir)/tests/screens/mptests/node1/$(dirstamp)
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES = \
$(project_bspdir)/tests/mp14--node1.exe \
$(project_bspdir)/tests/screens/mptests/node1/mp14-node1.scn

$(PGM): ${ARCH}/$(dirstamp)

AM_CPPFLAGS += -DNODE_NUMBER=1 -I$(srcdir)/..
AM_CPPFLAGS += -DMAX_LONG_TEST_DURATION=$(MAX_LONG_TEST_DURATION)

if HAS_MP

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all-local: $(TMPINSTALL_FILES)
else
all-local:
endif

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
