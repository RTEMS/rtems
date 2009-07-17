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
include_HEADERS += ../../shared/include/tm27.h
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

# startup
libbsp_a_SOURCES += ../../shared/bsplibc.c ../../shared/bsppost.c \
    startup/bspgetworkarea.c ../../shared/bsppretaskinghook.c \
    ../../shared/bsppredriverhook.c ../../shared/bspclean.c \
    startup/bspreset.c ../../shared/bootcard.c ../../shared/sbrk.c \
    ../../shared/gnatinstallhandler.c \
    startup/bspstart.c
# clock
libbsp_a_SOURCES += clock/clockdrv.c ../../../shared/clockdrv_shell.h
# console
libbsp_a_SOURCES += console/conio.c console/console.c \
    console/defaultfont.h
# timer
libbsp_a_SOURCES += timer/timer.c

include_HEADERS += irq/irq.h
# irq
libbsp_a_SOURCES += irq/irq.c irq/bsp_irq_init.c irq/irq_init.c \
    irq/bsp_irq_asm.S irq/irq_asm.S

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
