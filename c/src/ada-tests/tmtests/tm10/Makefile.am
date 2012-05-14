include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_tm10
ada_tm10_SOURCES = tm10.adb config.h tmtest.adb tmtest.ads
ada_tm10_SOURCES += ../../support/init.c

ada_tm10$(EXEEXT): tm10.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
