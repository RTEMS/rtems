##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

if HAS_POSIX
LIBNAME = libposix
LIB = $(ARCH)/$(LIBNAME).a
endif

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/lib.am

if HAS_POSIX

POSIX_OBJS = $(wildcard ../../posix/src/$(ARCH)/*.o)

OBJS = $(POSIX_OBJS)

$(LIB): ${OBJS}
	$(make-library)

POSIX_LIB = $(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a

$(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

all: ${ARCH} $(LIB) $(TMPINSTALL_FILES)

endif

TMPINSTALL_FILES += $(POSIX_LIB)

include $(top_srcdir)/../../../automake/local.am
