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

SUBDIRS = . tools

include_HEADERS += include/coverhd.h
include_HEADERS += include/i2c.h
include_HEADERS += include/i2cdrv.h
include_HEADERS += include/ds1307.h
include_HEADERS += include/nvram.h
nodist_include_HEADERS += ../../shared/tod.h

EXTRA_DIST = times

EXTRA_DIST += start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<

project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds startup/gdbinit \
    startup/linkcmds.flash

noinst_PROGRAMS += startup.rel
startup_rel_SOURCES = startup/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c ../../m68k/shared/m68kpretaskinghook.c \
    startup/bspstart.c startup/init5206e.c ../../shared/bootcard.c \
    ../../shared/main.c ../../shared/sbrk.c ../../m68k/shared/setvec.c \
    ../../shared/gnatinstallhandler.c
startup_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += console.rel
console_rel_SOURCES = console/console.c
console_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += i2c.rel
i2c_rel_SOURCES = i2c/i2c.c i2c/i2cdrv.c
i2c_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
i2c_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += tod.rel
tod_rel_SOURCES = tod/ds1307.c tod/todcfg.c ../../shared/tod.c
tod_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
tod_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += nvram.rel
nvram_rel_SOURCES = nvram/nvram.c
nvram_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
nvram_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD = startup.rel console.rel i2c.rel tod.rel nvram.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/misc.rel \
    ../../../libcpu/@RTEMS_CPU@/mcf5206/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/mcf5206/mcfuart.rel \
    ../../../libcpu/@RTEMS_CPU@/mcf5206/timer.rel \
    ../../../libcpu/@RTEMS_CPU@/mcf5206/mbus.rel

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

$(PROJECT_LIB)/start.$(OBJEXT): start.$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/start.$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/start.$(OBJEXT)

$(PROJECT_LIB)/linkcmds: startup/linkcmds $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds
PREINSTALL_FILES += $(PROJECT_LIB)/linkcmds

$(PROJECT_LIB)/gdbinit: startup/gdbinit $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/gdbinit
PREINSTALL_FILES += $(PROJECT_LIB)/gdbinit

$(PROJECT_LIB)/linkcmds.flash: startup/linkcmds.flash $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds.flash
PREINSTALL_FILES += $(PROJECT_LIB)/linkcmds.flash

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES += $(PREINSTALL_DIRS)
CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
