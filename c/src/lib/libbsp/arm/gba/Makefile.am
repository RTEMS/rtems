##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/arm_mode_bits.h
include_HEADERS += include/asm_macros.h
include_HEADERS += include/gba_registers.h
include_HEADERS += include/conio.h
include_HEADERS += include/tm27.h
include_HEADERS += include/gba.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_HEADERS += ../../shared/include/coverhd.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/start.S
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =

# some objects have to be forced together to ensure they are ALWAYS
# the ones linked into the application executable.  Every application
# needs startup/bspstart.c, so we will force in the others
noinst_PROGRAMS += gbaoverrides.rel
gbaoverrides_rel_SOURCES = startup/bspstart.c startup/cpu.c startup/cpu_asm.S
gbaoverrides_rel_CPPFLAGS = $(AM_CPPFLAGS)
gbaoverrides_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# startup
libbsp_a_SOURCES += ../../shared/bsplibc.c ../../shared/bsppost.c \
    startup/bspgetworkarea.c ../../shared/bsppretaskinghook.c \
    ../../shared/bsppredriverhook.c ../../shared/bspclean.c \
    startup/bspreset.c ../../shared/bootcard.c ../../shared/sbrk.c \
    ../../shared/gnatinstallhandler.c
# clock
libbsp_a_SOURCES += clock/clockdrv.c
# console
libbsp_a_SOURCES += console/conio.c console/console.c \
    console/defaultfont.c
# timer
libbsp_a_SOURCES += timer/timer.c

include_HEADERS += irq/irq.h
# irq
libbsp_a_SOURCES += irq/irq.c irq/bsp_irq_init.c irq/irq_init.c \
    irq/bsp_irq_asm.S irq/irq_asm.S

libbsp_a_LIBADD = gbaoverrides.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
