##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

EXTRA_PROGRAMS =
CLEANFILES =
noinst_DATA =

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . tools

include_HEADERS += include/coverhd.h
include_HEADERS += include/page_table.h
include_HEADERS += include/tod.h

nodist_include_HEADERS += $(top_srcdir)/../shared/mvme/mvme16x_hw.h

EXTRA_DIST = ../../m68k/shared/start.S
start$(LIB_VARIANT).$(OBJEXT): ../../m68k/shared/start.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<

project_lib_DATA = start$(LIB_VARIANT).$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

EXTRA_PROGRAMS += startup.rel
CLEANFILES += startup.rel
startup_rel_SOURCES = startup/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c startup/bspstart.c ../../shared/bootcard.c \
    ../../m68k/shared/m68kpretaskinghook.c ../../shared/main.c \
    startup/page_table.c ../../shared/sbrk.c ../../m68k/shared/setvec.c \
    ../../shared/gnatinstallhandler.c
startup_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += startup_g.rel
CLEANFILES += startup_g.rel
startup_g_rel_SOURCES = $(startup_rel_SOURCES)
startup_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
startup_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += startup$(LIB_VARIANT).rel

EXTRA_PROGRAMS += clock.rel
CLEANFILES += clock.rel
clock_rel_SOURCES = clock/ckinit.c
clock_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += clock_g.rel
CLEANFILES += clock_g.rel
clock_g_rel_SOURCES = $(clock_rel_SOURCES)
clock_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
clock_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += clock$(LIB_VARIANT).rel

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

EXTRA_PROGRAMS += timer.rel
CLEANFILES += timer.rel
timer_rel_SOURCES = timer/timer.c timer/timerisr.S
timer_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += timer_g.rel
CLEANFILES += timer_g.rel
timer_g_rel_SOURCES = $(timer_rel_SOURCES)
timer_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
timer_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += timer$(LIB_VARIANT).rel

EXTRA_PROGRAMS += tod.rel
CLEANFILES += tod.rel
tod_rel_SOURCES = tod/tod.c
tod_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
tod_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += tod_g.rel
CLEANFILES += tod_g.rel
tod_g_rel_SOURCES = $(tod_rel_SOURCES)
tod_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
tod_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += tod$(LIB_VARIANT).rel

if HAS_NETWORKING
network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
EXTRA_PROGRAMS += network.rel
CLEANFILES += network.rel
network_rel_SOURCES = ../mvme167/network/network.c
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
#--
EXTRA_LIBRARIES = libbsp.a
CLEANFILES += libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD = startup$(LIB_VARIANT).rel clock$(LIB_VARIANT).rel \
    console$(LIB_VARIANT).rel timer$(LIB_VARIANT).rel tod$(LIB_VARIANT).rel
if HAS_NETWORKING
libbsp_a_LIBADD += network$(LIB_VARIANT).rel
endif
libbsp_a_LIBADD += \
    ../../../libcpu/@RTEMS_CPU@/shared/cache$(LIB_VARIANT).rel \
    ../../../libcpu/@RTEMS_CPU@/shared/misc$(LIB_VARIANT).rel
if HAS_FPSP
libbsp_a_LIBADD += \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/fpsp$(LIB_VARIANT).rel
endif

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

$(PROJECT_INCLUDE)/page_table.h: include/page_table.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/page_table.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/page_table.h

$(PROJECT_INCLUDE)/tod.h: include/tod.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/tod.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/tod.h

$(PROJECT_INCLUDE)/mvme16x_hw.h: $(top_srcdir)/../shared/mvme/mvme16x_hw.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mvme16x_hw.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mvme16x_hw.h

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
