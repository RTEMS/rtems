##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../../aclocal

EXTRA_DIST =
CLEANDIRS =
noinst_DATA =

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

include_mipsdir = $(includedir)/libcpu
include_mips_HEADERS = include/mongoose-v.h

## duart
include_libchipdir = $(includedir)/libchip
include_libchip_HEADERS = duart/mg5uart.h

duart_C_FILES = duart/mg5uart.c duart/mg5uart_reg.c
EXTRA_DIST += $(duart_C_FILES) duart/README.mguart

duart_rel_OBJECTS = $(duart_C_FILES:duart/%.c=duart/$(ARCH)/%.$(OBJEXT))

duart/$(ARCH)/$(dirstamp):
	@$(mkdir_p) duart/$(ARCH)
	@: > duart/$(ARCH)/$(dirstamp)
duart/$(ARCH)/%.$(OBJEXT): duart/%.c duart/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
duart/$(ARCH)/mg5uart.rel: $(duart_rel_OBJECTS)
	$(make-rel)
CLEANDIRS += duart/o-optimize duart/o-debug
noinst_DATA += duart/$(ARCH)/mg5uart.rel

## vectorisrs
vectorisrs_C_FILES = vectorisrs/maxvectors.c vectorisrs/vectorisrs.c
EXTRA_DIST += $(vectorisrs_C_FILES)

vectorisrs_rel_OBJECTS = \
    $(vectorisrs_C_FILES:vectorisrs/%.c=vectorisrs/$(ARCH)/%.$(OBJEXT))

vectorisrs/$(ARCH)/$(dirstamp):
	@$(mkdir_p) vectorisrs/$(ARCH)
	@: > vectorisrs/$(ARCH)/$(dirstamp)
vectorisrs/$(ARCH)/%.$(OBJEXT): vectorisrs/%.c vectorisrs/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
vectorisrs/$(ARCH)/vectorisrs.rel: $(vectorisrs_rel_OBJECTS)
	$(make-rel)
CLEANDIRS += vectorisrs/o-optimize vectorisrs/o-debug
noinst_DATA += vectorisrs/$(ARCH)/vectorisrs.rel

## --

all-local: $(PREINSTALL_FILES)

all-local: $(PREINSTALL_FILES)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/mongoose-v.h: include/mongoose-v.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/mongoose-v.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/mongoose-v.h

$(PROJECT_INCLUDE)/libchip/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libchip
	@: > $(PROJECT_INCLUDE)/libchip/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/$(dirstamp)

$(PROJECT_INCLUDE)/libchip/mg5uart.h: duart/mg5uart.h $(PROJECT_INCLUDE)/libchip/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libchip/mg5uart.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libchip/mg5uart.h

CLEANFILES = $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
