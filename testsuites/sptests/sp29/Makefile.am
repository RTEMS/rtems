##
## $Id$
##


MANAGERS = all

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c

DOCS = sp29.doc sp29.scn

OBJS = $(lib_a_OBJECTS)


PGM = ${ARCH}/sp29.exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

project_bspdir=$(PROJECT_ROOT)

$(project_bspdir)/tests/$(dirstamp):
	@$(mkdir_p) $(project_bspdir)/tests
	@: > $(project_bspdir)/tests/$(dirstamp)

$(project_bspdir)/tests/sp29.exe: $(PGM) $(project_bspdir)/tests/$(dirstamp)
	$(INSTALL_PROGRAM) $< $@

$(project_bspdir)/tests/screens/sptests/$(dirstamp):
	@$(mkdir_p) $(project_bspdir)/tests/screens/sptests
	@: > $(project_bspdir)/tests/screens/sptests/$(dirstamp)

$(project_bspdir)/tests/screens/sptests/sp29.scn: sp29.scn $(project_bspdir)/tests/screens/sptests/$(dirstamp)
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES = $(project_bspdir)/tests/sp29.exe
TMPINSTALL_FILES += $(project_bspdir)/tests/screens/sptests/sp29.scn

$(PGM): ${ARCH}/$(dirstamp)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
