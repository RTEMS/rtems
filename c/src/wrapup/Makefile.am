##
##  $Id$
##
##  build and install "glommed" librtemsall.a
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

LIB = $(PROJECT_RELEASE)/lib/librtemsall${LIB_VARIANT}.a

# HACK: We need to evaluate CFLAGS at make-time, because these
#	fscking <bsp>.cfgs change CFLAGS and can change 
#	MULTISUBDIR on-the-fly - A true PITA :(
if MULTILIB
MULTISUBDIR = /$(shell $(CC) --print-multi-directory $(CFLAGS))
PROJECT_CPUdir = $(PROJECT_ROOT)/lib$(MULTISUBDIR)
else
PROJECT_CPUdir = $(PROJECT_RELEASE)/lib
endif

if HAS_NETWORKING
LIBNETWORKING = $(PROJECT_RELEASE)/lib/libnetworking$(LIB_VARIANT).a \
    $(wildcard $(PROJECT_RELEASE)/lib/librpc$(LIB_VARIANT).a) \
    $(wildcard $(PROJECT_RELEASE)/lib/libxdr$(LIB_VARIANT).a)
endif

if HAS_RDBG
LIBRDBG = $(PROJECT_RELEASE)/lib/librdbg$(LIB_VARIANT).a
endif

if HAS_POSIX
LIBPOSIX = $(PROJECT_CPUdir)/libposix$(LIB_VARIANT).a
endif

if HAS_ITRON
LIBITRON = $(PROJECT_CPUdir)/libitron$(LIB_VARIANT).a
endif

SRCS =  $(PROJECT_RELEASE)/lib/libbsp$(LIB_VARIANT).a
SRCS += $(PROJECT_CPUdir)/librtems$(LIB_VARIANT).a
SRCS += $(LIBNETWORKING) $(LIBRDBG) $(LIBPOSIX) $(LIBITRON) \
    $(wildcard $(PROJECT_RELEASE)/lib/libcpu$(LIB_VARIANT).a) \
    $(wildcard $(PROJECT_RELEASE)/lib/librtcio$(LIB_VARIANT).a) \
    $(wildcard $(PROJECT_RELEASE)/lib/libserialio$(LIB_VARIANT).a) \
    $(wildcard $(PROJECT_RELEASE)/lib/libnetchip$(LIB_VARIANT).a) \
    $(PROJECT_RELEASE)/lib/libcsupport$(LIB_VARIANT).a \
    $(PROJECT_RELEASE)/lib/libmisc$(LIB_VARIANT).a \
    $(PROJECT_RELEASE)/lib/libfs$(LIB_VARIANT).a \
    $(wildcard $(PROJECT_RELEASE)/lib/rtems-ctor$(LIB_VARIANT).o)

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/librtemsall${LIB_VARIANT}.a

all-local: $(ARCH) $(TMPINSTALL_FILES)

$(LIB): $(SRCS)
	rm -f $@
	rm -rf $(ARCH)/*
	for f in $(SRCS); do \
	  case $$f in \
	  *.o | *.rel) \
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
	rm -f $(ARCH)/*
	$(RANLIB) $@

include $(top_srcdir)/../../../automake/local.am
