## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../tests/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm23
tm23_SOURCES = tm23.adb config.h tmtest.adb tmtest.ads
tm23_SOURCES += ../../support/init.c

tm23$(EXEEXT): tm23.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../tests/automake/local.am
