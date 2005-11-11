##
## $Id$
##

TEST = sp09

MANAGERS = io message dual_ported_memory event multi_processor region \
    semaphore signal rate_monotonic timer partition

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c delay.c isr.c screen01.c screen02.c screen03.c screen04.c \
    screen05.c screen06.c screen07.c screen08.c screen09.c screen10.c \
    screen11.c screen12.c screen13.c screen14.c task1.c task2.c task3.c \
    task4.c system.h

DOCS = $(TEST).scn $(TEST).doc

OBJS = $(lib_a_OBJECTS)


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

$(project_bspdir)/tests/screens/sptests/$(dirstamp):
	@$(mkdir_p) $(project_bspdir)/tests/screens/sptests
	@: > $(project_bspdir)/tests/screens/sptests/$(dirstamp)

$(project_bspdir)/tests/screens/sptests/$(TEST).scn: $(TEST).scn $(project_bspdir)/tests/screens/sptests/$(dirstamp)
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES = $(project_bspdir)/tests/$(TEST).exe
TMPINSTALL_FILES += $(project_bspdir)/tests/screens/sptests/$(TEST).scn

$(PGM): ${ARCH}/$(dirstamp)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
