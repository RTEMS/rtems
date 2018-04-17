ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs



start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += startup/linkcmds.ram
dist_project_lib_DATA += startup/linkcmds.rom

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

SHGEN = $(PROJECT_TOPdir)/tools/cpu/sh/shgen

scitab.c: $(SHGEN)
	$(SHGEN) -H @CPU_CLOCK_RATE_HZ@ sci > $@
BUILT_SOURCES = scitab.c
CLEANFILES = scitab.c

# startup
librtemsbsp_a_SOURCES += startup/hw_init.c
librtemsbsp_a_SOURCES += ../../shared/bsppredriverhook.c
librtemsbsp_a_SOURCES += ../shared/startup/bspstart.c
librtemsbsp_a_SOURCES += ../../shared/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../../shared/sbrk.c
librtemsbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../shared/bsphwinit.c
librtemsbsp_a_SOURCES += ../../shared/bspreset.c
librtemsbsp_a_SOURCES += clock/ckinit.c
librtemsbsp_a_SOURCES += startup/cpu_asm.c
librtemsbsp_a_SOURCES += startup/ispsh7045.c
librtemsbsp_a_SOURCES += timer/timer.c
# scitab
librtemsbsp_a_SOURCES += scitab.c
# console
librtemsbsp_a_SOURCES += ../shared/console.c
librtemsbsp_a_SOURCES += console/config.c
librtemsbsp_a_SOURCES += console/sci.c
librtemsbsp_a_SOURCES += console/sci_termios.c
# debugio
librtemsbsp_a_SOURCES += ../../shared/dummy_printk_support.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/sh/gensh2/headers.am
