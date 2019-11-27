include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_tm09
ada_tm09_SOURCES = tm09.adb config.h tmtest.adb tmtest.ads
ada_tm09_SOURCES += init.c

ada_tm09$(EXEEXT): tm09.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

