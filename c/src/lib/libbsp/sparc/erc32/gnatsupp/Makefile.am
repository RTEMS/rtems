##
## $Id$
##

PGM = $(ARCH)/gnatsupp.rel

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

C_FILES = gnatsupp.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

sparc_shared_C_FILES = $(top_srcdir)/../../sparc/shared/gnatcommon.c
OBJS += \
    $(sparc_shared_C_FILES:$(top_srcdir)/../../sparc/shared/%.c=$(ARCH)/%.$(OBJEXT))

$(ARCH)/%.$(OBJEXT): $(top_srcdir)/../../sparc/shared/%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<

$(PGM): $(OBJS)
	$(make-rel)

noinst_DATA = $(PGM)

EXTRA_DIST = $(C_FILES)

include $(top_srcdir)/../../../../automake/local.am
