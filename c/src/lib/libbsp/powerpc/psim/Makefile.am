##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . @exceptions@ wrapup tools

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

include_HEADERS += include/coverhd.h

EXTRA_DIST = start/start.S
start$(LIB_VARIANT).$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<
project_lib_DATA = start$(LIB_VARIANT).$(OBJEXT)

EXTRA_DIST += ../../powerpc/shared/start/rtems_crti.S
rtems_crti$(LIB_VARIANT).$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<
project_lib_DATA += rtems_crti$(LIB_VARIANT).$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

EXTRA_PROGRAMS += startup.rel
CLEANFILES += startup.rel
startup_rel_SOURCES = startup/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c startup/bspstart.c ../../shared/bootcard.c \
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

EXTRA_PROGRAMS += pclock.rel
CLEANFILES += pclock.rel
pclock_rel_SOURCES = ../shared/clock/p_clock.c
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
console_rel_SOURCES = console/console-io.c console/consupp.S \
    ../../shared/console-polled.c
console_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += console_g.rel
CLEANFILES += console_g.rel
console_g_rel_SOURCES = $(console_rel_SOURCES)
console_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
console_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += console$(LIB_VARIANT).rel

include_bsp_HEADERS = irq/irq.h

EXTRA_PROGRAMS += irq.rel
CLEANFILES += irq.rel
irq_rel_SOURCES = irq/irq.c irq/irq_init.c irq/irq_asm.S
irq_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
irq_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += irq_g.rel
CLEANFILES += irq_g.rel
irq_g_rel_SOURCES = $(irq_rel_SOURCES)
irq_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
irq_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += irq$(LIB_VARIANT).rel

EXTRA_DIST += vectors/README

EXTRA_PROGRAMS += vectors.rel
CLEANFILES += vectors.rel
vectors_rel_SOURCES = vectors/align_h.S vectors/vectors.S \
    ../../powerpc/shared/vectors/vectors_init.c
vectors_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
vectors_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += vectors_g.rel
CLEANFILES += vectors_g.rel
vectors_g_rel_SOURCES = $(vectors_rel_SOURCES)
vectors_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
vectors_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += vectors$(LIB_VARIANT).rel

include_bsp_HEADERS += ../../powerpc/shared/vectors/vectors.h

if HAS_MP
EXTRA_DIST += shmsupp/README

EXTRA_PROGRAMS += shmsupp.rel
CLEANFILES += shmsupp.rel
shmsupp_rel_SOURCES = shmsupp/addrconv.c shmsupp/getcfg.c shmsupp/lock.c \
    shmsupp/mpisr.c
shmsupp_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
shmsupp_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += shmsupp_g.rel
CLEANFILES += shmsupp_g.rel
shmsupp_g_rel_SOURCES = $(shmsupp_rel_SOURCES)
shmsupp_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
shmsupp_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += shmsupp$(LIB_VARIANT).rel
endif

all-local: $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

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

$(PROJECT_INCLUDE)/coverhd.h: include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
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
PREINSTALL_FILES += $(PROJECT_LIB)/linkcmds

$(PROJECT_INCLUDE)/bsp/irq.h: irq/irq.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/irq.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/irq.h

$(PROJECT_INCLUDE)/bsp/vectors.h: ../../powerpc/shared/vectors/vectors.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/vectors.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/vectors.h

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)
CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
