## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm11
tm11_SOURCES = tm11.adb config.h tmtest.adb tmtest.ads

tm11$(EXEEXT): tm11.adb
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../automake/local.am
