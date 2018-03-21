ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =


EXTRA_DIST += ../shared/start/start.S
start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

libbsp_a_SOURCES += \
	startup/bspstart.c \
	../../shared/bspclean.c ../../shared/sbrk.c ../../shared/setvec.c \
	../../shared/bootcard.c ../../shared/gnatinstallhandler.c \
	../../shared/getentropy-cpucounter.c \
	../../shared/bspgetworkarea.c
libbsp_a_SOURCES += ../../shared/bspreset.c

libbsp_a_SOURCES += console/console-io.c
libbsp_a_SOURCES += ../../../../../../bsps/bfin/shared/cache/cache.c
libbsp_a_SOURCES += ../../../../../../bsps/bfin/shared/interrupt.c

EXTRA_DIST += times

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/bfin/shared/shared.am
include $(srcdir)/../../../../../../bsps/bfin/eZKit533/headers.am
