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

EXTRA_DIST = start/start.S
start$(LIB_VARIANT).$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<
project_lib_DATA = start$(LIB_VARIANT).$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

EXTRA_PROGRAMS += startup.rel
CLEANFILES += startup.rel
startup_rel_SOURCES = startup/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c startup/bspstart.c ../../shared/bootcard.c \
    ../../shared/main.c ../../shared/sbrk.c startup/setvec.c \
    startup/inittlb.c ../../shared/gnatinstallhandler.c startup/idtmem.S \
    startup/idttlb.S
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
console_rel_SOURCES = console/console.c console/led.S
console_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += console_g.rel
CLEANFILES += console_g.rel
console_g_rel_SOURCES = $(console_rel_SOURCES)
console_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
console_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += console$(LIB_VARIANT).rel

EXTRA_PROGRAMS += liblnk.rel
CLEANFILES += liblnk.rel
liblnk_rel_SOURCES = liblnk/liblnk.S
liblnk_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
liblnk_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += liblnk_g.rel
CLEANFILES += liblnk_g.rel
liblnk_g_rel_SOURCES = $(liblnk_rel_SOURCES)
liblnk_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
liblnk_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += liblnk$(LIB_VARIANT).rel

EXTRA_LIBRARIES = libbsp.a
CLEANFILES += libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD = startup$(LIB_VARIANT).rel console$(LIB_VARIANT).rel \
    liblnk$(LIB_VARIANT).rel
libbsp_a_LIBADD += ../../../libcpu/mips/shared/cache$(LIB_VARIANT).rel \
    ../../../libcpu/mips/shared/interrupts$(LIB_VARIANT).rel \
    ../../../libcpu/mips/r46xx/clock$(LIB_VARIANT).rel \
    ../../../libcpu/mips/r46xx/timer$(LIB_VARIANT).rel \
    ../../../libcpu/mips/r46xx/vectorisrs$(LIB_VARIANT).rel

EXTRA_LIBRARIES += libbsp_g.a
CLEANFILES += libbsp_g.a
libbsp_g_a_SOURCES = $(libbsp_a_SOURCES)
libbsp_g_a_LIBADD = $(libbsp_a_LIBADD)

noinst_DATA += libbsp$(LIB_VARIANT).a

all-local: $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

EXTRA_DIST += times

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

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)
CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
