## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

include $(top_srcdir)/ada.am

if EXPADA
noinst_PROGRAMS = tm20
endif
tm20_SOURCES = tm20.adb config.h tmtest.adb tmtest.ads

tm20$(EXEEXT): tm20.adb
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../automake/local.am
