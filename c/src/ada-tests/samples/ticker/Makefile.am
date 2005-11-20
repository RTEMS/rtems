## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

ticker$(EXEEXT): ticker.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

init.$(OBJEXT): ../../support/init.c
	$(COMPILE.c) -I$(srcdir) -c $<

noinst_PROGRAMS = ticker

ticker_SOURCES = ticker.adb config.h sptest.adb sptest.ads

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = ticker.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
