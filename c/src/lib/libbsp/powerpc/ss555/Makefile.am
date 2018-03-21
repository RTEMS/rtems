ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h

EXTRA_DIST += times

EXTRA_DIST += ../../powerpc/shared/start/rtems_crti.S
rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = rtems_crti.$(OBJEXT)

project_lib_DATA += linkcmds

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# pclock
libbsp_a_SOURCES += clock/p_clock.c
# console
libbsp_a_SOURCES += console/console.c
# startup
libbsp_a_SOURCES += ../../shared/bspclean.c \
    ../../shared/bsppredriverhook.c \
    startup/bspstart.c ../../shared/bootcard.c startup/iss555.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/sbrk.c ../../shared/gnatinstallhandler.c start/start.S \
    ../../shared/bspgetworkarea.c
libbsp_a_SOURCES += ../../shared/bspreset.c
# tm27supp
libbsp_a_SOURCES += startup/tm27supp.c

libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/exceptions/ppc_exc_print.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/ppc-dec-timer.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/ss555/dev/clock.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/ss555/dev/console-generic.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/ss555/start/irq_asm.S
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/ss555/start/irq.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/ss555/start/irq_init.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/ss555/start/raw_exception.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/ss555/start/vectors_init.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/ss555/start/vectors.S

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared.am
include $(srcdir)/../../../../../../bsps/powerpc/ss555/headers.am
