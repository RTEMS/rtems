##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

EXTRA_PROGRAMS =
CLEANFILES =
noinst_DATA =

include_HEADERS += include/coverhd.h

EXTRA_DIST = start/start.S start/start.ram start/start.rom
start$(LIB_VARIANT).$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<
project_lib_DATA = start$(LIB_VARIANT).$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds startup/linkcmds.ram \
    startup/linkcmds.rom

EXTRA_PROGRAMS += startup.rel
CLEANFILES += startup.rel
startup_rel_SOURCES = startup/hw_init.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c startup/bspstart.c startup/bspclean.c \
    ../../shared/sbrk.c ../../shared/bootcard.c ../../shared/main.c \
    ../../shared/gnatinstallhandler.c
startup_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += startup_g.rel
CLEANFILES += startup_g.rel
startup_g_rel_SOURCES = $(startup_rel_SOURCES)
startup_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
startup_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += startup$(LIB_VARIANT).rel

SHGEN = $(PROJECT_TOPdir)/tools/cpu/sh/shgen

scitab.c: $(SHGEN)
	$(SHGEN) -H @CPU_CLOCK_RATE_HZ@ sci > $@
BUILT_SOURCES = scitab.c
CLEANFILES += scitab.c

EXTRA_PROGRAMS += scitab.rel
CLEANFILES += scitab.rel
scitab_rel_SOURCES = scitab.c
scitab_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
scitab_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += scitab_g.rel
CLEANFILES += scitab_g.rel
scitab_g_rel_SOURCES = $(scitab_rel_SOURCES)
scitab_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
scitab_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += scitab$(LIB_VARIANT).rel

EXTRA_PROGRAMS += console.rel
CLEANFILES += console.rel
console_rel_SOURCES = ../../sh/shared/console.c console/config.c
console_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += console_g.rel
CLEANFILES += console_g.rel
console_g_rel_SOURCES = $(console_rel_SOURCES)
console_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
console_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += console$(LIB_VARIANT).rel

EXTRA_LIBRARIES = libbsp.a
CLEANFILES += libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD = startup$(LIB_VARIANT).rel scitab$(LIB_VARIANT).rel \
    console$(LIB_VARIANT).rel
libbsp_a_LIBADD += \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/clock$(LIB_VARIANT).rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/timer$(LIB_VARIANT).rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/sci$(LIB_VARIANT).rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/score$(LIB_VARIANT).rel

EXTRA_LIBRARIES += libbsp_g.a
CLEANFILES += libbsp_g.a
libbsp_g_a_SOURCES = $(libbsp_a_SOURCES)
libbsp_g_a_LIBADD = $(libbsp_a_LIBADD)

noinst_DATA += libbsp$(LIB_VARIANT).a

all-local: $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

PREINSTALL_DIRS =
TMPINSTALL_FILES =

$(PROJECT_INCLUDE)/coverhd.h: include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

$(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT): start$(LIB_VARIANT).$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)

$(PROJECT_LIB)/linkcmds: startup/linkcmds $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds
TMPINSTALL_FILES += $(PROJECT_LIB)/linkcmds

$(PROJECT_LIB)/linkcmds.ram: startup/linkcmds.ram $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds.ram
TMPINSTALL_FILES += $(PROJECT_LIB)/linkcmds.ram

$(PROJECT_LIB)/linkcmds.rom: startup/linkcmds.rom $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds.rom
TMPINSTALL_FILES += $(PROJECT_LIB)/linkcmds.rom

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)
CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
