##
##  $Id$
##
##  build and install "glommed" librtemsbsp.a
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

LIB = $(PROJECT_RELEASE)/lib/librtemsbsp${LIB_VARIANT}.a

if HAS_NETWORKING
LIBNETWORKING = ../libnetworking/wrapup/$(ARCH)/libnetapps.a
endif

if HAS_RDBG
LIBRDBG = $(PROJECT_RELEASE)/lib/librdbg$(LIB_VARIANT).a
endif

if HAS_LIBBSP
LIBBSP = ../lib/libbsp/@RTEMS_CPU@/$(RTEMS_BSP_FAMILY)/wrapup/$(ARCH)/libbsp.a
endif

SRCS = $(LIBBSP) $(LIBNETWORKING) $(LIBRDBG) \
    $(wildcard $(PROJECT_RELEASE)/lib/libcpu$(LIB_VARIANT).a) \
    $(wildcard $(PROJECT_RELEASE)/lib/librtcio$(LIB_VARIANT).a) \
    $(wildcard $(PROJECT_RELEASE)/lib/libserialio$(LIB_VARIANT).a) \
    $(wildcard $(PROJECT_RELEASE)/lib/libnetchip$(LIB_VARIANT).a) \
    ../libmisc/wrapup/$(ARCH)/libmisc.a \
    $(wildcard $(PROJECT_RELEASE)/lib/rtems-ctor$(LIB_VARIANT).o)

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/librtemsbsp${LIB_VARIANT}.a

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
