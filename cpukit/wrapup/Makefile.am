##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

project_lib_DATA = $(ARCH)/librtemscpu$(LIB_VARIANT).a

LIBS =
if LIBSCORECPU
LIBS += ../score/cpu/$(RTEMS_CPU)/$(ARCH)/libscorecpu.a
endif
LIBS += ../score/libscore$(LIB_VARIANT).a
LIBS += ../sapi/libsapi$(LIB_VARIANT).a
LIBS += ../rtems/librtems$(LIB_VARIANT).a

if HAS_POSIX
LIBS += ../posix/libposix$(LIB_VARIANT).a
endif

if HAS_ITRON
LIBS += ../itron/libitron$(LIB_VARIANT).a
endif

LIBS += ../libcsupport/libcsupport$(LIB_VARIANT).a

LIBS += ../libblock/libblock$(LIB_VARIANT).a
if !UNIX
LIBS += ../libfs/libdosfs$(LIB_VARIANT).a
endif
LIBS += ../libfs/libimfs$(LIB_VARIANT).a

LIBS += ../libmisc/libmonitor$(LIB_VARIANT).a
LIBS += ../libmisc/libuntar$(LIB_VARIANT).a
LIBS += ../libmisc/libstackchk$(LIB_VARIANT).a
LIBS += ../libmisc/libcpuuse$(LIB_VARIANT).a
LIBS += ../libmisc/librtmonuse$(LIB_VARIANT).a

## XXX temporarily removed because it causes a
## XXX number of BSPs to not link "main(){}" used by autoconf
# if LIBSERDBG
# LIBS += ../libmisc/libserdbg$(LIB_VARIANT).a
# endif

if LIBSHELL
LIBS += ../libmisc/libshell$(LIB_VARIANT).a
endif

LIBS += ../libmisc/libdumpbuf$(LIB_VARIANT).a
LIBS += ../libmisc/libdevnull$(LIB_VARIANT).a
LIBS += ../libmisc/libdummy$(LIB_VARIANT).a
LIBS += ../libmisc/libmw-fb$(LIB_VARIANT).a
LIBS += ../libmisc/libcapture$(LIB_VARIANT).a
LIBS += ../libmisc/libfsmount$(LIB_VARIANT).a

if HAS_NETWORKING
LIBS += ../libnetworking/libnetworking$(LIB_VARIANT).a
LIBS += ../libnetworking/libc$(LIB_VARIANT).a
LIBS += ../libnetworking/lib$(LIB_VARIANT).a
endif

if LIBRPC
LIBS += ../librpc/librpc$(LIB_VARIANT).a
LIBS += ../librpc/libxdr$(LIB_VARIANT).a
endif

$(LIBS): $(ARCH)/$(dirstamp)

$(ARCH)/librtemscpu$(LIB_VARIANT).a: ${LIBS}
	rm -f $@
	rm -rf $(ARCH)/*.$(OBJEXT) $(ARCH)/*.rel
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
	$(AR) rc $@ $(ARCH)/*
	rm -f $(ARCH)/*.$(OBJEXT) $(ARCH)/*.rel
	$(RANLIB) $@

all-local: $(TMPINSTALL_FILES)

$(PROJECT_LIB)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)

$(PROJECT_LIB)/librtemscpu$(LIB_VARIANT).a: $(ARCH)/librtemscpu$(LIB_VARIANT).a $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/librtemscpu$(LIB_VARIANT).a
TMPINSTALL_FILES += $(PROJECT_LIB)/librtemscpu$(LIB_VARIANT).a

include $(top_srcdir)/automake/local.am
