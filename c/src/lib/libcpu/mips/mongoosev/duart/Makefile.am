##
## $Id$
##


PGM = $(ARCH)/mg5uart.rel

C_FILES = mg5uart.c mg5uart_reg.c

mg5uart_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT)) $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/compile.am
include $(top_srcdir)/../../../../../automake/lib.am

$(PROJECT_INCLUDE)/libchip:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/libchip/%.h: %.h
	$(INSTALL_DATA) $< $@

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(mg5uart_rel_OBJECTS)
	$(make-rel)

## include_libchip_HEADERS = mg5uart.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)/libchip \
       $(PROJECT_INCLUDE)/libchip/mg5uart.h

##    $(include_libchip_HEADERS:%=$(PROJECT_INCLUDE)/libchip/%)
all-local: $(ARCH) $(PREINSTALL_FILES) $(mg5uart_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = mg5uart.c README.mguart mg5uart.c mg5uart.h mg5uart_reg.c

include $(top_srcdir)/../../../../../automake/local.am
