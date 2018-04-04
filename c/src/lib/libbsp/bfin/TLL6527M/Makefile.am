ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =


start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a

librtemsbsp_a_SOURCES = \
	startup/bspstart.c \
	../../shared/bspclean.c ../../shared/sbrk.c ../../shared/setvec.c \
	../../shared/bootcard.c ../../shared/gnatinstallhandler.c \
	../../shared/getentropy-cpucounter.c \
	../../shared/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../../shared/bspreset.c

librtemsbsp_a_SOURCES += console/console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/bfin/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/bfin/TLL6527M/start/interrupt.c


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/bfin/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/bfin/TLL6527M/headers.am
