##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../../aclocal

EXTRA_PROGRAMS =
CLEANFILES =
noinst_DATA =

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

if tx39
include_libcpudir = $(includedir)/libcpu
include_libcpu_HEADERS = include/tx3904.h

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

PREINSTALL_FILES =

if tx39
$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/tx3904.h: include/tx3904.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/tx3904.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/tx3904.h
endif

CLEANFILES += $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
