##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../aclocal

nobase_include_HEADERS = rtems/powerpc/cache.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/powerpc:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%: %
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES = $(PROJECT_INCLUDE) \
  $(PROJECT_INCLUDE)/rtems \
  $(PROJECT_INCLUDE)/rtems/powerpc \
  $(nobase_include_HEADERS:%=$(PROJECT_INCLUDE)/%)

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

include $(top_srcdir)/../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../automake/local.am
