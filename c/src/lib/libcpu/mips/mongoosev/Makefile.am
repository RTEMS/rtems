##
## $Id$
##

noinst_PROGRAMS =
EXTRA_DIST =

include $(top_srcdir)/../../../automake/compile.am

if mongoosev
include_mipsdir = $(includedir)/libcpu
include_mips_HEADERS = include/mongoose-v.h

## duart
include_libchipdir = $(includedir)/libchip
include_libchip_HEADERS = duart/mg5uart.h

EXTRA_DIST += duart/README.mguart

noinst_PROGRAMS += duart.rel
duart_rel_SOURCES = duart/mg5uart.c duart/mg5uart.h duart/mg5uart_reg.c
duart_rel_CPPFLAGS = $(AM_CPPFLAGS)
duart_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## vectorisrs

noinst_PROGRAMS += vectorisrs.rel
vectorisrs_rel_SOURCES = vectorisrs/maxvectors.c vectorisrs/vectorisrs.c
vectorisrs_rel_CPPFLAGS = $(AM_CPPFLAGS)
vectorisrs_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

if mongoosev
$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/mongoose-v.h: include/mongoose-v.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/mongoose-v.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/mongoose-v.h

$(PROJECT_INCLUDE)/libchip/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libchip
	@: > $(PROJECT_INCLUDE)/libchip/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/libchip/$(dirstamp)

$(PROJECT_INCLUDE)/libchip/mg5uart.h: duart/mg5uart.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/mg5uart.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/mg5uart.h
endif

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/local.am
