##
## $Id$
##

PGM = $(ARCH)/tty_drv.rel

include_HEADERS = tty_drv.h

C_FILES = tty_drv.c

tty_drv_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(tty_drv_rel_OBJECTS)
	$(make-rel)

all-local: $(tty_drv_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = tty_drv.c tty_drv.h

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)

$(PROJECT_INCLUDE)/tty_drv.h: tty_drv.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/tty_drv.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/tty_drv.h

include $(top_srcdir)/../../../automake/local.am
