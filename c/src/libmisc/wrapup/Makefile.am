##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

LIBNAME = libmisc
LIB = ${ARCH}/$(LIBNAME).a

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(RTEMS_ROOT)/make/lib.cfg

TMP_LIBS = ../monitor/$(ARCH)/libmonitor-tmp.a ../untar/$(ARCH)/libuntar-tmp.a \
    ../stackchk/$(ARCH)/libstackchk-tmp.a ../cpuuse/$(ARCH)/libcpuuse-tmp.a \
    ../rtmonuse/$(ARCH)/librtmonuse-tmp.a \
    ../dumpbuf/$(ARCH)/libdumpbuf-tmp.a

TMPINSTALL_FILES = \
$(PROJECT_RELEASE)/lib \
$(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIBSUFFIX_VA)

$(PROJECT_RELEASE)/lib:
	@$(mkinstalldirs) $@
$(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIBSUFFIX_VA): $(LIB)
	$(INSTALL_DATA) $< $@

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): $(TMP_LIBS)
	$(RM) $@
	$(RM) -r $(ARCH)
	mkdir $(ARCH)
	cd $(ARCH); for lib in $^; do \
                         $(AR) -xv ../$$lib; \
                    done
	$(AR) ruv $@ $(ARCH)/*
	$(RANLIB) $@

all: ${ARCH} $(LIB) $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
