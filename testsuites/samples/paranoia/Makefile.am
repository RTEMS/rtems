##
##  $Id$
##

PGM = ${ARCH}/paranoia.exe

MANAGERS = all

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c paranoia.c system.h

DOCS = paranoia.doc

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

$(project_bspdir)/samples/paranoia.exe: $(PGM) $(project_bspdir)/samples/$(dirstamp)
	$(INSTALL_PROGRAM) $< $@

$(project_bspdir)/tests/paranoia.exe: $(PGM) $(project_bspdir)/tests/$(dirstamp)
	$(INSTALL_PROGRAM) $< $@

TMPINSTALL_FILES = $(project_bspdir)/tests/paranoia.exe
TMPINSTALL_FILES += $(project_bspdir)/samples/paranoia.exe

$(PGM): ${ARCH}/$(dirstamp)

#  -DCYGNUS   turns on more verbose msgs

AM_CPPFLAGS += -DNOSIGNAL -DCYGNUS -DNOMAIN

LD_LIBS = -lm

all-local: $(TMPINSTALL_FILES)

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
