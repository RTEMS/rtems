##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . bootloader @exceptions@ wrapup

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

EXTRA_PROGRAMS =
CLEANFILES =
noinst_DATA =

include_bspdir = $(includedir)/bsp

include_HEADERS += ../../powerpc/shared/include/nvram.h
include_HEADERS += ../../shared/include/coverhd.h

EXTRA_DIST = ../../powerpc/shared/start/start.S
start$(LIB_VARIANT).$(OBJEXT): ../../powerpc/shared/start/start.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<
project_lib_DATA = start$(LIB_VARIANT).$(OBJEXT)

EXTRA_DIST += ../../powerpc/shared/start/rtems_crti.S
rtems_crti$(LIB_VARIANT).$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<
project_lib_DATA += rtems_crti$(LIB_VARIANT).$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

EXTRA_PROGRAMS += startup.rel
CLEANFILES += startup.rel
startup_rel_SOURCES = ../../powerpc/shared/startup/bspstart.c \
    ../../powerpc/shared/startup/pgtbl_setup.c \
    ../../powerpc/shared/startup/pgtbl_activate.c \
    ../../powerpc/shared/startup/sbrk.c ../../shared/bootcard.c \
    ../../shared/bspclean.c ../../shared/bsplibc.c ../../shared/bsppost.c \
    ../../shared/main.c ../../shared/gnatinstallhandler.c
startup_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += startup_g.rel
CLEANFILES += startup_g.rel
startup_g_rel_SOURCES = $(startup_rel_SOURCES)
startup_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
startup_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += startup$(LIB_VARIANT).rel

EXTRA_PROGRAMS += pclock.rel
CLEANFILES += pclock.rel
pclock_rel_SOURCES = ../../powerpc/shared/clock/p_clock.c
pclock_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
pclock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += pclock_g.rel
CLEANFILES += pclock_g.rel
pclock_g_rel_SOURCES = $(pclock_rel_SOURCES)
pclock_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
pclock_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += pclock$(LIB_VARIANT).rel

include_bsp_HEADERS = ../../powerpc/shared/console/consoleIo.h
include_bsp_HEADERS += ../../powerpc/shared/console/uart.h

console_CPPFLAGS = -DSTATIC_LOG_ALLOC
EXTRA_PROGRAMS += console.rel
CLEANFILES += console.rel
console_rel_SOURCES = ../../powerpc/shared/console/polled_io.c \
    ../../powerpc/shared/console/uart.c \
    ../../powerpc/shared/console/console.c \
    ../../powerpc/shared/console/inch.c \
    ../../powerpc/shared/console/reboot.c \
    ../../powerpc/shared/console/consoleIo.h \
    ../../powerpc/shared/console/keyboard.h \
    ../../powerpc/shared/console/uart.h
console_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V) \
    $(console_CPPFLAGS)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += console_g.rel
CLEANFILES += console_g.rel
console_g_rel_SOURCES = $(console_rel_SOURCES)
console_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V) \
    $(console_CPPFLAGS)
console_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += console$(LIB_VARIANT).rel

include_bsp_HEADERS += ../../powerpc/shared/irq/irq.h

EXTRA_PROGRAMS += irq.rel
CLEANFILES += irq.rel
irq_rel_SOURCES = ../../powerpc/shared/irq/irq_init.c \
    ../../powerpc/shared/irq/i8259.c ../../powerpc/shared/irq/irq.c \
    ../../powerpc/shared/irq/irq_asm.S ../../powerpc/shared/irq/irq.h
irq_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
irq_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += irq_g.rel
CLEANFILES += irq_g.rel
irq_g_rel_SOURCES = $(irq_rel_SOURCES)
irq_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
irq_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += irq$(LIB_VARIANT).rel

include_bsp_HEADERS += ../../powerpc/shared/motorola/motorola.h

EXTRA_PROGRAMS += motorola.rel
CLEANFILES += motorola.rel
motorola_rel_SOURCES = ../../powerpc/shared/motorola/motorola.h \
    ../../powerpc/shared/motorola/motorola.c
motorola_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
motorola_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += motorola_g.rel
CLEANFILES += motorola_g.rel
motorola_g_rel_SOURCES = $(motorola_rel_SOURCES)
motorola_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
motorola_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += motorola$(LIB_VARIANT).rel

include_bsp_HEADERS += ../../powerpc/shared/openpic/openpic.h

EXTRA_PROGRAMS += openpic.rel
CLEANFILES += openpic.rel
openpic_rel_SOURCES = ../../powerpc/shared/openpic/openpic.h \
    ../../powerpc/shared/openpic/openpic.c \
    ../../powerpc/shared/openpic/openpic.h

openpic_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
openpic_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += openpic_g.rel
CLEANFILES += openpic_g.rel
openpic_g_rel_SOURCES = $(openpic_rel_SOURCES)
openpic_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
openpic_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += openpic$(LIB_VARIANT).rel

include_bsp_HEADERS += ../../powerpc/shared/pci/pci.h

EXTRA_PROGRAMS += pci.rel
CLEANFILES += pci.rel
pci_rel_SOURCES = ../../powerpc/shared/pci/pci.c \
    ../../powerpc/shared/pci/detect_raven_bridge.c \
    ../../powerpc/shared/pci/pcifinddevice.c ../../powerpc/shared/pci/pci.h
