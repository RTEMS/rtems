##
## Kate Feng
##

VPATH = @srcdir@:@srcdir@/../../shared/console

include_HEADERS = bspopts.h
include_HEADERS += bsp.h 

H_FILES = bsp.h 

include_bspdir = $(includedir)/bsp 

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bspopts.h

all-local: $(PREINSTALL_FILES) $(ARCH) $(OBJS)

include $(top_srcdir)/../../../../automake/local.am
