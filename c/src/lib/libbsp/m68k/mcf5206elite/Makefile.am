##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

EXTRA_PROGRAMS =
CLEANFILES =
noinst_DATA =

SUBDIRS = . tools

include_HEADERS += include/coverhd.h
include_HEADERS += include/i2c.h
include_HEADERS += include/i2cdrv.h
include_HEADERS += include/ds1307.h
include_HEADERS += include/nvram.h
nodist_include_HEADERS += ../../shared/tod.h

EXTRA_DIST = times

EXTRA_DIST += start/start.S
start$(LIB_VARIANT).$(OBJEXT): start/start.S
	$(CCASCOMPILE) -DASM -o $@ -c $<

project_lib_DATA = start$(LIB_VARIANT).$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds startup/gdbinit \
    startup/linkcmds.flash

EXTRA_PROGRAMS += startup.rel
CLEANFILES += startup.rel
startup_rel_SOURCES = startup/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c ../shared/m68kpretaskinghook.c startup/bspstart.c \
    startup/init5206e.c ../../shared/bootcard.c ../../shared/main.c \
    ../../shared/sbrk.c ../shared/setvec.c \
    ../../shared/gnatinstallhandler.c
startup_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += startup_g.rel
CLEANFILES += startup_g.rel
startup_g_rel_SOURCES = $(startup_rel_SOURCES)
startup_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
startup_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += startup$(LIB_VARIANT).rel

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

EXTRA_PROGRAMS += i2c.rel
CLEANFILES += i2c.rel
i2c_rel_SOURCES = i2c/i2c.c i2c/i2cdrv.c
i2c_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
i2c_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += i2c_g.rel
CLEANFILES += i2c_g.rel
i2c_g_rel_SOURCES = $(i2c_rel_SOURCES)
i2c_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
i2c_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += i2c$(LIB_VARIANT).rel

EXTRA_PROGRAMS += tod.rel
CLEANFILES += tod.rel
tod_rel_SOURCES = tod/ds1307.c tod/todcfg.c ../../shared/tod.c
tod_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
tod_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += tod_g.rel
CLEANFILES += tod_g.rel
tod_g_rel_SOURCES = $(tod_rel_SOURCES)
tod_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
tod_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += tod$(LIB_VARIANT).rel

EXTRA_PROGRAMS += nvram.rel
CLEANFILES += nvram.rel
nvram_rel_SOURCES = nvram/nvram.c
nvram_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
nvram_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += nvram_g.rel
CLEANFILES += nvram_g.rel
nvram_g_rel_SOURCES = $(nvram_rel_SOURCES)
nvram_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
nvram_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += nvram$(LIB_VARIANT).rel

EXTRA_LIBRARIES = libbsp.a
CLEANFILES += libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD = startup$(LIB_VARIANT).rel console$(LIB_VARIANT).rel \
    i2c$(LIB_VARIANT).rel tod$(LIB_VARIANT).rel nvram$(LIB_VARIANT).rel
libbsp_a_LIBADD += \
    ../../../libcpu/@RTEMS_CPU@/shared/cache$(LIB_VARIANT).rel \
    ../../../libcpu/@RTEMS_CPU@/shared/misc$(LIB_VARIANT).rel \
    ../../../libcpu/@RTEMS_CPU@/mcf5206/clock$(LIB_VARIANT).rel \
    ../../../libcpu/@RTEMS_CPU@/mcf5206/console$(LIB_VARIANT).rel \
    ../../../libcpu/@RTEMS_CPU@/mcf5206/timer$(LIB_VARIANT).rel \
    ../../../libcpu/@RTEMS_CPU@/mcf5206/mbus$(LIB_VARIANT).rel

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

$(PROJECT_INCLUDE)/i2c.h: include/i2c.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/i2c.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/i2c.h

$(PROJECT_INCLUDE)/i2cdrv.h: include/i2cdrv.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/i2cdrv.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/i2cdrv.h

$(PROJECT_INCLUDE)/ds1307.h: include/ds1307.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/ds1307.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/ds1307.h

$(PROJECT_INCLUDE)/nvram.h: include/nvram.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/nvram.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/nvram.h

$(PROJECT_INCLUDE)/tod.h: ../../shared/tod.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/tod.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/tod.h

$(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT): start$(LIB_VARIANT).$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)

$(PROJECT_LIB)/linkcmds: startup/linkcmds $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds
TMPINSTALL_FILES += $(PROJECT_LIB)/linkcmds

$(PROJECT_LIB)/gdbinit: startup/gdbinit $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/gdbinit
TMPINSTALL_FILES += $(PROJECT_LIB)/gdbinit

$(PROJECT_LIB)/linkcmds.flash: startup/linkcmds.flash $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds.flash
TMPINSTALL_FILES += $(PROJECT_LIB)/linkcmds.flash

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)
CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
