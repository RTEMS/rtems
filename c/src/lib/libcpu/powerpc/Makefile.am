##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include_rtems_powerpcdir = $(includedir)/rtems/powerpc
include_rtems_powerpc_HEADERS = rtems/powerpc/cache.h \
    rtems/powerpc/debugmod.h

## Check configure.in for conditions to set the conditionals below
if shared
SHARED_LIB = shared
endif

if mpc505
CPU_SUBDIR = mpc505
endif
if mpc6xx
CPU_SUBDIR = mpc6xx
endif
if mpc8xx
CPU_SUBDIR = mpc8xx
endif
if mpc8260
CPU_SUBDIR = mpc8260
endif
if ppc403
CPU_SUBDIR = ppc403
endif
if ppc405
## 403 and 405 chips use the same CPU sources...
CPU_SUBDIR = ppc403
endif

SUBDIRS = $(SHARED_LIB) $(CPU_SUBDIR) wrapup

all-local: $(TMPINSTALL_FILES)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/rtems/powerpc/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems/powerpc
	@: > $(PROJECT_INCLUDE)/rtems/powerpc/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/powerpc/cache.h: rtems/powerpc/cache.h $(PROJECT_INCLUDE)/rtems/powerpc/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/powerpc/cache.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/powerpc/cache.h

$(PROJECT_INCLUDE)/rtems/powerpc/debugmod.h: rtems/powerpc/debugmod.h $(PROJECT_INCLUDE)/rtems/powerpc/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/powerpc/debugmod.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/powerpc/debugmod.h

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
