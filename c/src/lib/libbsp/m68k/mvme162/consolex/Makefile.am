##
## $Id$
##

PGM = $(ARCH)/consolex.rel

C_FILES = consolex.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include_HEADERS = consolex.h

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

all-local: $(OBJS) $(PGM) $(TMPINSTALL_FILES)

.PRECIOUS: $(PGM)

EXTRA_DIST = README cTest.c consolex.c consolex.h

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)

$(PROJECT_INCLUDE)/consolex.h: consolex.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/consolex.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/consolex.h

include $(top_srcdir)/../../../../automake/local.am
