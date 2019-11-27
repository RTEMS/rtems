include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am

include $(top_srcdir)/ada.am

AM_ADAFLAGS += -I$(srcdir)/..

noinst_PROGRAMS = mp07_ada_mp07_node1
mp07_ada_mp07_node1_SOURCES = mp07_node1.adb ../mptest.adb config.h
mp07_ada_mp07_node1_SOURCES += ../mptest.ads
mp07_ada_mp07_node1_SOURCES += init.c

mp07_ada_mp07_node1$(EXEEXT): mp07_node1.adb ../mptest.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = ada_mp07-node1.scn

