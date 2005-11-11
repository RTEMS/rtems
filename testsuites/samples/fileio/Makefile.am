##
##  Makefile.am,v 1.9 2002/08/11 05:18:59 ralf Exp
##

PGM = ${ARCH}/fileio.exe

MANAGERS = io message semaphore

if LIBCHIP
noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c system.h
OBJS = $(lib_a_OBJECTS)
endif

DOCS = fileio.doc

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

$(project_bspdir)/samples/fileio.exe: $(PGM) $(project_bspdir)/samples/$(dirstamp)
	$(INSTALL_PROGRAM) $< $@

$(project_bspdir)/tests/fileio.exe: $(PGM) $(project_bspdir)/tests/$(dirstamp)
	$(INSTALL_PROGRAM) $< $@

TMPINSTALL_FILES = $(project_bspdir)/tests/fileio.exe
TMPINSTALL_FILES += $(project_bspdir)/samples/fileio.exe

$(PGM): ${ARCH}/$(dirstamp)

#
# (OPTIONAL) Add local stuff here using +=
#

if LIBCHIP
all-local: $(TMPINSTALL_FILES)

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)
endif

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
