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

EXTRA_DIST =

include_HEADERS += include/bare.h
include_HEADERS += include/crc.h
include_HEADERS += include/debugport.h
include_HEADERS += include/m68302scc.h
include_HEADERS += include/coverhd.h

# FIXME: Better merge both files into one and use #ifdef GDB_MONITOR_ACTIVE
if ODS68302_DEBUG
EXTRA_DIST += start/debugreset.S
start.$(OBJEXT): start/debugreset.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<
else
EXTRA_DIST += start/reset.S
start.$(OBJEXT): start/reset.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<
endif

project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds startup/rom startup/debugger

noinst_PROGRAMS += startup.rel
startup_rel_SOURCES = startup/cpuboot.c startup/crc.c startup/debugport.c \
    startup/gdb-hooks.c ../../shared/bootcard.c ../../shared/main.c \
    startup/m68302scc.c startup/m68k-stub.c \
    ../../m68k/shared/m68kpretaskinghook.c startup/memcheck.c \
    startup/trace.c ../../shared/gnatinstallhandler.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c startup/bspstart.c startup/bspclean.c \
    ../../shared/sbrk.c ../../m68k/shared/setvec.c
startup_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += clock.rel
clock_rel_SOURCES = clock/ckinit.c
clock_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += console.rel
console_rel_SOURCES = console/console.c
console_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += timer.rel
timer_rel_SOURCES = timer/timer.c timer/timerisr.S
timer_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD = startup.rel clock.rel console.rel timer.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/misc.rel

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

$(PROJECT_INCLUDE)/bare.h: include/bare.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bare.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bare.h

$(PROJECT_INCLUDE)/crc.h: include/crc.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/crc.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/crc.h

$(PROJECT_INCLUDE)/debugport.h: include/debugport.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/debugport.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/debugport.h

$(PROJECT_INCLUDE)/m68302scc.h: include/m68302scc.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/m68302scc.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/m68302scc.h

$(PROJECT_INCLUDE)/coverhd.h: include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

$(PROJECT_LIB)/start.$(OBJEXT): start.$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/start.$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/start.$(OBJEXT)

$(PROJECT_LIB)/linkcmds: startup/linkcmds $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds
PREINSTALL_FILES += $(PROJECT_LIB)/linkcmds

$(PROJECT_LIB)/rom: startup/rom $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/rom
PREINSTALL_FILES += $(PROJECT_LIB)/rom

$(PROJECT_LIB)/debugger: startup/debugger $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/debugger
PREINSTALL_FILES += $(PROJECT_LIB)/debugger

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES += $(PREINSTALL_DIRS)
CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
