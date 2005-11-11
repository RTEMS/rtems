##
## $Id$
##

NODE = 1
PGM = $(ARCH)/mp13-node$(NODE).exe

MANAGERS = io mp message semaphore

noinst_LIBRARIES = lib.a
lib_a_SOURCES = ../init.c ../task1.c ../task2.c ../system.h

DOCS = mp13-node$(NODE).doc mp13-node$(NODE).scn

OBJS = $(lib_a_OBJECTS)


include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

project_bspdir=$(PROJECT_ROOT)

$(project_bspdir)/tests/$(dirstamp):
	@$(mkdir_p) $(project_bspdir)/tests
	@: > $(project_bspdir)/tests/$(dirstamp)

$(project_bspdir)/tests/mp13--node$(NODE).exe: $(PGM) $(project_bspdir)/tests/$(dirstamp)
	$(INSTALL_PROGRAM) $< $@

$(project_bspdir)/tests/screens/mptests/node$(NODE)/$(dirstamp):
	@$(mkdir_p) $(project_bspdir)/tests/screens/mptests/node$(NODE)
	@: > $(project_bspdir)/tests/screens/mptests/node$(NODE)/$(dirstamp)

$(project_bspdir)/tests/screens/mptests/node$(NODE)/mp13-node$(NODE).scn: mp13-node$(NODE).scn $(project_bspdir)/tests/screens/mptests/node$(NODE)/$(dirstamp)
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES = \
$(project_bspdir)/tests/mp13--node$(NODE).exe \
$(project_bspdir)/tests/screens/mptests/node$(NODE)/mp13-node$(NODE).scn

$(PGM): ${ARCH}/$(dirstamp)

AM_CPPFLAGS += -DNODE_NUMBER=$(NODE) -I$(srcdir)/..

if HAS_MP

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all-local: $(TMPINSTALL_FILES)
else
all-local:
endif

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
