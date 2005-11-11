##
##  $Id$
##

PGM = ${ARCH}/minimum.exe

MANAGERS = all

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c

DOCS = minimum.doc minimum.scn

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

$(project_bspdir)/samples/minimum.exe: $(PGM) $(project_bspdir)/samples/$(dirstamp)
	$(INSTALL_PROGRAM) $< $@

$(project_bspdir)/tests/minimum.exe: $(PGM) $(project_bspdir)/tests/$(dirstamp)
	$(INSTALL_PROGRAM) $< $@

TMPINSTALL_FILES = $(project_bspdir)/tests/minimum.exe
TMPINSTALL_FILES += $(project_bspdir)/samples/minimum.exe

$(PGM): ${ARCH}/$(dirstamp)

#
# (OPTIONAL) Add local stuff here using +=
#

all-local: $(TMPINSTALL_FILES)
# 	This test should not be executed

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
