##
##  $Id$
##

PGM = ${ARCH}/cdtest.exe

MANAGERS = all

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c main.cc system.h

DOCS = cdtest.scn

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

$(project_bspdir)/samples/cdtest.exe: $(PGM) $(project_bspdir)/samples/$(dirstamp)
	$(INSTALL_PROGRAM) $< $@

$(project_bspdir)/tests/cdtest.exe: $(PGM) $(project_bspdir)/tests/$(dirstamp)
	$(INSTALL_PROGRAM) $< $@

TMPINSTALL_FILES = $(project_bspdir)/tests/cdtest.exe
TMPINSTALL_FILES += $(project_bspdir)/samples/cdtest.exe

$(PGM): ${ARCH}/$(dirstamp)

# AM_CPPFLAGS += -DRTEMS_TEST_IO_STREAM

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-cxx-exe)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
