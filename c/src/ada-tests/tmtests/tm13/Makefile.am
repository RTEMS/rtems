include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_tm13
ada_tm13_SOURCES = tm13.adb config.h tmtest.adb tmtest.ads
ada_tm13_SOURCES += ../../support/init.c

ada_tm13$(EXEEXT): tm13.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
