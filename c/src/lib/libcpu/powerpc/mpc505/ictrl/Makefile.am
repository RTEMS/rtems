##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

PGM = $(ARCH)/ictrl.rel

C_FILES = ictrl.c

include_HEADERS = ictrl.h

ictrl_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/compile.am
include $(top_srcdir)/../../../../../automake/lib.am

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(ictrl_rel_OBJECTS)
	$(make-rel)

TMPINSTALL_FILES += $(PROJECT_INCLUDE) \
   $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

all-local: $(ARCH) $(ictrl_rel_OBJECTS) $(PGM) $(TMPINSTALL_FILES)

.PRECIOUS: $(PGM)

EXTRA_DIST = ictrl.c ictrl.h

include $(top_srcdir)/../../../../../automake/local.am
