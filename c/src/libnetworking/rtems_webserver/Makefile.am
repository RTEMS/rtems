##
## $Id$
##

LIBNAME = lib.a
LIB = $(ARCH)/$(LIBNAME)

C_FILES = asp.c balloc.c default.c ejlex.c ejparse.c emfdb.c form.c h.c \
    handler.c md5c.c mime.c misc.c webpage.c ringq.c rom.c security.c sock.c \
    socket.c sym.c uemf.c um.c url.c value.c wbase64.c webrom.c webs.c \
    websuemf.c webmain.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

#
# Add local stuff here using +=
#

AM_CPPFLAGS += -DWEBS -DUEMF -DOS="RTEMS"

$(LIB): $(OBJS)
	$(make-library)

if HAS_NETWORKING
if HAS_POSIX
include_goaheaddir = $(includedir)/goahead

include_HEADERS = rtems_webserver.h
include_goahead_HEADERS = ej.h ejIntrn.h emfdb.h md5.h uemf.h um.h webs.h \
    wsIntrn.h

all-local: $(PREINSTALL_FILES) $(OBJS) $(LIB)
endif
endif

.PRECIOUS: $(LIB)

EXTRA_DIST = asp.c balloc.c default.c ej.h ejlex.c ejparse.c form.c h.c \
    handler.c mime.c misc.c ringq.c rom.c rtems_webserver.h security.c \
    sock.c socket.c sym.c uemf.c uemf.h um.h url.c value.c wbase64.c \
    webcomp.c webmain.c webpage.c webrom.c webs.c webs.h websuemf.c \
    wsIntrn.h

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/$(dirstamp)

if HAS_NETWORKING
if HAS_POSIX
$(PROJECT_INCLUDE)/goahead/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/goahead
	@: > $(PROJECT_INCLUDE)/goahead/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/goahead/$(dirstamp)

$(PROJECT_INCLUDE)/rtems_webserver.h: rtems_webserver.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems_webserver.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems_webserver.h

$(PROJECT_INCLUDE)/goahead/ej.h: ej.h $(PROJECT_INCLUDE)/goahead/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/goahead/ej.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/goahead/ej.h

$(PROJECT_INCLUDE)/goahead/ejIntrn.h: ejIntrn.h $(PROJECT_INCLUDE)/goahead/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/goahead/ejIntrn.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/goahead/ejIntrn.h

$(PROJECT_INCLUDE)/goahead/emfdb.h: emfdb.h $(PROJECT_INCLUDE)/goahead/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/goahead/emfdb.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/goahead/emfdb.h

$(PROJECT_INCLUDE)/goahead/md5.h: md5.h $(PROJECT_INCLUDE)/goahead/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/goahead/md5.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/goahead/md5.h

$(PROJECT_INCLUDE)/goahead/uemf.h: uemf.h $(PROJECT_INCLUDE)/goahead/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/goahead/uemf.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/goahead/uemf.h

$(PROJECT_INCLUDE)/goahead/um.h: um.h $(PROJECT_INCLUDE)/goahead/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/goahead/um.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/goahead/um.h

$(PROJECT_INCLUDE)/goahead/webs.h: webs.h $(PROJECT_INCLUDE)/goahead/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/goahead/webs.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/goahead/webs.h

$(PROJECT_INCLUDE)/goahead/wsIntrn.h: wsIntrn.h $(PROJECT_INCLUDE)/goahead/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/goahead/wsIntrn.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/goahead/wsIntrn.h
endif
endif

include $(top_srcdir)/automake/local.am
