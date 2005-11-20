## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp03
sp03_SOURCES = sp03.adb config.h sptest.adb sptest.ads
sp03_SOURCES += ../../support/init.c

sp03$(EXEEXT): sp03.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = sp03.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
