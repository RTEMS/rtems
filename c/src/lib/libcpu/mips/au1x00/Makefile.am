##
## $Id$
##

EXTRA_PROGRAMS =
CLEANFILES =
noinst_DATA =

include $(top_srcdir)/../../../automake/compile.am

if au1x00
include_libcpudir = $(includedir)/libcpu
include_libcpu_HEADERS = include/au1x00.h

EXTRA_PROGRAMS += vectorisrs.rel
CLEANFILES += vectorisrs.rel
vectorisrs_rel_SOURCES = vectorisrs/maxvectors.c vectorisrs/vectorisrs.c
vectorisrs_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
vectorisrs_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += vectorisrs_g.rel
CLEANFILES += vectorisrs_g.rel
vectorisrs_g_rel_SOURCES = $(vectorisrs_rel_SOURCES)
vectorisrs_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
vectorisrs_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += vectorisrs$(LIB_VARIANT).rel
endif

all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

if au1x00
$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/au1x00.h: include/au1x00.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/au1x00.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/au1x00.h
endif

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/local.am
