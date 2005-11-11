##
## $Id$
##

PGM = $(ARCH)/mp09-node1.exe

MANAGERS = io mp message

noinst_LIBRARIES = lib.a
lib_a_SOURCES = ../init.c ../task1.c ../recvmsg.c ../sendmsg.c ../system.h

DOCS = mp09-node1.doc mp09-node1.scn

OBJS = $(lib_a_OBJECTS)


include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

project_bspdir=$(PROJECT_ROOT)

$(project_bspdir)/tests/$(dirstamp):
	@$(mkdir_p) $(project_bspdir)/tests
	@: > $(project_bspdir)/tests/$(dirstamp)

$(project_bspdir)/tests/mp09--node1.exe: $(PGM) $(project_bspdir)/tests/$(dirstamp)
	$(INSTALL_PROGRAM) $< $@

$(project_bspdir)/tests/screens/mptests/node1/$(dirstamp):
	@$(mkdir_p) $(project_bspdir)/tests/screens/mptests/node1
	@: > $(project_bspdir)/tests/screens/mptests/node1/$(dirstamp)

$(project_bspdir)/tests/screens/mptests/node1/mp09-node1.scn: mp09-node1.scn $(project_bspdir)/tests/screens/mptests/node1/$(dirstamp)
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES = \
$(project_bspdir)/tests/mp09--node1.exe \
$(project_bspdir)/tests/screens/mptests/node1/mp09-node1.scn

$(PGM): ${ARCH}/$(dirstamp)

AM_CPPFLAGS += -DNODE_NUMBER=1 -I$(srcdir)/..

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
