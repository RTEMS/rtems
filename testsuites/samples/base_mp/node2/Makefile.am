##
##  $Id$
##

PGM = ${ARCH}/base_mp-node2.exe

MANAGERS = io mp

noinst_LIBRARIES = lib.a
lib_a_SOURCES = ../init.c ../apptask.c ../system.h

DOCS = base_mp-node2.doc base_mp-node2.scn

OBJS = $(lib_a_OBJECTS)


include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

project_bspdir=$(PROJECT_ROOT)

$(project_bspdir)/samples/$(dirstamp):
	@$(mkdir_p) $(project_bspdir)/samples
	@: > $(project_bspdir)/samples/$(dirstamp)
$(project_bspdir)/tests/$(dirstamp):
	@$(mkdir_p) $(project_bspdir)/tests
	@: > $(project_bspdir)/tests/$(dirstamp)

$(project_bspdir)/samples/base_mp-node2.exe: $(PGM) $(project_bspdir)/samples/$(dirstamp)
	$(INSTALL_PROGRAM) $< $@

$(project_bspdir)/tests/base_mp-node2.exe: $(PGM) $(project_bspdir)/tests/$(dirstamp)
	$(INSTALL_PROGRAM) $< $@

TMPINSTALL_FILES = $(project_bspdir)/tests/base_mp-node2.exe
TMPINSTALL_FILES += $(project_bspdir)/samples/base_mp-node2.exe

$(PGM): ${ARCH}/$(dirstamp)

AM_CPPFLAGS += -DNODE_NUMBER=2 -I.


${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
