##
## $Id$
##


LIBNAME = libmisc
LIB = $(ARCH)/$(LIBNAME).a

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

## XXX temporarily remove this from the list because it causes a 
## XXX number of BSPs to not link "main(){}" used by autoconf
##    ../serdbg/$(ARCH)/libserdbg-tmp.a
 
TMP_LIBS = ../monitor/$(ARCH)/libmonitor-tmp.a \
    ../untar/$(ARCH)/libuntar-tmp.a ../stackchk/$(ARCH)/libstackchk-tmp.a \
    ../cpuuse/$(ARCH)/libcpuuse-tmp.a ../rtmonuse/$(ARCH)/librtmonuse-tmp.a \
    ../shell/$(ARCH)/libshell-tmp.a ../dumpbuf/$(ARCH)/libdumpbuf-tmp.a \
    ../devnull/$(ARCH)/libdevnull-tmp.a ../dummy/$(ARCH)/libdummy-tmp.a \
    ../mw-fb/$(ARCH)/libmw-fb-tmp.a ../capture/$(ARCH)/libcapture-tmp.a

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

all-local: $(ARCH) $(LIB)

include $(top_srcdir)/../../../automake/local.am
