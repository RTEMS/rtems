##
## Makefile.am,v 1.8.4.1 2003/02/20 21:55:34 joel Exp
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
DISTCLEANFILES = include/bspopts.h
nodist_include_HEADERS += ../../shared/include/coverhd.h

noinst_PROGRAMS =

include_bspdir = $(includedir)/bsp

###
dist_project_lib_DATA += startup/linkcmds

noinst_PROGRAMS += startup.rel
startup_rel_SOURCES = startup/bspstart.c ../../shared/bootcard.c \
    ../../shared/main.c ../../shared/bsppost.c ../../shared/bsplibc.c \
    ../../powerpc/shared/startup/sbrk.c ../../shared/bspclean.c \
    ../../shared/gnatinstallhandler.c \
    ../../powerpc/shared/startup/pgtbl_setup.c \
    ../../powerpc/shared/startup/pgtbl_activate.c
startup_rel_CPPFLAGS = $(AM_CPPFLAGS)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

###
noinst_PROGRAMS += pclock.rel
pclock_rel_SOURCES = ../../powerpc/shared/clock/p_clock.c
pclock_rel_CPPFLAGS = $(AM_CPPFLAGS)
pclock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

###
include_bsp_HEADERS = ../../powerpc/shared/console/uart.h \
    ../../powerpc/shared/console/consoleIo.h \
    ../../powerpc/shared/motorola/motorola.h \
    ../../powerpc/shared/residual/residual.h \
    ../../powerpc/shared/residual/pnp.h \
    ../../powerpc/shared/console/consoleIo.h console/rsPMCQ1.h

noinst_PROGRAMS += console.rel
console_rel_SOURCES = console/console.c console/ns16550cfg.c \
    console/mc68360_scc.c console/rsPMCQ1.c console/alloc360.c \
    console/init68360.c
console_rel_CPPFLAGS = $(AM_CPPFLAGS)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
###
include_bsp_HEADERS += ../../powerpc/shared/openpic/openpic.h

noinst_PROGRAMS += openpic.rel
openpic_rel_SOURCES = ../../powerpc/shared/openpic/openpic.h \
    ../../powerpc/shared/openpic/openpic.c

openpic_rel_CPPFLAGS = $(AM_CPPFLAGS)
openpic_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

###
include_bsp_HEADERS += ../../powerpc/shared/pci/pci.h

noinst_PROGRAMS += pci.rel
pci_rel_SOURCES = pci/no_host_bridge.c ../../powerpc/shared/pci/pci.c \
    ../../powerpc/shared/pci/pcifinddevice.c
pci_rel_CPPFLAGS = $(AM_CPPFLAGS)
pci_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

###
include_bsp_HEADERS += ../../powerpc/shared/irq/irq.h

noinst_PROGRAMS += irq.rel
irq_rel_SOURCES = irq/irq_init.c irq/irq.c ../../powerpc/shared/irq/i8259.c \
    ../../powerpc/shared/irq/irq_asm.S
irq_rel_CPPFLAGS = $(AM_CPPFLAGS)
irq_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

##
include_bsp_HEADERS += ../../powerpc/shared/vectors/vectors.h

noinst_PROGRAMS += vectors.rel
vectors_rel_SOURCES = ../../powerpc/shared/vectors/vectors_init.c \
    ../../powerpc/shared/vectors/vectors.S
vectors_rel_CPPFLAGS = $(AM_CPPFLAGS)
vectors_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

##
include_bsp_HEADERS += ../../shared/vmeUniverse/vmeUniverse.h \
    ../../powerpc/shared/vme/VMEConfig.h ../../powerpc/shared/vme/VME.h

noinst_PROGRAMS += vme.rel
vme_rel_SOURCES = ../../shared/vmeUniverse/vmeUniverse.c vme/vmeconfig.c
vme_rel_CPPFLAGS = $(AM_CPPFLAGS)
vme_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

##

EXTRA_DIST = start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

EXTRA_DIST += ../../powerpc/shared/start/rtems_crti.S
rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD = pclock.rel console.rel irq.rel openpic.rel pci.rel \
    vectors.rel startup.rel vme.rel

libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/stack.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/exceptions.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/mmu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/timer.rel

all-local: $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

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

$(PROJECT_INCLUDE)/coverhd.h: ../../shared/include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

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

$(PROJECT_INCLUDE)/bsp/consoleIo.h: ../../powerpc/shared/console/consoleIo.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/consoleIo.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/consoleIo.h

$(PROJECT_INCLUDE)/bsp/motorola.h: ../../powerpc/shared/motorola/motorola.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/motorola.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/motorola.h

$(PROJECT_INCLUDE)/bsp/residual.h: ../../powerpc/shared/residual/residual.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/residual.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/residual.h

$(PROJECT_INCLUDE)/bsp/pnp.h: ../../powerpc/shared/residual/pnp.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/pnp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/pnp.h

$(PROJECT_INCLUDE)/bsp/consoleIo.h: ../../powerpc/shared/console/consoleIo.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/consoleIo.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/consoleIo.h

$(PROJECT_INCLUDE)/bsp/rsPMCQ1.h: console/rsPMCQ1.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/rsPMCQ1.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/rsPMCQ1.h

$(PROJECT_INCLUDE)/bsp/openpic.h: ../../powerpc/shared/openpic/openpic.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/openpic.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/openpic.h

$(PROJECT_INCLUDE)/bsp/pci.h: ../../powerpc/shared/pci/pci.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/pci.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/pci.h

$(PROJECT_INCLUDE)/bsp/irq.h: ../../powerpc/shared/irq/irq.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/irq.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/irq.h

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

$(PROJECT_LIB)/start.$(OBJEXT): start.$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/start.$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/start.$(OBJEXT)

$(PROJECT_LIB)/rtems_crti.$(OBJEXT): rtems_crti.$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/rtems_crti.$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/rtems_crti.$(OBJEXT)

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES += $(PREINSTALL_DIRS)
CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
