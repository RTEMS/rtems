
ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

# include
DISTCLEANFILES             = include/bspopts.h

# start
rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = rtems_crti.$(OBJEXT)

project_lib_DATA += linkcmds

noinst_LIBRARIES = libbsp.a

# startup
libbsp_a_SOURCES = startup/bspclean.c \
                   ../shared/startup/zerobss.c \
                   ../../shared/bspgetworkarea.c \
                   startup/bspstart.c \
                   ../../shared/bootcard.c \
                   ../../shared/getentropy-cpucounter.c \
                   ../../shared/sbrk.c \
                   ../../shared/gnatinstallhandler.c \
                   ../shared/src/memcpy.c
libbsp_a_SOURCES += ../../shared/bspreset.c

# start
libbsp_a_SOURCES += start/start.S

# console
libbsp_a_SOURCES += startup/dummy_console.c \
                    ../../shared/dummy_printk_support.c

# irq
libbsp_a_SOURCES += irq/irq_init.c

# mmu
libbsp_a_SOURCES += mmu/mmu.c

libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/dev/clock-ppc403.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/dev/timer-ppc-dec.c


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions.am
include $(srcdir)/../../../../../../bsps/powerpc/virtex4/headers.am
