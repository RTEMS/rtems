##
## $Id$
##

LIB = $(project_libdir)/librtemscpu$(LIB_VARIANT).a

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

LIBS =
if LIBSCORECPU
LIBS += ../score/cpu/$(RTEMS_CPU)/$(ARCH)/libscorecpu.a
endif
LIBS += ../score/$(ARCH)/libscore.a
LIBS += ../sapi/$(ARCH)/libsapi.a
LIBS += ../rtems/$(ARCH)/librtems.a

if HAS_POSIX
LIBS += ../posix/$(ARCH)/libposix.a
endif

if HAS_ITRON
LIBS += ../itron/$(ARCH)/libitron.a
endif

LIBS += ../libcsupport/$(ARCH)/libcsupport.a

LIBS += ../libblock/$(ARCH)/libblock.a
if !UNIX
LIBS += ../libfs/src/dosfs/$(ARCH)/libdosfs.a
endif
LIBS += ../libfs/src/imfs/$(ARCH)/libimfs.a

LIBS += ../libmisc/wrapup/$(ARCH)/libmisc.a

if HAS_NETWORKING
LIBS += ../libnetworking/wrapup/$(ARCH)/libnetworking.a
endif

if LIBRPC
LIBS += ../librpc/src/rpc/$(ARCH)/librpc.a
LIBS += ../librpc/src/xdr/$(ARCH)/libxdr.a
endif

$(LIB): ${LIBS}
	rm -f $@
	rm -rf $(ARCH)/*
	for f in $(LIBS); do \
	  case $$f in \
	  *.$(OBJEXT) | *.rel) \
	    if test -f $(ARCH)/`basename $$f`; then \
	      if cmp $$f $(ARCH)/`basename $$f`; then \
		true; \
	      else \
		echo 1>&2 "ERROR -- `basename $$f` in multiple files"; \
		exit 1; \
	      fi; \
	    else \
	      cp $$f $(ARCH)/; \
	      chmod a-w $(ARCH)/`basename $$f`; \
	    fi; \
	    ;; \
	  *.a) \
	    cd $(ARCH); \
	      $(AR) xv ../$$f || exit 1; \
	      chmod a-w * ; \
            cd ..; \
	    ;; \
	  esac; \
	done
	test -d $(project_libdir) || $(mkinstalldirs) $(project_libdir)
	$(AR) rc $@ $(ARCH)/*
	rm -f $(ARCH)/*.$(OBJEXT)
	$(RANLIB) $@

TMPINSTALL_FILES += $(project_libdir)/librtemscpu$(LIB_VARIANT).a

all-am: $(ARCH) $(LIB) $(TMPINSTALL_FILES)

install-data-hook: $(LIB)
	@$(mkinstalldirs) $(DESTDIR)/$(libdir)
	$(INSTALL_DATA) $(LIB) $(DESTDIR)/$(libdir)

include $(top_srcdir)/automake/local.am
