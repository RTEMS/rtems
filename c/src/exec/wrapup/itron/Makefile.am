##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

if HAS_ITRON
LIBNAME = libitron
LIB = $(ARCH)/$(LIBNAME).a
endif

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/lib.am

if HAS_ITRON

ITRON_OBJS = $(wildcard ../../itron/src/$(ARCH)/*.o)

OBJS = $(ITRON_OBJS)

$(LIB): ${OBJS}
	$(make-library)

ITRON_LIB = \
$(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a

$(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

all: ${ARCH} $(LIB) $(TMPINSTALL_FILES)

endif

TMPINSTALL_FILES += $(ITRON_LIB)

include $(top_srcdir)/../../../automake/local.am
