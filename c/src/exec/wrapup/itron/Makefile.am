##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

if HAS_ITRON
LIBNAME = libitron
LIB = $(ARCH)/$(LIBNAME).a
endif

include $(top_srcdir)/../../../automake/multilib.am
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

if HAS_ITRON

ITRON_OBJS = $(wildcard ../../itron/src/$(ARCH)/*.o)

OBJS = $(ITRON_OBJS)

$(LIB): ${OBJS}
	$(make-library)

ITRON_LIB = $(PROJECT_RELEASE)/lib$(MULTISUBDIR)/$(LIBNAME)$(LIB_VARIANT).a

$(PROJECT_RELEASE)/lib$(MULTISUBDIR)/$(LIBNAME)$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

all-local: ${ARCH} $(LIB) $(TMPINSTALL_FILES)

endif

TMPINSTALL_FILES += $(ITRON_LIB)

include $(top_srcdir)/../../../automake/local.am
