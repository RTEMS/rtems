##
## $Id$
##


include_libchipdir = $(includedir)/libchip

LIBNAME = librtcio
LIB = $(ARCH)/$(LIBNAME).a

C_FILES = rtcprobe.c icm7170.c icm7170_reg.c icm7170_reg2.c icm7170_reg4.c \
    icm7170_reg8.c m48t08.c m48t08_reg.c m48t08_reg2.c m48t08_reg4.c \
    m48t08_reg8.c

C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include_libchip_HEADERS = rtc.h icm7170.h m48t08.h

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

PREINSTALL_FILES = $(PROJECT_INCLUDE)/libchip \
    $(include_libchip_HEADERS:%=$(PROJECT_INCLUDE)/libchip/%)

$(PROJECT_INCLUDE)/libchip:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/libchip/%.h: %.h
	$(INSTALL_DATA) $< $@

#
# Add local stuff here using +=
#

AM_CFLAGS += $(LIBC_DEFINES)

$(LIB): $(OBJS)
	$(make-library)

all-local: $(ARCH) $(PREINSTALL_FILES) $(LIB)

DOC_FILES = README.ds1643 README.icm7170 README.m48t08 README.m48t18 STATUS

EXTRA_DIST = $(C_FILES) $(DOC_FILES)

include $(top_srcdir)/../../../automake/local.am
