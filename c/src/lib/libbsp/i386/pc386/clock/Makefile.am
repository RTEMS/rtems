##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../shared/tod:@srcdir@/../../../shared

PGM = $(ARCH)/clock.rel

C_FILES = ckinit.c todcfg.c tod.c
clock_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include_HEADERS = tod.h

tod.h: $(top_srcdir)/../../shared/tod.h
	cp $< $@
CLEANFILES = tod

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE) \
    $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(clock_rel_OBJECTS)
	$(make-rel)

# the .rel file built here will be put into libbsp.a by
# 	../wrapup/Makefile

all-local: $(ARCH) $(clock_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = 

include $(top_srcdir)/../../../../../../automake/local.am
