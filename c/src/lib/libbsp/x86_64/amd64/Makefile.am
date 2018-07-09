ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/x86_64/amd64/start/bsp_specs

noinst_PROGRAMS =

project_lib_DATA = linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
#  XXX: We may want a custom bsp_work_area_initialize to detect memory size like
#  the i386
#
# FreeBSD's bootloader may leave a bootinfo structure for the kernel to find later:
# http://fxr.watson.org/fxr/source/i386/include/bootinfo.h?v=FREEBSD11#L48
#
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/x86_64/amd64/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/x86_64/amd64/start/start.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c
# clock
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/clock/clock-simidle.c
# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/x86_64/amd64/console/console.c
# timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/btimer/btimer-stub.c
# cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/x86_64/amd64/headers.am
