##
## Makefile.am,v 1.8.4.1 2003/02/20 21:55:34 joel Exp
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# wrapup is the one that actually builds and installs the library
#  from the individual .rel files built in other directories
SUBDIRS = . @exceptions@ wrapup

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h
nodist_include_HEADERS = include/bspopts.h

EXTRA_PROGRAMS =
CLEANFILES =
noinst_DATA =

include_bspdir = $(includedir)/bsp

###
dist_project_lib_DATA += startup/linkcmds

# UNUSED
EXTRA_DIST = startup/bootpstuff.c

EXTRA_PROGRAMS += startup.rel
CLEANFILES += startup.rel
startup_rel_SOURCES = startup/bspstart.c \
    ../../powerpc/shared/startup/pgtbl_setup.c startup/pgtbl_activate.c \
    ../../powerpc/shared/startup/sbrk.c ../../shared/bootcard.c \
    startup/bspclean.c ../../shared/bsplibc.c ../../shared/bsppost.c \
    ../../shared/main.c ../../shared/gnatinstallhandler.c startup/reboot.c
startup_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += startup_g.rel
CLEANFILES += startup_g.rel
startup_g_rel_SOURCES = $(startup_rel_SOURCES)
startup_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
startup_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += startup$(LIB_VARIANT).rel

###
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

###
include_bsp_HEADERS = ../../powerpc/shared/console/uart.h

EXTRA_PROGRAMS += console.rel
CLEANFILES += console.rel
console_rel_SOURCES = ../../powerpc/shared/console/uart.c \
    ../../powerpc/shared/console/console.c
console_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += console_g.rel
CLEANFILES += console_g.rel
console_g_rel_SOURCES = $(console_rel_SOURCES)
console_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
console_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += console$(LIB_VARIANT).rel

###
include_bsp_HEADERS += pci/gtpcireg.h pci/pci.h

EXTRA_PROGRAMS += pci.rel
CLEANFILES += pci.rel
pci_rel_SOURCES = pci/pci.c pci/pci_interface.c pci/detect_host_bridge.c \
    pci/pcifinddevice.c
pci_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
pci_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += pci_g.rel
CLEANFILES += pci_g.rel
pci_g_rel_SOURCES = $(pci_rel_SOURCES)
pci_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
pci_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += pci$(LIB_VARIANT).rel

###
include_bsp_HEADERS += irq/irq.h

EXTRA_PROGRAMS += irq.rel
CLEANFILES += irq.rel
irq_rel_SOURCES = irq/irq_init.c irq/GT64260Int.c irq/irq.c ../../powerpc/shared/irq/irq_asm.S
irq_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
irq_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += irq_g.rel
CLEANFILES += irq_g.rel
irq_g_rel_SOURCES = $(irq_rel_SOURCES)
irq_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
irq_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += irq$(LIB_VARIANT).rel

###
nodist_include_HEADERS += ../../shared/tod.h

EXTRA_PROGRAMS += tod.rel
CLEANFILES += tod.rel
tod_rel_SOURCES = ../../shared/tod.c tod/todcfg.c
tod_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
tod_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += tod_g.rel
CLEANFILES += tod_g.rel
tod_g_rel_SOURCES = $(tod_rel_SOURCES)
tod_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
tod_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += tod$(LIB_VARIANT).rel

##
include_bsp_HEADERS += ../../powerpc/shared/vectors/vectors.h \
    vectors/bspException.h

EXTRA_PROGRAMS += vectors.rel
CLEANFILES += vectors.rel
vectors_rel_SOURCES = ../../powerpc/shared/vectors/vectors_init.c \
    vectors/exceptionhandler.c ../../powerpc/shared/vectors/vectors.S
vectors_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
vectors_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += vectors_g.rel
CLEANFILES += vectors_g.rel
vectors_g_rel_SOURCES = $(vectors_rel_SOURCES)
vectors_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
vectors_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += vectors$(LIB_VARIANT).rel

##
include_bsp_HEADERS += GT64260/bspMvme5500.h GT64260/gtreg.h \
    GT64260/GT64260TWSI.h GT64260/VPD.h
EXTRA_PROGRAMS += GT64260.rel
CLEANFILES += GT64260.rel
GT64260_rel_SOURCES = GT64260/GT64260TWSI.c GT64260/MVME5500I2C.c
GT64260_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
GT64260_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += GT64260_g.rel
CLEANFILES += GT64260_g.rel
GT64260_g_rel_SOURCES = $(GT64260_rel_SOURCES)
GT64260_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
GT64260_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += GT64260$(LIB_VARIANT).rel

##
include_bsp_HEADERS += network/GT64260eth.h network/GT64260ethreg.h

EXTRA_PROGRAMS += network.rel
CLEANFILES += network.rel
network_rel_SOURCES = network/GT64260eth.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += network_g.rel
CLEANFILES += network_g.rel
network_g_rel_SOURCES = $(network_rel_SOURCES)
network_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
network_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += network$(LIB_VARIANT).rel

##
include_bsp_HEADERS += vme/VME.h vme/VMEConfig.h
include_bsp_HEADERS += ../../shared/vmeUniverse/vmeUniverse.h

