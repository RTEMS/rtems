##
## $Id$
##

include $(top_srcdir)/../../../automake/compile.am

EXTRA_DIST =
noinst_PROGRAMS =
CLEANFILES =

if mpc6xx
include_libcpudir = $(includedir)/libcpu
include_libcpu_HEADERS = exceptions/raw_exception.h

# exceptions
noinst_PROGRAMS += exceptions.rel
exceptions_rel_SOURCES = exceptions/raw_exception.c exceptions/asm_utils.S
exceptions_rel_CPPFLAGS = $(AM_CPPFLAGS)
exceptions_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# mmu
include_libcpu_HEADERS += mmu/bat.h mmu/pte121.h

noinst_PROGRAMS += mmu.rel
mmu_rel_SOURCES = mmu/bat.c mmu/bat.h mmu/pte121.c mmu/pte121.h \
    mmu/mmuAsm.S
mmu_rel_CPPFLAGS = $(AM_CPPFLAGS)
mmu_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# clock
include_libcpu_HEADERS += clock/c_clock.h

noinst_PROGRAMS += clock.rel
clock_rel_SOURCES = clock/c_clock.c clock/c_clock.h
clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# timer
noinst_PROGRAMS += timer.rel
timer_rel_SOURCES = timer/timer.c
timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
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
