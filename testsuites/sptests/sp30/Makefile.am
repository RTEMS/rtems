##
## $Id$
##


MANAGERS = all

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c resume.c task1.c system.h

DOCS = sp30.doc sp30.scn

OBJS = $(lib_a_OBJECTS)

PGM = ${ARCH}/sp30.exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

project_bspdir=$(PROJECT_ROOT)

$(project_bspdir)/tests/$(dirstamp):
	@$(mkdir_p) $(project_bspdir)/tests
	@: > $(project_bspdir)/tests/$(dirstamp)

$(project_bspdir)/tests/sp30.exe: $(PGM) $(project_bspdir)/tests/$(dirstamp)
	$(INSTALL_PROGRAM) $< $@

$(project_bspdir)/tests/screens/sptests/$(dirstamp):
	@$(mkdir_p) $(project_bspdir)/tests/screens/sptests
	@: > $(project_bspdir)/tests/screens/sptests/$(dirstamp)

$(project_bspdir)/tests/screens/sptests/sp30.scn: sp30.scn $(project_bspdir)/tests/screens/sptests/$(dirstamp)
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES = $(project_bspdir)/tests/sp30.exe
TMPINSTALL_FILES += $(project_bspdir)/tests/screens/sptests/sp30.scn

$(PGM): ${ARCH}/$(dirstamp)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
