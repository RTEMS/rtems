##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

include_rtemsdir = $(includedir)/rtems

include_rtems_HEADERS = include/confdefs.h
include_rtems_HEADERS += include/rtems/config.h include/rtems/extension.h \
    include/rtems/fatal.h include/rtems/init.h include/rtems/io.h \
    include/rtems/mptables.h include/rtems/sptables.h

EXTRA_DIST = include/rtems/README

if INLINE
include_rtems_HEADERS += inline/rtems/extension.inl
else
include_rtems_HEADERS += macros/rtems/extension.inl
endif

## src
AM_CPPFLAGS += -D__RTEMS_INSIDE__

noinst_LIBRARIES = libsapi.a
libsapi_a_SOURCES = src/debug.c src/extension.c src/extensioncreate.c \
    src/extensiondelete.c src/extensionident.c src/fatal.c src/exinit.c \
    src/io.c src/itronapi.c src/posixapi.c src/rtemsapi.c
libsapi_a_CPPFLAGS = $(AM_CPPFLAGS)

all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/rtems/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems
	@: > $(PROJECT_INCLUDE)/rtems/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/confdefs.h: include/confdefs.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/confdefs.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/confdefs.h

$(PROJECT_INCLUDE)/rtems/config.h: include/rtems/config.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/config.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/config.h

$(PROJECT_INCLUDE)/rtems/extension.h: include/rtems/extension.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/extension.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/extension.h

$(PROJECT_INCLUDE)/rtems/fatal.h: include/rtems/fatal.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/fatal.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/fatal.h

$(PROJECT_INCLUDE)/rtems/init.h: include/rtems/init.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/init.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/init.h

$(PROJECT_INCLUDE)/rtems/io.h: include/rtems/io.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/io.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/io.h

$(PROJECT_INCLUDE)/rtems/mptables.h: include/rtems/mptables.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/mptables.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/mptables.h

$(PROJECT_INCLUDE)/rtems/sptables.h: include/rtems/sptables.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/sptables.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/sptables.h

if INLINE
$(PROJECT_INCLUDE)/rtems/extension.inl: inline/rtems/extension.inl $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/extension.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/extension.inl

else
$(PROJECT_INCLUDE)/rtems/extension.inl: macros/rtems/extension.inl $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/extension.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/extension.inl
endif

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/automake/local.am
