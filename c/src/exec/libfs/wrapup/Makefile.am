##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

LIBNAME = libfs
LIB = $(ARCH)/$(LIBNAME).a

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

IMFSLIB = ../src/imfs/$(ARCH)/libimfs.a

TMP_LIBS = $(IMFSLIB)

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
