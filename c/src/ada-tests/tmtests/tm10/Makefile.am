## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../tests/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm10
tm10_SOURCES = tm10.adb config.h tmtest.adb tmtest.ads
tm10_SOURCES += ../../support/init.c

tm10$(EXEEXT): tm10.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../tests/automake/local.am
