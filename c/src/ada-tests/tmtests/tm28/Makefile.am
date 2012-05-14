include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_tm28
ada_tm28_SOURCES = tm28.adb config.h tmtest.adb tmtest.ads
ada_tm28_SOURCES += ../../support/init.c

ada_tm28$(EXEEXT): tm28.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
