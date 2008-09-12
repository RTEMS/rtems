##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

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
DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =

EXTRA_DIST = start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

# some objects have to be forced together to ensure they are ALWAYS
# the ones linked into the application executable.  Every application
# needs startup/bspstart.c, so we will force in the others
noinst_PROGRAMS += gbaoverrides.rel
gbaoverrides_rel_SOURCES = startup/bspstart.c startup/cpu.c startup/cpu_asm.S
gbaoverrides_rel_CPPFLAGS = $(AM_CPPFLAGS)
gbaoverrides_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

startup_SOURCES = ../../shared/bsplibc.c ../../shared/bsppost.c \
    startup/bspgetworkarea.c ../../shared/bsppretaskinghook.c \
    ../../shared/bsppredriverhook.c startup/bspclean.c \
    ../../shared/bootcard.c ../../shared/sbrk.c \
    ../../shared/gnatinstallhandler.c
clock_SOURCES = clock/clockdrv.c
console_SOURCES = console/conio.c console/console.c \
    console/defaultfont.c
timer_SOURCES = timer/timer.c

include_HEADERS += irq/irq.h
irq_SOURCES = irq/irq.c irq/bsp_irq_init.c irq/irq_init.c \
    irq/bsp_irq_asm.S irq/irq_asm.S

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(startup_SOURCES) $(clock_SOURCES) $(console_SOURCES) \
    $(timer_SOURCES) $(irq_SOURCES)

libbsp_a_LIBADD = gbaoverrides.rel
if HAS_NETWORKING
##
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
