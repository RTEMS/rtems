#
#  $Id$
#

LIBNAME = libserialio
LIB = $(ARCH)/$(LIBNAME).a

C_FILES = mc68681.c mc68681_baud.c mc68681_reg.c mc68681_reg2.c \
    mc68681_reg4.c mc68681_reg8.c ns16550.c z85c30.c z85c30_reg.c serprobe.c \
    termios_baud2index.c termios_baud2num.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

noinst_HEADERS = mc68681_p.h ns16550_p.h z85c30_p.h

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

#
# Add local stuff here using +=
#
if LIBCHIP
include_libchipdir = $(includedir)/libchip
include_libchip_HEADERS = mc68681.h ns16550.h z85c30.h serial.h sersupp.h

$(LIB): $(OBJS)
	$(make-library)

all-local: $(PREINSTALL_FILES) $(LIB)
endif

DOC_FILES = README.mc68681 README.ns16550 README.xr88681 README.z85c30 \
    STATUS

EXTRA_DIST = $(C_FILES) $(DOC_FILES)

PREINSTALL_FILES =

if LIBCHIP
$(PROJECT_INCLUDE)/libchip/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/libchip
	@: > $(PROJECT_INCLUDE)/libchip/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/$(dirstamp)

$(PROJECT_INCLUDE)/libchip/mc68681.h: mc68681.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/mc68681.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/mc68681.h

$(PROJECT_INCLUDE)/libchip/ns16550.h: ns16550.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/ns16550.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/ns16550.h

$(PROJECT_INCLUDE)/libchip/z85c30.h: z85c30.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/z85c30.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/z85c30.h

$(PROJECT_INCLUDE)/libchip/serial.h: serial.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/serial.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/serial.h

$(PROJECT_INCLUDE)/libchip/sersupp.h: sersupp.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/sersupp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/sersupp.h
endif

include $(top_srcdir)/automake/local.am
