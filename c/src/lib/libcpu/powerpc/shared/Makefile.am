##
## $Id$
##

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

# include

libcpuspec_C_FILES = include/cpuIdent.c
libcpuspec_OBJS = \
    $(libcpuspec_C_FILES:include/%.c=include/$(ARCH)/%.$(OBJEXT))

include_libcpudir = $(includedir)/libcpu

include_libcpu_HEADERS = include/spr.h include/io.h include/mmu.h \
    include/page.h include/byteorder.h include/pgtable.h include/cpuIdent.h

include/$(ARCH)/$(dirstamp):
	@$(mkdir_p) include/$(ARCH)
	@: > include/$(ARCH)/$(dirstamp)
include/$(ARCH)/%.$(OBJEXT): include/%.c include/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
include/$(ARCH)/libcpuspec.a: $(libcpuspec_OBJS)
	$(make-library)
CLEANDIRS = include/o-optimize include/o-debug

EXTRA_DIST = libcpuspec_C_FILES

# src

src_C_FILES = src/cache.c src/stack.c

common_C_FILES = $(top_srcdir)/../shared/src/cache_aligned_malloc.c
common_C_FILES += $(top_srcdir)/../shared/src/cache_manager.c

src_O_FILES = $(src_C_FILES:src/%.c=src/$(ARCH)/%.$(OBJEXT))
common_O_FILES = \
    $(common_C_FILES:$(top_srcdir)/../shared/src/%.c=src/$(ARCH)/%.$(OBJEXT))

src/$(ARCH)/$(dirstamp):
	@$(mkdir_p) src/$(ARCH)
	@: > src/$(ARCH)/$(dirstamp)
src/$(ARCH)/%.$(OBJEXT): $(top_srcdir)/../shared/src/%.c src/$(ARCH)/$(dirstamp)
	${COMPILE} -I$(srcdir)/src -o $@ -c $<
src/$(ARCH)/%.$(OBJEXT): src/%.c src/$(ARCH)/$(dirstamp)
	${COMPILE} -I$(srcdir)/src -o $@ -c $<
CLEANDIRS += src/o-optimize src/o-debug

noinst_HEADERS = src/cache_.h
include_libcpu_HEADERS += src/stackTrace.h
include_libcpu_HEADERS += $(top_srcdir)/../shared/include/cache.h

all-local: $(PREINSTALL_FILES) include/$(ARCH)/libcpuspec.a $(src_O_FILES) $(common_O_FILES)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/spr.h: include/spr.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/spr.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/spr.h

$(PROJECT_INCLUDE)/libcpu/io.h: include/io.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/io.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/io.h

$(PROJECT_INCLUDE)/libcpu/mmu.h: include/mmu.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/mmu.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/mmu.h

$(PROJECT_INCLUDE)/libcpu/page.h: include/page.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/page.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/page.h

$(PROJECT_INCLUDE)/libcpu/byteorder.h: include/byteorder.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/byteorder.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/byteorder.h

$(PROJECT_INCLUDE)/libcpu/pgtable.h: include/pgtable.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/pgtable.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/pgtable.h

$(PROJECT_INCLUDE)/libcpu/cpuIdent.h: include/cpuIdent.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/cpuIdent.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/cpuIdent.h

$(PROJECT_INCLUDE)/libcpu/stackTrace.h: src/stackTrace.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/stackTrace.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/stackTrace.h

$(PROJECT_INCLUDE)/libcpu/cache.h: $(top_srcdir)/../shared/include/cache.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/cache.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/cache.h

include $(top_srcdir)/../../../automake/local.am
