##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

LIBNAME = libmisc
LIB = $(ARCH)/$(LIBNAME).a

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

if HAS_NETWORKING
NETLIBS=../rootfs/$(ARCH)/librootfs-tmp.a
endif

TMP_LIBS = ../monitor/$(ARCH)/libmonitor-tmp.a \
    ../untar/$(ARCH)/libuntar-tmp.a ../stackchk/$(ARCH)/libstackchk-tmp.a \
    ../cpuuse/$(ARCH)/libcpuuse-tmp.a ../rtmonuse/$(ARCH)/librtmonuse-tmp.a \
    ../shell/$(ARCH)/libshell-tmp.a ../dumpbuf/$(ARCH)/libdumpbuf-tmp.a \
    ../devnull/$(ARCH)/libdevnull-tmp.a ../dummy/$(ARCH)/dummy.rel \
    ../mw-fb/$(ARCH)/libmw-fb-tmp.a $(NETLIBS)

$(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): $(TMP_LIBS)
	$(RM) $@
	$(RM) -r $(ARCH)
	mkdir $(ARCH)
	cd $(ARCH); for lib in $^; do $(AR) -xv ../$$lib; done
	$(AR) ruv $@ $(ARCH)/*
	$(RANLIB) $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a

all-local: $(ARCH) $(LIB) $(TMPINSTALL_FILES)

.PRECIOUS: $(LIB)

include $(top_srcdir)/../../../automake/local.am
