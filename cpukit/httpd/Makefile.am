##
## $Id$
##

LIBNAME = lib.a
LIB = $(ARCH)/$(LIBNAME)

C_FILES = asp.c balloc.c default.c ejlex.c ejparse.c emfdb.c form.c \
    h.c handler.c md5c.c mime.c misc.c webpage.c ringq.c rom.c security.c \
    sock.c socket.c sym.c uemf.c um.c url.c value.c wbase64.c webrom.c webs.c \
    websuemf.c webmain.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

H_FILES = ej.h ejIntrn.h emfdb.h md5.h uemf.h um.h webs.h wsIntrn.h

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

#
# Add local stuff here using +=
#

AM_CPPFLAGS += -DWEBS -DUEMF -DOS="RTEMS"

$(LIB): $(OBJS)
	$(make-library)

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/goahead:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/goahead/%.h: %.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/rtems_webserver.h: rtems_webserver.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE) $(PROJECT_INCLUDE)/rtems_webserver.h \
    $(PROJECT_INCLUDE)/goahead $(H_FILES:%.h=$(PROJECT_INCLUDE)/goahead/%.h)

if HAS_NETWORKING
if HAS_POSIX
include_goaheaddir = $(includedir)/goahead

include_HEADERS = rtems_webserver.h
include_goahead_HEADERS = $(H_FILES)

all-local: $(PREINSTALL_FILES) $(ARCH) $(OBJS) $(LIB)
endif
endif

.PRECIOUS: $(LIB)

EXTRA_DIST = asp.c balloc.c default.c ej.h ejlex.c ejparse.c form.c h.c \
    handler.c mime.c misc.c ringq.c rom.c rtems_webserver.h security.c \
    sock.c socket.c sym.c uemf.c uemf.h um.h url.c value.c wbase64.c webcomp.c \
    webmain.c webpage.c webrom.c webs.c webs.h websuemf.c wsIntrn.h

include $(top_srcdir)/automake/local.am
