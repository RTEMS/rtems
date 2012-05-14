include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

ada_nsecs$(EXEEXT): nsecs.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

init.$(OBJEXT): ../../support/init.c
	$(COMPILE.c) -I$(srcdir) -c $<

noinst_PROGRAMS = ada_nsecs

ada_nsecs_SOURCES = nsecs.adb config.h sptest.adb sptest.ads

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = ada_nsecs.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
