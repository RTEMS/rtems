##
## $Id$
##

LIBNAME = librtcio
LIB = $(ARCH)/$(LIBNAME).a

C_FILES = rtcprobe.c icm7170.c icm7170_reg.c icm7170_reg2.c icm7170_reg4.c \
    icm7170_reg8.c m48t08.c m48t08_reg.c m48t08_reg2.c m48t08_reg4.c \
    m48t08_reg8.c

OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

#
# Add local stuff here using +=
#
if LIBCHIP
include_libchipdir = $(includedir)/libchip
include_libchip_HEADERS = rtc.h icm7170.h m48t08.h

$(LIB): $(OBJS)
	$(make-library)

all-local: $(PREINSTALL_FILES) $(LIB)
endif

DOC_FILES = README.ds1643 README.icm7170 README.m48t08 README.m48t18 STATUS

EXTRA_DIST = $(C_FILES) $(DOC_FILES)

PREINSTALL_FILES =

if LIBCHIP
$(PROJECT_INCLUDE)/libchip/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/libchip
	@: > $(PROJECT_INCLUDE)/libchip/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/$(dirstamp)

$(PROJECT_INCLUDE)/libchip/rtc.h: rtc.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/rtc.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/rtc.h

$(PROJECT_INCLUDE)/libchip/icm7170.h: icm7170.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/icm7170.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/icm7170.h

$(PROJECT_INCLUDE)/libchip/m48t08.h: m48t08.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/m48t08.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/m48t08.h
endif

include $(top_srcdir)/automake/local.am