pci_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
pci_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += pci_g.rel
CLEANFILES += pci_g.rel
pci_g_rel_SOURCES = $(pci_rel_SOURCES)
pci_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
pci_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += pci$(LIB_VARIANT).rel

include_bsp_HEADERS += ../../powerpc/shared/residual/residual.h \
    ../../powerpc/shared/residual/pnp.h

EXTRA_PROGRAMS += residual.rel
CLEANFILES += residual.rel
residual_rel_SOURCES = ../../powerpc/shared/residual/pnp.h \
    ../../powerpc/shared/residual/residual.h \
    ../../powerpc/shared/residual/residual.c
residual_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
residual_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += residual_g.rel
CLEANFILES += residual_g.rel
residual_g_rel_SOURCES = $(residual_rel_SOURCES)
residual_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
residual_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += residual$(LIB_VARIANT).rel

include_bsp_HEADERS += ../../powerpc/shared/vectors/vectors.h

EXTRA_PROGRAMS += vectors.rel
CLEANFILES += vectors.rel
vectors_rel_SOURCES = ../../powerpc/shared/vectors/vectors.h \
    ../../powerpc/shared/vectors/vectors_init.c \
    ../../powerpc/shared/vectors/vectors.S
vectors_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
vectors_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += vectors_g.rel
CLEANFILES += vectors_g.rel
vectors_g_rel_SOURCES = $(vectors_rel_SOURCES)
vectors_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
vectors_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += vectors$(LIB_VARIANT).rel

include_bsp_HEADERS += ../../shared/vmeUniverse/vmeUniverse.h \
    ../../powerpc/shared/vme/VMEConfig.h ../../powerpc/shared/vme/VME.h

EXTRA_PROGRAMS += vme.rel
CLEANFILES += vme.rel
vme_rel_SOURCES = ../../shared/vmeUniverse/vmeUniverse.c \
    ../../shared/vmeUniverse/vmeUniverse.h \
    ../../powerpc/shared/vme/vmeconfig.c \
    ../../powerpc/shared/vme/VMEConfig.h ../../powerpc/shared/vme/VME.h
vme_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
vme_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += vme_g.rel
CLEANFILES += vme_g.rel
vme_g_rel_SOURCES = $(vme_rel_SOURCES)
vme_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
vme_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += vme$(LIB_VARIANT).rel

all-local: $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

EXTRA_DIST += BOOTING README.MVME2300 README.OTHERBOARDS README.dec21140
EXTRA_DIST += times.mcp750 times.mvme2307

PREINSTALL_DIRS =
TMPINSTALL_FILES =

$(PROJECT_INCLUDE)/bsp/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/bsp
	@: > $(PROJECT_INCLUDE)/bsp/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/bsp/$(dirstamp)

$(PROJECT_INCLUDE)/nvram.h: ../../powerpc/shared/include/nvram.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/nvram.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/nvram.h

$(PROJECT_INCLUDE)/coverhd.h: ../../shared/include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

$(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT): start$(LIB_VARIANT).$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)

$(PROJECT_LIB)/rtems_crti$(LIB_VARIANT).$(OBJEXT): rtems_crti$(LIB_VARIANT).$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/rtems_crti$(LIB_VARIANT).$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/rtems_crti$(LIB_VARIANT).$(OBJEXT)

$(PROJECT_LIB)/linkcmds: startup/linkcmds $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds
TMPINSTALL_FILES += $(PROJECT_LIB)/linkcmds

$(PROJECT_INCLUDE)/bsp/consoleIo.h: ../../powerpc/shared/console/consoleIo.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/consoleIo.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/consoleIo.h

$(PROJECT_INCLUDE)/bsp/uart.h: ../../powerpc/shared/console/uart.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/uart.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/uart.h

$(PROJECT_INCLUDE)/bsp/irq.h: ../../powerpc/shared/irq/irq.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/irq.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/irq.h

$(PROJECT_INCLUDE)/bsp/motorola.h: ../../powerpc/shared/motorola/motorola.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/motorola.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/motorola.h

$(PROJECT_INCLUDE)/bsp/openpic.h: ../../powerpc/shared/openpic/openpic.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/openpic.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/openpic.h

$(PROJECT_INCLUDE)/bsp/pci.h: ../../powerpc/shared/pci/pci.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/pci.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/pci.h

$(PROJECT_INCLUDE)/bsp/residual.h: ../../powerpc/shared/residual/residual.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/residual.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/residual.h

$(PROJECT_INCLUDE)/bsp/pnp.h: ../../powerpc/shared/residual/pnp.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/pnp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/pnp.h

$(PROJECT_INCLUDE)/bsp/vectors.h: ../../powerpc/shared/vectors/vectors.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/vectors.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/vectors.h

$(PROJECT_INCLUDE)/bsp/vmeUniverse.h: ../../shared/vmeUniverse/vmeUniverse.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/vmeUniverse.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/vmeUniverse.h

$(PROJECT_INCLUDE)/bsp/VMEConfig.h: ../../powerpc/shared/vme/VMEConfig.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/VMEConfig.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/VMEConfig.h

$(PROJECT_INCLUDE)/bsp/VME.h: ../../powerpc/shared/vme/VME.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/VME.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/VME.h

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)
CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
