##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

include_bspdir = $(includedir)/bsp

include_HEADERS += include/coverhd.h
include_bsp_HEADERS = include/mbx.h include/commproc.h include/8xx_immap.h \
    irq/irq.h vectors/vectors.h

EXTRA_DIST = times-mbx821 times-mbx860

dist_project_lib_DATA += startup/linkcmds

noinst_PROGRAMS += pclock.rel
pclock_rel_SOURCES = clock/p_clock.c
pclock_rel_CPPFLAGS = $(AM_CPPFLAGS)
pclock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += console.rel
console_rel_SOURCES = console/console.c
console_rel_CPPFLAGS = $(AM_CPPFLAGS)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += irq.rel
irq_rel_SOURCES = irq/irq.c irq/irq_asm.S irq/irq_init.c
irq_rel_CPPFLAGS = $(AM_CPPFLAGS)
irq_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += ide.rel
ide_rel_SOURCES = ide/idecfg.c ide/pcmcia_ide.c
ide_rel_CPPFLAGS = $(AM_CPPFLAGS)
ide_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += vectors.rel
vectors_rel_SOURCES = vectors/vectors.h vectors/vectors_init.c \
    vectors/vectors.S
vectors_rel_CPPFLAGS = $(AM_CPPFLAGS)
vectors_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += startup.rel
startup_rel_SOURCES = ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c startup/bspstart.c ../../shared/bootcard.c \
    startup/imbx8xx.c ../../shared/main.c startup/mmutlbtab.c \
    ../../shared/sbrk.c ../../shared/gnatinstallhandler.c startup/start.S
startup_rel_CPPFLAGS = $(AM_CPPFLAGS)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

if HAS_NETWORKING
network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += network.rel
network_rel_SOURCES = network/network.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS) $(network_CPPFLAGS)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

libbsp_a_LIBADD = pclock.rel irq.rel ide.rel startup.rel console.rel \
    vectors.rel
if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
endif

libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc8xx/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc8xx/console-generic.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc8xx/cpm.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc8xx/mmu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc8xx/timer.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc8xx/exceptions.rel

all-local: $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

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

$(PROJECT_INCLUDE)/bsp/irq.h: irq/irq.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/irq.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/irq.h

$(PROJECT_INCLUDE)/bsp/vectors.h: vectors/vectors.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/vectors.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/vectors.h

$(PROJECT_LIB)/linkcmds: startup/linkcmds $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds
PREINSTALL_FILES += $(PROJECT_LIB)/linkcmds

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES += $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
