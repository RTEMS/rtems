## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

LIBNAME = lib.a
LIB = $(ARCH)/$(LIBNAME)

C_FILES = asp.c balloc.c wbase64.c default.c ejlex.c ejparse.c form.c h.c \
    handler.c mime.c misc.c webpage.c ringq.c rom.c security.c socket.c \
    sym.c uemf.c url.c value.c webcomp.c webrom.c webs.c websuemf.c \
    webmain.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/lib.am

#
# Add local stuff here using +=
#

AM_CPPFLAGS += -DWEBS -DUEMF -DOS="RTEMS"

$(LIB): $(OBJS)
	$(make-library)

if HAS_POSIX
all-local: $(ARCH) $(OBJS) $(LIB)
endif

.PRECIOUS: $(LIB)

EXTRA_DIST = asp.c balloc.c default.c ej.h ejlex.c ejparse.c form.c h.c \
    handler.c mime.c misc.c ringq.c rom.c rtems_webserver.h security.c \
    socket.c sym.c uemf.c uemf.h url.c value.c wbase64.c webcomp.c webmain.c \
    webpage.c webrom.c webs.c webs.h websuemf.c wsIntrn.h

include $(top_srcdir)/../../../automake/local.am
