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

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = include/rtems/bspIo.h include/rtems/userenv.h \
    include/rtems/fs.h include/rtems/stdint.h

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/rtems/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems
	@: > $(PROJECT_INCLUDE)/rtems/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/bspIo.h: include/rtems/bspIo.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/bspIo.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/bspIo.h

$(PROJECT_INCLUDE)/rtems/userenv.h: include/rtems/userenv.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/userenv.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/userenv.h

$(PROJECT_INCLUDE)/rtems/fs.h: include/rtems/fs.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/fs.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/fs.h

$(PROJECT_INCLUDE)/rtems/stdint.h: include/rtems/stdint.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/stdint.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/stdint.h

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
