##
## $Id$
##

LIBNAME = libnetchip
LIB = $(ARCH)/$(LIBNAME).a

C_FILES = cs8900.c dec21140.c i82586.c sonic.c if_fxp.c elnk.c open_eth.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

#
# Add local stuff here using +=
#

AM_CPPFLAGS += -D__INSIDE_RTEMS_BSD_TCPIP_STACK__

if LIBCHIP
include_libchipdir = $(includedir)/libchip
include_libchip_HEADERS = cs8900.h i82586var.h if_fxpvar.h sonic.h \
    if_media.h mii.h open_eth.h

$(LIB): $(OBJS)
	$(make-library)

$(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a

if HAS_NETWORKING
all-local: $(PREINSTALL_FILES) $(OBJS) $(LIB) $(TMPINSTALL_FILES)
endif
endif

.PRECIOUS: $(LIB)

EXTRA_DIST = README README.cs8900 README.dec21140 README.i82586 \
    README.open_eth README.sonic cs8900.c cs8900.c.bsp dec21140.c i82586.c \
    if_fxp.c open_eth.c sonic.c

PREINSTALL_FILES =

if LIBCHIP
$(PROJECT_INCLUDE)/libchip/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/libchip
	@: > $(PROJECT_INCLUDE)/libchip/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/$(dirstamp)

$(PROJECT_INCLUDE)/libchip/cs8900.h: cs8900.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/cs8900.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/cs8900.h

$(PROJECT_INCLUDE)/libchip/i82586var.h: i82586var.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/i82586var.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/i82586var.h

$(PROJECT_INCLUDE)/libchip/if_fxpvar.h: if_fxpvar.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/if_fxpvar.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/if_fxpvar.h

$(PROJECT_INCLUDE)/libchip/sonic.h: sonic.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/sonic.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/sonic.h

$(PROJECT_INCLUDE)/libchip/if_media.h: if_media.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/if_media.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/if_media.h

$(PROJECT_INCLUDE)/libchip/mii.h: mii.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/mii.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/mii.h

$(PROJECT_INCLUDE)/libchip/open_eth.h: open_eth.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/open_eth.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/open_eth.h

endif

include $(top_srcdir)/automake/local.am
