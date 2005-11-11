##
## $Id$
##

TEST = rtems++

MANAGERS = event io msg sem signal timer rate_monotonic

if HAS_CXX
noinst_LIBRARIES = lib.a
lib_a_SOURCES = Init.cc Task1.cc Task2.cc Task3.cc System.h
endif

DOCS = $(TEST).scn $(TEST).doc



PGM = ${ARCH}/$(TEST).exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

project_bspdir=$(PROJECT_ROOT)

$(project_bspdir)/tests/$(dirstamp):
	@$(mkdir_p) $(project_bspdir)/tests
	@: > $(project_bspdir)/tests/$(dirstamp)

$(project_bspdir)/tests/$(TEST).exe: $(PGM) $(project_bspdir)/tests/$(dirstamp)
	$(INSTALL_PROGRAM) $< $@

TMPINSTALL_FILES = $(project_bspdir)/tests/$(TEST).exe

$(PGM): ${ARCH}/$(dirstamp)

#
# (OPTIONAL) Add local stuff here using +=
#

LD_LIBS = -lrtems++

if HAS_CXX

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-cxx-exe)

all-local: $(TMPINSTALL_FILES)
else
all-local:
endif

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
