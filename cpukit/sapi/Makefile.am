##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

include_HEADERS = include/confdefs.h

include_rtemsdir = $(includedir)/rtems

include_rtems_HEADERS = include/rtems/config.h include/rtems/extension.h \
    include/rtems/fatal.h include/rtems/init.h include/rtems/io.h \
    include/rtems/mptables.h include/rtems/sptables.h

EXTRA_DIST = include/rtems/README

if INLINE
include_rtems_HEADERS += inline/rtems/extension.inl
else
include_rtems_HEADERS += macros/rtems/extension.inl
endif

## src

LIB=$(ARCH)/libsapi.a

EXTENSION_FILES = src/extension.c src/extensioncreate.c \
    src/extensiondelete.c src/extensionident.c
C_FILES = src/debug.c $(EXTENSION_FILES) src/fatal.c src/exinit.c src/io.c \
    src/itronapi.c src/posixapi.c src/rtemsapi.c
OBJS = $(C_FILES:src/%.c=${ARCH}/%.$(OBJEXT))

#
# Add local stuff here using +=
#

AM_CPPFLAGS += -I$(top_builddir)
AM_CPPFLAGS += -D__RTEMS_INSIDE__

all-local: $(PREINSTALL_FILES) $(LIB)

$(LIB): ${OBJS}
	$(make-library)

${ARCH}/%.$(OBJEXT): src/%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<

EXTRA_DIST += $(C_FILES)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)

$(PROJECT_INCLUDE)/confdefs.h: include/confdefs.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/confdefs.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/confdefs.h

$(PROJECT_INCLUDE)/rtems/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems
	@: > $(PROJECT_INCLUDE)/rtems/$(dirstamp)

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

include $(top_srcdir)/automake/local.am
