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

include_bspdir = $(includedir)/bsp

include_HEADERS += include/coverhd.h
include_bsp_HEADERS = include/mbx.h include/commproc.h include/8xx_immap.h

EXTRA_DIST = times-mbx821 times-mbx860

dist_project_lib_DATA += startup/linkcmds

EXTRA_PROGRAMS += pclock.rel
CLEANFILES += pclock.rel
pclock_rel_SOURCES = clock/p_clock.c
pclock_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
pclock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += pclock_g.rel
CLEANFILES += pclock_g.rel
pclock_g_rel_SOURCES = $(pclock_rel_SOURCES)
pclock_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
pclock_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += pclock$(LIB_VARIANT).rel

EXTRA_PROGRAMS += console.rel
CLEANFILES += console.rel
console_rel_SOURCES = console/console.c
console_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += console_g.rel
CLEANFILES += console_g.rel
console_g_rel_SOURCES = $(console_rel_SOURCES)
console_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
console_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += console$(LIB_VARIANT).rel

EXTRA_PROGRAMS += irq.rel
CLEANFILES += irq.rel
irq_rel_SOURCES = irq/irq.c irq/irq_asm.S irq/irq_init.c
irq_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
irq_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += irq_g.rel
CLEANFILES += irq_g.rel
irq_g_rel_SOURCES = $(irq_rel_SOURCES)
irq_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
irq_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += irq$(LIB_VARIANT).rel

EXTRA_PROGRAMS += ide.rel
CLEANFILES += ide.rel
ide_rel_SOURCES = ide/idecfg.c ide/pcmcia_ide.c
ide_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
ide_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += ide_g.rel
CLEANFILES += ide_g.rel
ide_g_rel_SOURCES = $(ide_rel_SOURCES)
ide_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
ide_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += ide$(LIB_VARIANT).rel

EXTRA_PROGRAMS += vectors.rel
CLEANFILES += vectors.rel
vectors_rel_SOURCES = vectors/vectors.h vectors/vectors_init.c \
    vectors/vectors.S
vectors_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
vectors_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += vectors_g.rel
CLEANFILES += vectors_g.rel
vectors_g_rel_SOURCES = $(vectors_rel_SOURCES)
vectors_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
vectors_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += vectors$(LIB_VARIANT).rel

EXTRA_PROGRAMS += startup.rel
CLEANFILES += startup.rel
startup_rel_SOURCES = ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c startup/bspstart.c ../../shared/bootcard.c \
    startup/imbx8xx.c ../../shared/main.c startup/mmutlbtab.c \
    ../../shared/sbrk.c ../../shared/gnatinstallhandler.c startup/start.S
startup_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += startup_g.rel
CLEANFILES += startup_g.rel
startup_g_rel_SOURCES = $(startup_rel_SOURCES)
startup_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
startup_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += startup$(LIB_VARIANT).rel

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

all-local: $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

PREINSTALL_DIRS =
TMPINSTALL_FILES =

$(PROJECT_INCLUDE)/bsp/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/bsp
	@: > $(PROJECT_INCLUDE)/bsp/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/bsp/$(dirstamp)

$(PROJECT_INCLUDE)/coverhd.h: include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

$(PROJECT_INCLUDE)/bsp/mbx.h: include/mbx.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/mbx.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/mbx.h

$(PROJECT_INCLUDE)/bsp/commproc.h: include/commproc.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/commproc.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/commproc.h

$(PROJECT_INCLUDE)/bsp/8xx_immap.h: include/8xx_immap.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/8xx_immap.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/8xx_immap.h

$(PROJECT_LIB)/linkcmds: startup/linkcmds $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds
TMPINSTALL_FILES += $(PROJECT_LIB)/linkcmds

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)
CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
