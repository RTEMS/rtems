##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/multilib.am
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

AM_CPPFLAGS += -DCPU_SYNC_IO $(LIBC_DEFINES)

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/score:
	$(mkinstalldirs) $@

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = \
    rtems/score/types.h \
    rtems/score/cpu.h \
    rtems/score/unix.h \
    rtems/score/unixsize.h
PREINSTALL_FILES = $(PROJECT_INCLUDE)/rtems/score \
    $(include_rtems_score_HEADERS:%.h=$(PROJECT_INCLUDE)/%.h)

C_FILES = cpu.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

LIB = $(ARCH)/libscorecpu.a

$(LIB): $(OBJS)
	$(make-library)

all-local: $(ARCH) $(LIB)

EXTRA_DIST = cpu.c

include $(top_srcdir)/../../../automake/local.am
