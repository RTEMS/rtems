## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm23.exe
tm23_exe_SOURCES = tm23.adb config.h tmtest.adb tmtest.ads
tm23_exe_SOURCES += ../../support/init.c

tm23.exe: tm23.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
