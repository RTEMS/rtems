##
## $Id$
##

ACLOCAL_AMFLAGS = -I aclocal

include $(top_srcdir)/automake/multilib.am

SUBDIRS = . ada score rtems sapi posix itron
SUBDIRS += libcsupport libblock libfs
SUBDIRS += libnetworking librpc
SUBDIRS += libmisc
SUBDIRS += wrapup

noinst_DATA = preinstall-stamp

preinstall-stamp:
	$(MAKE) $(AM_MAKEFLAGS) preinstall
	touch preinstall-stamp
CLEANFILES = preinstall-stamp

$(PROJECT_INCLUDE)/%.h: include/%.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE):
	@$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@

PREINSTALL_FILES = $(PROJECT_INCLUDE)

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = include/rtems/bspIo.h include/rtems/userenv.h \
  include/rtems/fs.h include/rtems/stdint.h

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems \
    $(include_rtems_HEADERS:include/%.h=$(PROJECT_INCLUDE)/%.h)

all-local: $(PREINSTALL_FILES)

AUTOMAKE_FILES = automake/compile.am automake/lib.am automake/local.am \
    automake/multilib.am automake/subdirs.am
EXTRA_DIST = $(AUTOMAKE_FILES)

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
