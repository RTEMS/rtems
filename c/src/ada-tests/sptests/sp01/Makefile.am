## $Id$

VPATH = @srcdir@:@srcdir@/../../support

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp01
sp01_SOURCES = sp01.adb config.h sptest.adb sptest.ads

sp01$(EXEEXT): sp01.adb init.o
	$(GNATCOMPILE) -margs -a $< -o $@

EXTRA_DIST = sp01.scn

include $(top_srcdir)/../../../automake/local.am
