##
## $Id$
##

PGM = $(ARCH)/mg5uart.rel

C_FILES = mg5uart.c mg5uart_reg.c

mg5uart_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(mg5uart_rel_OBJECTS)
	$(make-rel)

include_libchipdir = $(includedir)/libchip
include_libchip_HEADERS = mg5uart.h

all-local: $(ARCH) $(PREINSTALL_FILES) $(mg5uart_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = mg5uart.c README.mguart mg5uart.c mg5uart_reg.c

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/libchip:
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/libchip
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip

$(PROJECT_INCLUDE)/libchip/mg5uart.h: mg5uart.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/mg5uart.h

include $(top_srcdir)/../../../automake/local.am
