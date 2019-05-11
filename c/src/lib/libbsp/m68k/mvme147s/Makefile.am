ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/m68k/mvme147s/start/bsp_specs

noinst_PROGRAMS =


## Zilog component header files

start.$(OBJEXT): ../../../../../../bsps/m68k/shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mvme147s/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/setvec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/setvec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c
# clock
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mvme147/clock/ckinit.c
# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mvme147/console/console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/printk-dummy.c
# timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mvme147/btimer/btimer.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mvme147/btimer/timerisr.S

if HAS_MP
# shmsupp
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mvme147s/mpci/addrconv.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mvme147s/mpci/getcfg.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mvme147s/mpci/lock.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mvme147s/mpci/mpisr.c
endif

librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/m68kidle.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/memProbe.c

include $(srcdir)/../../../../../../bsps/shared/irq-default-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/m68k/mvme147s/headers.am
