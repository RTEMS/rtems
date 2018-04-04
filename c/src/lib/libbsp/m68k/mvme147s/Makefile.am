ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =


## Zilog component header files

start.$(OBJEXT): ../../m68k/shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../shared/bspclean.c \
    ../../shared/bsppredriverhook.c \
    startup/bspstart.c ../../shared/bootcard.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/setvec.c \
    ../../shared/bspgetworkarea.c ../../shared/setvec.c \
    ../../shared/sbrk.c ../../shared/gnatinstallhandler.c
librtemsbsp_a_SOURCES += ../../shared/bspreset.c
# clock
librtemsbsp_a_SOURCES += ../mvme147/clock/ckinit.c
# console
librtemsbsp_a_SOURCES += ../mvme147/console/console.c \
    ../../shared/dummy_printk_support.c
# timer
librtemsbsp_a_SOURCES += ../mvme147/timer/timer.c ../mvme147/timer/timerisr.S

if HAS_MP
# shmsupp
librtemsbsp_a_SOURCES += shmsupp/addrconv.c shmsupp/getcfg.c shmsupp/lock.c \
    shmsupp/mpisr.c
endif

librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/m68kidle.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/memProbe.c


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/m68k/mvme147s/headers.am
