ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =

project_lib_DATA = linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../shared/bspclean.c \
    ../../shared/bsppredriverhook.c \
    ../../shared/startup/bspgetworkarea.c \
    startup/bspstart.c ../../shared/bootcard.c ../../shared/sbrk.c \
    ../../shared/getentropy-cpucounter.c \
    startup/setvec.c ../../shared/gnatinstallhandler.c
librtemsbsp_a_SOURCES += ../../shared/bspreset.c
# clock
librtemsbsp_a_SOURCES += clock/ckinit.c
# console
librtemsbsp_a_SOURCES += console/console.c
# timer
librtemsbsp_a_SOURCES += timer/timer.c timer/timerisr.c
# shmsupp
librtemsbsp_a_SOURCES += shmsupp/addrconv.c shmsupp/getcfg.c shmsupp/lock.c \
    shmsupp/mpisr.c


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/no_cpu/no_bsp/headers.am
