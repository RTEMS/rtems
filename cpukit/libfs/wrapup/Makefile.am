##
## $Id$
##

LIBNAME = libfs
LIB = $(ARCH)/$(LIBNAME).a

include $(top_srcdir)/../automake/multilib.am
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/lib.am

IMFSLIB = ../src/imfs/$(ARCH)/libimfs.a

if !UNIX
DOSFSLIB = ../src/dosfs/$(ARCH)/libdosfs.a
endif

TMP_LIBS = $(IMFSLIB) $(DOSFSLIB)

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

.PRECIOUS: $(LIB)

include $(top_srcdir)/../automake/local.am
