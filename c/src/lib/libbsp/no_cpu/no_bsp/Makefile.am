ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/no_cpu/no_bsp/start/bsp_specs

noinst_PROGRAMS =

project_lib_DATA = linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/no_cpu/no_bsp/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/no_cpu/no_bsp/start/setvec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c
# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/no_cpu/no_bsp/clock/ckinit.c
# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/no_cpu/no_bsp/console/console.c
# timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/no_cpu/no_bsp/btimer/btimer.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/no_cpu/no_bsp/btimer/timerisr.c
# shmsupp
librtemsbsp_a_SOURCES += ../../../../../../bsps/no_cpu/no_bsp/mpci/addrconv.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/no_cpu/no_bsp/mpci/getcfg.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/no_cpu/no_bsp/mpci/lock.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/no_cpu/no_bsp/mpci/mpisr.c

include $(srcdir)/../../../../../../bsps/shared/irq-default-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/no_cpu/no_bsp/headers.am
