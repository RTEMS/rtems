##
## $Id$
##

EXTRA_PROGRAMS =
CLEANFILES =
noinst_DATA =

include $(top_srcdir)/../../../automake/compile.am

# include
if shared
include_libcpudir = $(includedir)/libcpu

include_libcpu_HEADERS = include/mmu.h 

## arm920
EXTRA_PROGRAMS += arm920.rel
CLEANFILES += arm920.rel
arm920_rel_SOURCES = arm920/mmu.c 
arm920_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V) -I$(srcdir)/src
arm920_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += arm920_g.rel
CLEANFILES += arm920_g.rel
arm920_g_rel_SOURCES = $(arm920_rel_SOURCES)
arm920_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V) -I$(srcdir)/src
arm920_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += arm920$(LIB_VARIANT).rel

endif

all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

if shared
$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/mmu.h: include/mmu.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/mmu.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/mmu.h

endif

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/local.am
