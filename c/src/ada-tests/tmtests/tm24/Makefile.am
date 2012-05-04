include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_tm24
ada_tm24_SOURCES = tm24.adb config.h tmtest.adb tmtest.ads
ada_tm24_SOURCES += ../../support/init.c

ada_tm24$(EXEEXT): tm24.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
