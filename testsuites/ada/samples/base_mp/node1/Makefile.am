include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am

include $(top_srcdir)/ada.am

AM_ADAFLAGS += -I$(srcdir)/..
AM_CPPFLAGS += -I$(srcdir)/..

if EXPADA
noinst_PROGRAMS = ada_base_mp_node1
ada_base_mp_node1_SOURCES = init.c mptest-per_node_configuration.adb
endif

ada_base_mp_node1$(EXEEXT): ../mptest.adb init.o
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = ada_base_mp-node1.scn

