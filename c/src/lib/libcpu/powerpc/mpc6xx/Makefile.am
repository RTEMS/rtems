##
## $Id$
##

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

EXTRA_DIST =
EXTRA_PROGRAMS =
CLEANFILES =
noinst_DATA =

if mpc6xx
include_libcpudir = $(includedir)/libcpu
include_libcpu_HEADERS = exceptions/raw_exception.h

# exceptions
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

# mmu
include_libcpu_HEADERS += mmu/bat.h mmu/pte121.h

EXTRA_PROGRAMS += mmu.rel
CLEANFILES += mmu.rel
mmu_rel_SOURCES = mmu/bat.c mmu/bat.h mmu/pte121.c mmu/pte121.h \
    mmu/mmuAsm.S
mmu_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
mmu_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += mmu_g.rel
CLEANFILES += mmu_g.rel
mmu_g_rel_SOURCES = $(mmu_rel_SOURCES)
mmu_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
mmu_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += mmu$(LIB_VARIANT).rel

# clock
include_libcpu_HEADERS += clock/c_clock.h

EXTRA_PROGRAMS += clock.rel
CLEANFILES += clock.rel
clock_rel_SOURCES = clock/c_clock.c clock/c_clock.h
clock_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += clock_g.rel
CLEANFILES += clock_g.rel
clock_g_rel_SOURCES = $(clock_rel_SOURCES)
clock_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
clock_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += clock$(LIB_VARIANT).rel

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

## ----
all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

if mpc6xx
$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/raw_exception.h: exceptions/raw_exception.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/raw_exception.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/raw_exception.h

$(PROJECT_INCLUDE)/libcpu/bat.h: mmu/bat.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/bat.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/bat.h

$(PROJECT_INCLUDE)/libcpu/pte121.h: mmu/pte121.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/pte121.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/pte121.h

$(PROJECT_INCLUDE)/libcpu/c_clock.h: clock/c_clock.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/c_clock.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/c_clock.h
endif

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/local.am
