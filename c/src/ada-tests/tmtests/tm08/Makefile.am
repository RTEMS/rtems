## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm08
tm08_SOURCES = tm08.adb config.h tmtest.adb tmtest.ads

tm08$(EXEEXT): tm08.adb
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../automake/local.am
