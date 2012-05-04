include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_tm01
ada_tm01_SOURCES = tm01.adb config.h tmtest.adb tmtest.ads
ada_tm01_SOURCES += ../../support/init.c

ada_tm01$(EXEEXT): tm01.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
