##
## $Id$
##

PGM = $(ARCH)/ictrl.rel

C_FILES = ictrl.c

include_HEADERS = ictrl.h

ictrl_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(ictrl_rel_OBJECTS)
	$(make-rel)

all-local: $(ictrl_rel_OBJECTS) $(PGM) $(TMPINSTALL_FILES)

.PRECIOUS: $(PGM)

EXTRA_DIST = ictrl.c ictrl.h

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)

$(PROJECT_INCLUDE)/ictrl.h: ictrl.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/ictrl.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/ictrl.h

include $(top_srcdir)/../../../automake/local.am
