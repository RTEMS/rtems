##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../aclocal

include_HEADERS = include/tmacros.h include/buffer_test_io.h

all-local: $(PREINSTALL_FILES)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)

$(PROJECT_INCLUDE)/tmacros.h: include/tmacros.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/tmacros.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/tmacros.h

$(PROJECT_INCLUDE)/buffer_test_io.h: include/buffer_test_io.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/buffer_test_io.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/buffer_test_io.h

include $(top_srcdir)/../automake/local.am
