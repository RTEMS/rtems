include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_tm15
ada_tm15_SOURCES = tm15.adb config.h tmtest.adb tmtest.ads
ada_tm15_SOURCES += ../../support/init.c

ada_tm15$(EXEEXT): tm15.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
