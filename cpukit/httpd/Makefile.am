##
## $Id$
##

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

EXTRA_LIBRARIES =
CLEANFILES =

AM_CPPFLAGS += -DWEBS -DUEMF -DOS="RTEMS"

if HAS_NETWORKING
if HAS_POSIX
include_goaheaddir = $(includedir)/goahead

include_HEADERS = rtems_webserver.h
include_goahead_HEADERS = ej.h ejIntrn.h emfdb.h md5.h uemf.h um.h webs.h \
    wsIntrn.h

EXTRA_LIBRARIES += libhttpd.a
CLEANFILES += libhttpd.a
libhttpd_a_SOURCES = asp.c balloc.c default.c ejlex.c ejparse.c emfdb.c \
    form.c h.c handler.c md5c.c mime.c misc.c webpage.c ringq.c rom.c \
    security.c sock.c socket.c sym.c uemf.c um.c url.c value.c wbase64.c \
    webrom.c webs.c websuemf.c webmain.c
libhttpd_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += libhttpd_g.a
CLEANFILES += libhttpd_g.a
libhttpd_g_a_SOURCES = $(libhttpd_a_SOURCES)
libhttpd_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

noinst_DATA = libhttpd$(LIB_VARIANT).a
endif
endif

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = webcomp.c webpage.c

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/$(dirstamp)

if HAS_NETWORKING
if HAS_POSIX
$(PROJECT_INCLUDE)/goahead/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/goahead
	@: > $(PROJECT_INCLUDE)/goahead/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/goahead/$(dirstamp)

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

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/automake/local.am