EXTRA_PROGRAMS += vme.rel
CLEANFILES += vme.rel
vme_rel_SOURCES = ../../shared/vmeUniverse/vmeUniverse.c vme/vmeconfig.c
vme_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
vme_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += vme_g.rel
CLEANFILES += vme_g.rel
vme_g_rel_SOURCES = $(vme_rel_SOURCES)
vme_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
vme_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += vme$(LIB_VARIANT).rel

##

EXTRA_DIST += ../../powerpc/shared/start/rtems_crti.S
rtems_crti$(LIB_VARIANT).$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<
project_lib_DATA = rtems_crti$(LIB_VARIANT).$(OBJEXT)

EXTRA_PROGRAMS += mvme5500start.$(OBJEXT)
CLEANFILES += mvme5500start.$(OBJEXT)
mvme5500start___OBJEXT__SOURCES = start/preload.S start/start.S
mvme5500start___OBJEXT__CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
mvme5500start___OBJEXT__LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += mvme5500start_g.$(OBJEXT)
CLEANFILES += mvme5500start_g.$(OBJEXT)
mvme5500start_g___OBJEXT__SOURCES = $(mvme5500start___OBJEXT__SOURCES)
mvme5500start_g___OBJEXT__CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
mvme5500start_g___OBJEXT__LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_DATA += mvme5500start$(LIB_VARIANT).rel

###

PREINSTALL_DIRS =
PREINSTALL_FILES =
TMPINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/$(dirstamp)

$(PROJECT_LIB)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_LIB)/$(dirstamp)

$(PROJECT_LIB)/bsp_specs: bsp_specs $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/bsp_specs
PREINSTALL_FILES += $(PROJECT_LIB)/bsp_specs

$(PROJECT_INCLUDE)/bsp.h: include/bsp.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp.h

$(PROJECT_INCLUDE)/tm27.h: include/tm27.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/tm27.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/tm27.h

$(PROJECT_INCLUDE)/bspopts.h: include/bspopts.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bspopts.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bspopts.h

$(PROJECT_INCLUDE)/bsp/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/bsp
	@: > $(PROJECT_INCLUDE)/bsp/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/bsp/$(dirstamp)

$(PROJECT_LIB)/linkcmds: startup/linkcmds $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds
PREINSTALL_FILES += $(PROJECT_LIB)/linkcmds

$(PROJECT_INCLUDE)/bsp/uart.h: ../../powerpc/shared/console/uart.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/uart.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/uart.h

$(PROJECT_INCLUDE)/bsp/gtpcireg.h: pci/gtpcireg.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/gtpcireg.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/gtpcireg.h

$(PROJECT_INCLUDE)/bsp/pci.h: pci/pci.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/pci.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/pci.h

$(PROJECT_INCLUDE)/bsp/irq.h: irq/irq.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/irq.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/irq.h

$(PROJECT_INCLUDE)/tod.h: ../../shared/tod.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/tod.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/tod.h

$(PROJECT_INCLUDE)/bsp/vectors.h: ../../powerpc/shared/vectors/vectors.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/vectors.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/vectors.h

$(PROJECT_INCLUDE)/bsp/bspException.h: vectors/bspException.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/bspException.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/bspException.h

$(PROJECT_INCLUDE)/bsp/bspMvme5500.h: GT64260/bspMvme5500.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/bspMvme5500.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/bspMvme5500.h

$(PROJECT_INCLUDE)/bsp/gtreg.h: GT64260/gtreg.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/gtreg.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/gtreg.h

$(PROJECT_INCLUDE)/bsp/GT64260TWSI.h: GT64260/GT64260TWSI.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/GT64260TWSI.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/GT64260TWSI.h

$(PROJECT_INCLUDE)/bsp/VPD.h: GT64260/VPD.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/VPD.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/VPD.h

$(PROJECT_INCLUDE)/bsp/GT64260eth.h: network/GT64260eth.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/GT64260eth.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/GT64260eth.h

$(PROJECT_INCLUDE)/bsp/GT64260ethreg.h: network/GT64260ethreg.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/GT64260ethreg.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/GT64260ethreg.h

$(PROJECT_INCLUDE)/bsp/VME.h: vme/VME.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/VME.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/VME.h

$(PROJECT_INCLUDE)/bsp/VMEConfig.h: vme/VMEConfig.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/VMEConfig.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/VMEConfig.h

$(PROJECT_INCLUDE)/bsp/vmeUniverse.h: ../../shared/vmeUniverse/vmeUniverse.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/vmeUniverse.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/vmeUniverse.h

$(PROJECT_LIB)/rtems_crti$(LIB_VARIANT).$(OBJEXT): rtems_crti$(LIB_VARIANT).$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/rtems_crti$(LIB_VARIANT).$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/rtems_crti$(LIB_VARIANT).$(OBJEXT)

$(PROJECT_LIB)/mvme5500start$(LIB_VARIANT).rel: mvme5500start$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/mvme5500start$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/mvme5500start$(LIB_VARIANT).rel

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)
CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
