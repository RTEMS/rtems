##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/multilib.am
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

AM_CPPFLAGS += -DCPU_SYNC_IO $(LIBC_DEFINES)

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = rtems/score/types.h rtems/score/cpu.h \
    rtems/score/unix.h rtems/score/unixsize.h

C_FILES = cpu.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

LIB = $(ARCH)/libscorecpu.a

$(LIB): $(OBJS)
	$(make-library)

all-local: $(ARCH) $(PREINSTALL_FILES) $(LIB) \
	$(TMPINSTALL_FILES)

EXTRA_DIST = cpu.c

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/rtems/score/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/rtems/score
	@: > $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/score/types.h: rtems/score/types.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/types.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/types.h

$(PROJECT_INCLUDE)/rtems/score/cpu.h: rtems/score/cpu.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/cpu.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/cpu.h

$(PROJECT_INCLUDE)/rtems/score/unix.h: rtems/score/unix.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/unix.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/unix.h

$(PROJECT_INCLUDE)/rtems/score/unixsize.h: rtems/score/unixsize.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/unixsize.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/unixsize.h

include $(top_srcdir)/../../../automake/local.am
