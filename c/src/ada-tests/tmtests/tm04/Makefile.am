## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm04
tm04_SOURCES = tm04.adb config.h tmtest.adb tmtest.ads

tm04$(EXEEXT): tm04.adb
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../automake/local.am
