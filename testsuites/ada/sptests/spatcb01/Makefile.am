include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_spatcb01
ada_spatcb01_SOURCES = spatcb01.adb config.h
ada_spatcb01_SOURCES += ../../support/init.c

ada_spatcb01$(EXEEXT): spatcb01.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = ada_spatcb01.scn

