##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

EXTRA_PROGRAMS =
CLEANFILES =
noinst_DATA =

include_HEADERS += include/ep7312.h
nodist_include_HEADERS += ../../shared/include/coverhd.h

EXTRA_DIST = start/start.S
start$(LIB_VARIANT).$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<
project_lib_DATA = start$(LIB_VARIANT).$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

EXTRA_PROGRAMS += startup.rel
CLEANFILES += startup.rel
startup_rel_SOURCES = ../../shared/bsplibc.c ../../shared/bsppost.c \
    startup/bspstart.c startup/exit.c ../../shared/bootcard.c \
    ../../shared/main.c ../../shared/sbrk.c \
    ../../shared/gnatinstallhandler.c
startup_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += startup_g.rel
CLEANFILES += startup_g.rel
startup_g_rel_SOURCES = $(startup_rel_SOURCES)
startup_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
startup_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += startup$(LIB_VARIANT).rel

EXTRA_PROGRAMS += clock.rel
CLEANFILES += clock.rel
clock_rel_SOURCES = clock/clockdrv.c
clock_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += clock_g.rel
CLEANFILES += clock_g.rel
clock_g_rel_SOURCES = $(clock_rel_SOURCES)
clock_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
clock_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += clock$(LIB_VARIANT).rel

EXTRA_PROGRAMS += console.rel
CLEANFILES += console.rel
console_rel_SOURCES = console/uart.c ../../shared/console.c
console_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += console_g.rel
CLEANFILES += console_g.rel
console_g_rel_SOURCES = $(console_rel_SOURCES)
console_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
console_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += console$(LIB_VARIANT).rel

EXTRA_PROGRAMS += timer.rel
CLEANFILES += timer.rel
timer_rel_SOURCES = timer/timer.c
timer_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += timer_g.rel
CLEANFILES += timer_g.rel
timer_g_rel_SOURCES = $(timer_rel_SOURCES)
timer_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
timer_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += timer$(LIB_VARIANT).rel

include_HEADERS += irq/irq.h

EXTRA_PROGRAMS += irq.rel
CLEANFILES += irq.rel
irq_rel_SOURCES = irq/irq.c irq/bsp_irq_init.c \
    ../../arm/shared/irq/irq_init.c irq/bsp_irq_asm.S \
    ../../arm/shared/irq/irq_asm.S irq/irq.h
irq_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
irq_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += irq_g.rel
CLEANFILES += irq_g.rel
irq_g_rel_SOURCES = $(irq_rel_SOURCES)
irq_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
irq_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += irq$(LIB_VARIANT).rel

if HAS_NETWORKING
network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
EXTRA_PROGRAMS += network.rel
CLEANFILES += network.rel
network_rel_SOURCES = network/network.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V) \
    $(network_CPPFLAGS)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += network_g.rel
CLEANFILES += network_g.rel
network_g_rel_SOURCES = $(network_rel_SOURCES)
network_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V) \
    $(network_CPPFLAGS)
network_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += network$(LIB_VARIANT).rel
endif

EXTRA_LIBRARIES = libbsp.a
CLEANFILES += libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD = clock$(LIB_VARIANT).rel console$(LIB_VARIANT).rel \
    startup$(LIB_VARIANT).rel irq$(LIB_VARIANT).rel timer$(LIB_VARIANT).rel
if HAS_NETWORKING
libbsp_a_LIBADD += network$(LIB_VARIANT).rel
endif
EXTRA_LIBRARIES += libbsp_g.a
CLEANFILES += libbsp_g.a
libbsp_g_a_SOURCES = $(libbsp_a_SOURCES)
libbsp_g_a_LIBADD = $(libbsp_a_LIBADD)

noinst_DATA += libbsp$(LIB_VARIANT).a

all-local: $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

PREINSTALL_DIRS =
TMPINSTALL_FILES =

$(PROJECT_INCLUDE)/ep7312.h: include/ep7312.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/ep7312.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/ep7312.h

$(PROJECT_INCLUDE)/coverhd.h: ../../shared/include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

$(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT): start$(LIB_VARIANT).$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)

$(PROJECT_LIB)/linkcmds: startup/linkcmds $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds
TMPINSTALL_FILES += $(PROJECT_LIB)/linkcmds

$(PROJECT_INCLUDE)/irq.h: irq/irq.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/irq.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/irq.h

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)
CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
