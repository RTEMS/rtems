#
# $Id$
#

AUTOMAKE_OPTIONS = foreign 1.4

LIBNAME = libnetchip
LIB = ${ARCH}/${LIBNAME}.a

C_FILES = dec21140.c sonic.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

H_FILES = sonic.h
noinst_HEADERS = $(H_FILES)

SRCS = $(C_FILES) $(H_FILES)
OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(RTEMS_ROOT)/make/lib.cfg

$(PROJECT_INCLUDE)/libchip:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/libchip/%.h: %.h
	$(INSTALL_DATA) $< $@

#
# Add local stuff here using +=
#

AM_CPPFLAGS += -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
AM_CFLAGS += $(LIBC_DEFINES)

if HAS_NETWORKING

PREINSTALL_FILES = \
$(PROJECT_INCLUDE)/libchip \
$(noinst_HEADERS:%=$(PROJECT_INCLUDE)/libchip/%)

TMPINSTALL_FILES += \
$(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a

$(LIB): ${OBJS}
	$(make-library)

$(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

all: ${ARCH} $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

endif

EXTRA_DIST = README README.sonic
EXTRA_DIST += $(C_FILES)

include $(top_srcdir)/../../../automake/local.am
