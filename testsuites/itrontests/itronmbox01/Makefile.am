##
## $Id$
##

PGM = ${ARCH}/itronmbox01.exe

MANAGERS = all

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c system.h

DOCS = itronmbox01.doc itronmbox01.scn

OBJS = $(lib_a_OBJECTS)


include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

project_bspdir=$(PROJECT_ROOT)

$(project_bspdir)/tests/$(dirstamp):
	@$(mkdir_p) $(project_bspdir)/tests
	@: > $(project_bspdir)/tests/$(dirstamp)

$(project_bspdir)/tests/itronmbox01.exe: $(PGM) $(project_bspdir)/tests/$(dirstamp)
	$(INSTALL_PROGRAM) $< $@

$(project_bspdir)/tests/screens/itrontests/$(dirstamp):
	@$(mkdir_p) $(project_bspdir)/tests/screens/itrontests
	@: > $(project_bspdir)/tests/screens/itrontests/$(dirstamp)

$(project_bspdir)/tests/screens/itrontests/itronmbox01.scn: itronmbox01.scn $(project_bspdir)/tests/screens/itrontests/$(dirstamp)
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES = \
$(project_bspdir)/tests/itronmbox01.exe \
$(project_bspdir)/tests/screens/itrontests/itronmbox01.scn

$(PGM): ${ARCH}/$(dirstamp)



AM_CPPFLAGS += -I$(top_srcdir)/../support/include

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
