##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_libchipdir = $(includedir)/libchip

LIBNAME = libnetchip
LIB = $(ARCH)/$(LIBNAME).a

# add cs8900.c to work with it and make it compile
C_FILES = dec21140.c i82586.h sonic.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

include_libchip_HEADERS = cs8900.h i82586var.h sonic.h

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

$(PROJECT_INCLUDE)/libchip:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/libchip/%.h: %.h
	$(INSTALL_DATA) $< $@

#
# Add local stuff here using +=
#

AM_CPPFLAGS += -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
AM_CFLAGS += $(LIBC_DEFINES)

$(LIB): $(OBJS)
	$(make-library)

$(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

if HAS_NETWORKING
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip \
    $(include_libchip_HEADERS:%=$(PROJECT_INCLUDE)/libchip/%) 
endif

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a

if HAS_NETWORKING
all-local: $(ARCH) $(PREINSTALL_FILES) $(OBJS) $(LIB) $(TMPINSTALL_FILES)
endif

.PRECIOUS: $(LIB)

EXTRA_DIST = README README.cs8900 README.dec21140 README.i82586 README.sonic \
    cs8900.c dec21140.c i82586.c sonic.c

include $(top_srcdir)/../../../automake/local.am
