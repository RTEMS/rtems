## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

include $(top_srcdir)/ada.am

if EXPADA
noinst_PROGRAMS = tm27
endif
tm27_SOURCES = tm27.adb config.h tmtest.adb tmtest.ads

tm27$(EXEEXT): tm27.adb
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../automake/local.am
