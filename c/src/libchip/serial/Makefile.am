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

include $(top_srcdir)/../../automake/compile.am
include $(top_srcdir)/../../automake/lib.am

PREINSTALL_FILES = $(PROJECT_INCLUDE)/libchip \
    $(include_libchip_HEADERS:%=$(PROJECT_INCLUDE)/libchip/%)

$(PROJECT_INCLUDE)/libchip:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/libchip/%.h: %.h
	$(INSTALL_DATA) $< $@

#
# Add local stuff here using +=
#
if LIBCHIP
include_libchipdir = $(includedir)/libchip
include_libchip_HEADERS = mc68681.h ns16550.h z85c30.h serial.h sersupp.h

$(LIB): $(OBJS)
	$(make-library)

all-local: $(ARCH) $(PREINSTALL_FILES) $(LIB)
endif

DOC_FILES = README.mc68681 README.ns16550 README.xr88681 README.z85c30 \
    STATUS

EXTRA_DIST = $(C_FILES) $(DOC_FILES)

include $(top_srcdir)/../../automake/local.am
