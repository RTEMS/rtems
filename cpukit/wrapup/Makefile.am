##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

## Setup the variant build subdirectory
ARCH = o-optimize

project_lib_DATA = librtemscpu.a
CLEANFILES = librtemscpu.a

TMP_LIBS =
if LIBSCORECPU
TMP_LIBS += ../score/cpu/@RTEMS_CPU@/libscorecpu.a
endif
TMP_LIBS += ../score/libscore.a
TMP_LIBS += ../sapi/libsapi.a
TMP_LIBS += ../rtems/librtems.a

if HAS_POSIX
TMP_LIBS += ../posix/libposix.a
endif

if HAS_ITRON
TMP_LIBS += ../itron/libitron.a
endif

TMP_LIBS += ../libcsupport/libcsupport.a
if !UNIX
TMP_LIBS += ../libblock/libblock.a
TMP_LIBS += ../libfs/libdosfs.a
endif
TMP_LIBS += ../libfs/libimfs.a

TMP_LIBS += ../libmisc/libmonitor.a
TMP_LIBS += ../libmisc/libuntar.a
TMP_LIBS += ../libmisc/libstackchk.a
TMP_LIBS += ../libmisc/libcpuuse.a
TMP_LIBS += ../libmisc/librtmonuse.a

## XXX temporarily removed because it causes a
## XXX number of BSPs to not link "main(){}" used by autoconf
# if LIBSERDBG
# TMP_LIBS += ../libmisc/libserdbg.a
# endif

if LIBSHELL
TMP_LIBS += ../libmisc/libshell.a
endif

TMP_LIBS += ../libmisc/libdumpbuf.a
TMP_LIBS += ../libmisc/libdevnull.a
TMP_LIBS += ../libmisc/libdummy.a
TMP_LIBS += ../libmisc/libmw-fb.a
TMP_LIBS += ../libmisc/libcapture.a
TMP_LIBS += ../libmisc/libfsmount.a

if HAS_NETWORKING
TMP_LIBS += ../libnetworking/libnetworking.a
TMP_LIBS += ../libnetworking/libc.a
TMP_LIBS += ../libnetworking/lib.a
endif

if LIBRPC
TMP_LIBS += ../librpc/librpc.a
TMP_LIBS += ../librpc/libxdr.a
endif

librtemscpu.a: $(TMP_LIBS)
	rm -f $@
	$(mkdir_p) $(ARCH)
	rm -rf $(ARCH)/*.$(OBJEXT) $(ARCH)/*.rel
	for f in $(TMP_LIBS); do \
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

TMPINSTALL_FILES =

$(PROJECT_LIB)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)
TMPINSTALL_FILES += $(PROJECT_LIB)/$(dirstamp)

$(PROJECT_LIB)/librtemscpu.a: librtemscpu.a $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/librtemscpu.a
TMPINSTALL_FILES += $(PROJECT_LIB)/librtemscpu.a

CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/automake/local.am
