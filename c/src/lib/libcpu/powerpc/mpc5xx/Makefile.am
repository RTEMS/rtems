##
## Makefile.am,v 1.4 2002/03/28 00:48:14 joel Exp
##

CLEANFILES =
EXTRA_DIST =
EXTRA_PROGRAMS =
noinst_DATA =

include $(top_srcdir)/../../../automake/compile.am

if mpc5xx
include_libcpudir = $(includedir)/libcpu

# exceptions
include_libcpu_HEADERS = exceptions/raw_exception.h

EXTRA_PROGRAMS += exceptions.rel
CLEANFILES += exceptions.rel
exceptions_rel_SOURCES = exceptions/raw_exception.c exceptions/asm_utils.S
exceptions_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
exceptions_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += exceptions_g.rel
CLEANFILES += exceptions_g.rel
exceptions_g_rel_SOURCES = $(exceptions_rel_SOURCES)
exceptions_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
exceptions_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += exceptions$(LIB_VARIANT).rel

# ictrl
include_HEADERS = ictrl/ictrl.h

EXTRA_PROGRAMS += ictrl.rel
CLEANFILES += ictrl.rel
ictrl_rel_SOURCES = ictrl/ictrl.c ictrl/ictrl.h
ictrl_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
ictrl_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += ictrl_g.rel
CLEANFILES += ictrl_g.rel
ictrl_g_rel_SOURCES = $(ictrl_rel_SOURCES)
ictrl_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
ictrl_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += ictrl$(LIB_VARIANT).rel

# timer
EXTRA_PROGRAMS += timer.rel
CLEANFILES += timer.rel
timer_rel_SOURCES = timer/timer.c
timer_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += timer_g.rel
CLEANFILES += timer_g.rel
timer_g_rel_SOURCES = $(timer_rel_SOURCES)
timer_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
timer_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += timer$(LIB_VARIANT).rel
endif

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/$(dirstamp)

if mpc5xx
$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/raw_exception.h: exceptions/raw_exception.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/raw_exception.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/raw_exception.h

$(PROJECT_INCLUDE)/ictrl.h: ictrl/ictrl.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/ictrl.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/ictrl.h
endif

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../../../automake/local.am
