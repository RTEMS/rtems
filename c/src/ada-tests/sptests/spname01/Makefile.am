include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_spname01
ada_spname01_SOURCES = spname01.adb config.h sptest.adb sptest.ads

ada_spname01$(EXEEXT): spname01.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

init.$(OBJEXT): ../../support/init.c
	$(COMPILE.c) -I$(srcdir) -c $<

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = ada_spname01.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
