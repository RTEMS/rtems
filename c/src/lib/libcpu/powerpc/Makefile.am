##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include_rtems_powerpcdir = $(includedir)/rtems/powerpc
include_rtems_powerpc_HEADERS = rtems/powerpc/cache.h \
    rtems/powerpc/debugmod.h

SUBDIRS = shared mpc505 mpc6xx mpc8260 mpc8xx ppc403 wrapup

all-local: $(TMPINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/rtems/powerpc/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems/powerpc
	@: > $(PROJECT_INCLUDE)/rtems/powerpc/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/powerpc/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/powerpc/cache.h: rtems/powerpc/cache.h $(PROJECT_INCLUDE)/rtems/powerpc/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/powerpc/cache.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/powerpc/cache.h

$(PROJECT_INCLUDE)/rtems/powerpc/debugmod.h: rtems/powerpc/debugmod.h $(PROJECT_INCLUDE)/rtems/powerpc/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/powerpc/debugmod.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/powerpc/debugmod.h

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
