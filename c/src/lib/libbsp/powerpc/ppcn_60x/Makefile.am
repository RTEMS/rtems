##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . @exceptions@ wrapup

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

EXTRA_PROGRAMS =
CLEANFILES =
noinst_DATA =

nodist_include_HEADERS += $(top_srcdir)/../../shared/include/coverhd.h
include_HEADERS += include/tod.h
include_HEADERS += include/nvram.h include/pci.h

EXTRA_DIST = start/start.S
start$(LIB_VARIANT).$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<
project_lib_DATA = start$(LIB_VARIANT).$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

EXTRA_PROGRAMS += clock.rel
CLEANFILES += clock.rel
clock_rel_SOURCES = clock/clock.c
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
console_rel_SOURCES = console/console.c console/i8042vga.c \
    console/i8042vga.h console/i8042.c console/i8042_p.h \
    console/ns16550cfg.c console/ns16550cfg.h console/z85c30cfg.c \
    console/z85c30cfg.h console/vga.c console/vga_p.h
console_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += console_g.rel
CLEANFILES += console_g.rel
console_g_rel_SOURCES = $(console_rel_SOURCES)
console_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
console_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += console$(LIB_VARIANT).rel

EXTRA_PROGRAMS += pci.rel
CLEANFILES += pci.rel
pci_rel_SOURCES = pci/pci.c
pci_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
pci_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += pci_g.rel
CLEANFILES += pci_g.rel
pci_g_rel_SOURCES = $(pci_rel_SOURCES)
pci_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
pci_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += pci$(LIB_VARIANT).rel

EXTRA_PROGRAMS += nvram.rel
CLEANFILES += nvram.rel
nvram_rel_SOURCES = nvram/nvram.c nvram/ds1385.h nvram/mk48t18.h \
    nvram/prepnvr.h nvram/stk11c68.h
nvram_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
nvram_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += nvram_g.rel
CLEANFILES += nvram_g.rel
nvram_g_rel_SOURCES = $(nvram_rel_SOURCES)
nvram_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
nvram_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += nvram$(LIB_VARIANT).rel

EXTRA_PROGRAMS += startup.rel
CLEANFILES += startup.rel
startup_rel_SOURCES = startup/bspstart.c startup/bspclean.c \
    ../../shared/sbrk.c startup/setvec.c startup/spurious.c \
    startup/genpvec.c startup/swap.c ../../shared/main.c \
    ../../shared/bootcard.c ../../shared/bsplibc.c ../../shared/bsppost.c \
    ../../shared/gnatinstallhandler.c startup/bsptrap.S
startup_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += startup_g.rel
CLEANFILES += startup_g.rel
startup_g_rel_SOURCES = $(startup_rel_SOURCES)
startup_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
startup_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += startup$(LIB_VARIANT).rel

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

EXTRA_PROGRAMS += tod.rel
CLEANFILES += tod.rel
tod_rel_SOURCES = tod/tod.c tod/cmos.h
tod_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
tod_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += tod_g.rel
CLEANFILES += tod_g.rel
tod_g_rel_SOURCES = $(tod_rel_SOURCES)
tod_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
tod_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += tod$(LIB_VARIANT).rel

EXTRA_PROGRAMS += universe.rel
CLEANFILES += universe.rel
universe_rel_SOURCES = universe/universe.c
universe_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
universe_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += universe_g.rel
CLEANFILES += universe_g.rel
universe_g_rel_SOURCES = $(universe_rel_SOURCES)
universe_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
universe_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += universe$(LIB_VARIANT).rel

EXTRA_DIST += vectors/README vectors/align_h.S

EXTRA_PROGRAMS += vectors.rel
CLEANFILES += vectors.rel
vectors_rel_SOURCES = vectors/vectors.S
vectors_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
vectors_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += vectors_g.rel
CLEANFILES += vectors_g.rel
vectors_g_rel_SOURCES = $(vectors_rel_SOURCES)
vectors_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
vectors_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += vectors$(LIB_VARIANT).rel

if HAS_NETWORKING
network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
EXTRA_PROGRAMS += network.rel
CLEANFILES += network.rel
network_rel_SOURCES = network/amd79c970.c network/amd79c970.h
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

all-local: $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

EXTRA_DIST += STATUS

PREINSTALL_DIRS =
TMPINSTALL_FILES =

$(PROJECT_INCLUDE)/coverhd.h: $(top_srcdir)/../../shared/include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

$(PROJECT_INCLUDE)/tod.h: include/tod.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/tod.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/tod.h

$(PROJECT_INCLUDE)/nvram.h: include/nvram.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/nvram.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/nvram.h

$(PROJECT_INCLUDE)/pci.h: include/pci.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/pci.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/pci.h

$(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT): start$(LIB_VARIANT).$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)

$(PROJECT_LIB)/linkcmds: startup/linkcmds $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds
TMPINSTALL_FILES += $(PROJECT_LIB)/linkcmds

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)
CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
