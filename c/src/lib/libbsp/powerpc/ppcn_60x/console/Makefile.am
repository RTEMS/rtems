##
## $Id$
##


PGM = $(ARCH)/console.rel

C_FILES = console.c i8042vga.c i8042.c ns16550cfg.c z85c30cfg.c vga.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

all-local: $(ARCH) $(OBJS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = config.c console.c console.h debugio.c i8042.c i8042_p.h \
    i8042vga.c i8042vga.h ns16550cfg.c ns16550cfg.h vga.c vga_p.h \
    z85c30cfg.c z85c30cfg.h

include $(top_srcdir)/../../../../automake/local.am
