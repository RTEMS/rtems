##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

$(PROJECT_INCLUDE):
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: include/%.h
	$(INSTALL_DATA) $< $@
$(PROJECT_INCLUDE)/%.inl: $(INLINEdir)/%.inl
	$(INSTALL_DATA) $< $@

include_HEADERS = include/confdefs.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)
PREINSTALL_FILES += $(include_HEADERS:include/%=$(PROJECT_INCLUDE)/%)

include_rtemsdir = $(includedir)/rtems
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems

include_rtems_HEADERS = include/rtems/config.h include/rtems/extension.h \
    include/rtems/fatal.h \
    include/rtems/init.h include/rtems/io.h include/rtems/mptables.h \
    include/rtems/sptables.h

PREINSTALL_FILES += $(include_rtems_HEADERS:include/%=$(PROJECT_INCLUDE)/%)

EXTRA_DIST = include/rtems/README

include/rtems/sptables.h: $(top_builddir)/config.status $(top_srcdir)/sapi/include/rtems/sptables.h.in
	cd $(top_builddir) && CONFIG_FILES=sapi/$@ CONFIG_HEADERS= CONFIG_LINKS= $(SHELL) ./config.status

## inline

inline_H_FILES = inline/rtems/extension.inl
noinst_HEADERS = $(inline_H_FILES)

if INLINE
PREINSTALL_FILES += $(inline_H_FILES:inline/%=$(PROJECT_INCLUDE)/%)
include_rtems_HEADERS += $(inline_H_FILES)
endif

## macros
macros_H_FILES = macros/rtems/extension.inl
noinst_HEADERS += $(macros_H_FILES)

if MACROS
PREINSTALL_FILES += $(macros_H_FILES:macros/%=$(PROJECT_INCLUDE)/%)
include_rtems_HEADERS += $(macros_H_FILES)
endif

## src

LIB=$(ARCH)/libsapi.a

EXTENSION_FILES = src/extension.c src/extensioncreate.c src/extensiondelete.c \
    src/extensionident.c
C_FILES = src/debug.c $(EXTENSION_FILES) src/fatal.c src/exinit.c src/io.c \
    src/itronapi.c src/posixapi.c src/rtemsapi.c
OBJS = $(C_FILES:src/%.c=${ARCH}/%.$(OBJEXT))

#
# Add local stuff here using +=
#

AM_CPPFLAGS += -D__RTEMS_INSIDE__

all-local: $(PREINSTALL_FILES) ${ARCH} $(LIB)

$(LIB): ${OBJS}
	$(make-library)

${ARCH}/%.$(OBJEXT): src/%.c
	${COMPILE} -o $@ -c $<

EXTRA_DIST += $(C_FILES)

include $(top_srcdir)/automake/local.am
