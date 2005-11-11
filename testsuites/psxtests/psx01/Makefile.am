##
## $Id$
##


MANAGERS = all

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c task.c system.h ../include/pmacros.h

DOCS = psx01.scn

OBJS = $(lib_a_OBJECTS)


PGM = ${ARCH}/psx01.exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

project_bspdir=$(PROJECT_ROOT)

$(project_bspdir)/tests/$(dirstamp):
	@$(mkdir_p) $(project_bspdir)/tests
	@: > $(project_bspdir)/tests/$(dirstamp)

$(project_bspdir)/tests/psx01.exe: $(PGM) $(project_bspdir)/tests/$(dirstamp)
	$(INSTALL_PROGRAM) $< $@

$(project_bspdir)/tests/screens/psxtests/$(dirstamp):
	@$(mkdir_p) $(project_bspdir)/tests/screens/psxtests
	@: > $(project_bspdir)/tests/screens/psxtests/$(dirstamp)

$(project_bspdir)/tests/screens/psxtests/psx01.scn: psx01.scn $(project_bspdir)/tests/screens/psxtests/$(dirstamp)
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES = $(project_bspdir)/tests/psx01.exe
TMPINSTALL_FILES += $(project_bspdir)/tests/screens/psxtests/psx01.scn

$(PGM): ${ARCH}/$(dirstamp)


AM_CPPFLAGS += -I$(top_srcdir)/include

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
