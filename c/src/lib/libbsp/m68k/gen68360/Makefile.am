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

include_HEADERS += include/coverhd.h

EXTRA_DIST = start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds startup/linkcmds.prom \
    startup/linkcmds.bootp

noinst_PROGRAMS += startup.rel
startup_rel_SOURCES = startup/alloc360.c startup/bspclean.c \
    ../../shared/bsplibc.c ../../shared/bsppost.c \
    ../../m68k/shared/m68kpretaskinghook.c startup/bspstart.c \
    startup/init68360.c ../../shared/bootcard.c ../../shared/main.c \
    ../../shared/sbrk.c ../../m68k/shared/setvec.c \
    ../../shared/gnatinstallhandler.c
startup_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += clock.rel
clock_rel_SOURCES = clock/clock.c
clock_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += console.rel
console_rel_SOURCES = console/console.c console/consolereserveresources.c
console_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += timer.rel
timer_rel_SOURCES = timer/timer.c
timer_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

if HAS_NETWORKING
network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += network.rel
network_rel_SOURCES = network/network.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V) \
    $(network_CPPFLAGS)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD = startup.rel clock.rel console.rel timer.rel
if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
endif
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/misc.rel
if HAS_FPSP
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/fpsp.rel
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

$(PROJECT_INCLUDE)/coverhd.h: include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

$(PROJECT_LIB)/start.$(OBJEXT): start.$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/start.$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/start.$(OBJEXT)

$(PROJECT_LIB)/linkcmds: startup/linkcmds $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds
PREINSTALL_FILES += $(PROJECT_LIB)/linkcmds

$(PROJECT_LIB)/linkcmds.prom: startup/linkcmds.prom $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds.prom
PREINSTALL_FILES += $(PROJECT_LIB)/linkcmds.prom

$(PROJECT_LIB)/linkcmds.bootp: startup/linkcmds.bootp $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds.bootp
PREINSTALL_FILES += $(PROJECT_LIB)/linkcmds.bootp

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES += $(PREINSTALL_DIRS)
CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
