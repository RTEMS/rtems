##
## $Id$
##

LIB = $(ARCH)/libmisc.a

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

TMP_LIBS = ../monitor/$(ARCH)/libmonitor.a
TMP_LIBS += ../untar/$(ARCH)/libuntar.a
TMP_LIBS += ../stackchk/$(ARCH)/libstackchk.a
TMP_LIBS += ../cpuuse/$(ARCH)/libcpuuse.a
TMP_LIBS += ../rtmonuse/$(ARCH)/librtmonuse.a

## XXX temporarily remove this from the list because it causes a
## XXX number of BSPs to not link "main(){}" used by autoconf
# if LIBSERDBG
# TMP_LIBS += ../serdbg/$(ARCH)/libserdbg.a
# endif

if LIBSHELL
TMP_LIBS += ../shell/$(ARCH)/libshell.a
endif

TMP_LIBS += ../dumpbuf/$(ARCH)/libdumpbuf.a
TMP_LIBS += ../devnull/$(ARCH)/libdevnull.a
TMP_LIBS += ../dummy/$(ARCH)/libdummy.a
TMP_LIBS += ../mw-fb/$(ARCH)/libmw-fb.a
TMP_LIBS += ../capture/$(ARCH)/libcapture.a
TMP_LIBS += ../fsmount/$(ARCH)/libfsmount.a

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

include $(top_srcdir)/automake/local.am
