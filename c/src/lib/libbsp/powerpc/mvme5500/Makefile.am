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

# UNUSED
EXTRA_DIST = startup/bootpstuff.c

noinst_PROGRAMS += startup.rel
startup_rel_SOURCES = startup/bspstart.c \
    ../../powerpc/shared/startup/pgtbl_setup.c startup/pgtbl_activate.c \
    ../../powerpc/shared/startup/sbrk.c ../../shared/bootcard.c \
    startup/bspclean.c ../../shared/bsplibc.c ../../shared/bsppost.c \
    ../../shared/main.c ../../shared/gnatinstallhandler.c startup/reboot.c
startup_rel_CPPFLAGS = $(AM_CPPFLAGS)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

###
noinst_PROGRAMS += pclock.rel
pclock_rel_SOURCES = ../../powerpc/shared/clock/p_clock.c
pclock_rel_CPPFLAGS = $(AM_CPPFLAGS)
pclock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

###
include_bsp_HEADERS = ../../powerpc/shared/console/uart.h

noinst_PROGRAMS += console.rel
console_rel_SOURCES = ../../powerpc/shared/console/uart.c \
    ../../powerpc/shared/console/console.c
console_rel_CPPFLAGS = $(AM_CPPFLAGS)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

###
include_bsp_HEADERS += pci/gtpcireg.h pci/pci.h

noinst_PROGRAMS += pci.rel
pci_rel_SOURCES = pci/pci.c pci/pci_interface.c pci/detect_host_bridge.c \
    pci/pcifinddevice.c
pci_rel_CPPFLAGS = $(AM_CPPFLAGS)
pci_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

###
include_bsp_HEADERS += irq/irq.h

noinst_PROGRAMS += irq.rel
irq_rel_SOURCES = irq/irq_init.c irq/GT64260Int.c irq/irq.c \
    ../../powerpc/shared/irq/irq_asm.S
irq_rel_CPPFLAGS = $(AM_CPPFLAGS)
irq_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

###
nodist_include_HEADERS += ../../shared/tod.h

noinst_PROGRAMS += tod.rel
tod_rel_SOURCES = ../../shared/tod.c tod/todcfg.c
tod_rel_CPPFLAGS = $(AM_CPPFLAGS)
tod_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

##
include_bsp_HEADERS += ../../powerpc/shared/vectors/vectors.h \
    vectors/bspException.h

noinst_PROGRAMS += vectors.rel
vectors_rel_SOURCES = ../../powerpc/shared/vectors/vectors_init.c \
    vectors/exceptionhandler.c ../../powerpc/shared/vectors/vectors.S
vectors_rel_CPPFLAGS = $(AM_CPPFLAGS)
vectors_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

##
include_bsp_HEADERS += GT64260/bspMvme5500.h GT64260/gtreg.h \
    GT64260/GT64260TWSI.h GT64260/VPD.h
noinst_PROGRAMS += GT64260.rel
GT64260_rel_SOURCES = GT64260/GT64260TWSI.c GT64260/MVME5500I2C.c
GT64260_rel_CPPFLAGS = $(AM_CPPFLAGS)
GT64260_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

##
if HAS_NETWORKING
include_bsp_HEADERS += network/GT64260eth.h network/GT64260ethreg.h

network_CPPFLAGS = -D_KERNEL
noinst_PROGRAMS += network.rel
network_rel_SOURCES = network/GT64260eth.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS) $(network_CPPFLAGS)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif
##
include_bsp_HEADERS += vme/VME.h vme/VMEConfig.h
include_bsp_HEADERS += ../../shared/vmeUniverse/vmeUniverse.h

noinst_PROGRAMS += vme.rel
vme_rel_SOURCES = ../../shared/vmeUniverse/vmeUniverse.c vme/vmeconfig.c
vme_rel_CPPFLAGS = $(AM_CPPFLAGS)
vme_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

##

EXTRA_DIST += ../../powerpc/shared/start/rtems_crti.S
rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<
project_lib_DATA = rtems_crti.$(OBJEXT)

noinst_PROGRAMS += mvme5500start.$(OBJEXT)
mvme5500start___OBJEXT__SOURCES = start/preload.S start/start.S
mvme5500start___OBJEXT__CPPFLAGS = $(AM_CPPFLAGS)
mvme5500start___OBJEXT__LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_DATA += mvme5500start.$(OBJEXT)

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD = pclock.rel console.rel irq.rel pci.rel startup.rel tod.rel \
    vectors.rel GT64260.rel
if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
endif
libbsp_a_LIBADD += vme.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/stack.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
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

if HAS_NETWORKING
$(PROJECT_INCLUDE)/bsp/GT64260eth.h: network/GT64260eth.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/GT64260eth.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/GT64260eth.h

$(PROJECT_INCLUDE)/bsp/GT64260ethreg.h: network/GT64260ethreg.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/GT64260ethreg.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/GT64260ethreg.h
endif

$(PROJECT_INCLUDE)/bsp/VME.h: vme/VME.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/VME.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/VME.h

$(PROJECT_INCLUDE)/bsp/VMEConfig.h: vme/VMEConfig.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/VMEConfig.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/VMEConfig.h

$(PROJECT_INCLUDE)/bsp/vmeUniverse.h: ../../shared/vmeUniverse/vmeUniverse.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/vmeUniverse.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/vmeUniverse.h

$(PROJECT_LIB)/rtems_crti.$(OBJEXT): rtems_crti.$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/rtems_crti.$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/rtems_crti.$(OBJEXT)

$(PROJECT_LIB)/mvme5500start.$(OBJEXT): mvme5500start.$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/mvme5500start.$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/mvme5500start.$(OBJEXT)

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES += $(PREINSTALL_DIRS)
CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
