include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_tmck
ada_tmck_SOURCES = tmck.adb config.h tmtest.adb tmtest.ads
ada_tmck_SOURCES += ../../support/init.c

ada_tmck$(EXEEXT): tmck.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
