#
#  $Id$
#
#  build and install "glommed" librtemsall.a
#

AUTOMAKE_OPTIONS = foreign 1.4

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../automake/lib.am

LIBNAME = librtemsall
LIB = $(PROJECT_RELEASE)/lib/$(LIBNAME)${LIB_VARIANT}.a

SRCS = $(wildcard $(PROJECT_RELEASE)/lib/libbsp$(LIB_VARIANT).a) \
    $(PROJECT_RELEASE)/lib/librtems$(LIB_VARIANT).a \
    $(wildcard $(PROJECT_RELEASE)/lib/libposix$(LIB_VARIANT).a) \
    $(wildcard $(PROJECT_RELEASE)/lib/libitron$(LIB_VARIANT).a) \
    $(wildcard $(PROJECT_RELEASE)/lib/libnetworking$(LIB_VARIANT).a) \
    $(wildcard $(PROJECT_RELEASE)/lib/librpc$(LIB_VARIANT).a) \
    $(wildcard $(PROJECT_RELEASE)/lib/librdbg$(LIB_VARIANT).a) \
    $(wildcard $(PROJECT_RELEASE)/lib/libcpu$(LIB_VARIANT).a) \
    $(wildcard $(PROJECT_RELEASE)/lib/librtcio$(LIB_VARIANT).a) \
    $(wildcard $(PROJECT_RELEASE)/lib/libserialio$(LIB_VARIANT).a) \
    $(wildcard $(PROJECT_RELEASE)/lib/libnetchip$(LIB_VARIANT).a) \
    $(PROJECT_RELEASE)/lib/libcsupport$(LIB_VARIANT).a \
    $(PROJECT_RELEASE)/lib/libmisc$(LIB_VARIANT).a \
    $(wildcard $(PROJECT_RELEASE)/lib/rtems-ctor$(LIB_VARIANT).o)

CLEANFILES += $(ARCH)/check

TMPINSTALL_FILES += \
$(PROJECT_RELEASE)/lib/$(LIBNAME)${LIB_VARIANT}.a

all: $(ARCH) $(TMPINSTALL_FILES)

install: all

$(ARCH)/check: $(SRCS)
	@$(RM) $@; touch $@;
	@for f in $(SRCS); do \
	  case $$f in \
	  *.o)   echo " `basename $$f`" >> $@ \
	    ;; \
	  *.rel) echo " `basename $$f`" >> $@ \
	    ;; \
	  *.a) \
	  ( list=`$(AR) t $$f`;\
	    for i in $$list; do \
	      if fgrep " $$i" $@; then \
	        echo "ERROR -- $$i in multiple files"; exit 1; \
	      fi;\
	      echo " $$i" >> $@;\
	    done; ) \
	    ;; \
	  esac; \
	done;

$(LIB): $(ARCH)/check
	@for f in $(SRCS); do \
	  case $$f in \
	  *.o) $(AR) ru $@ $$f \
	    ;;\
	  *.rel) $(AR) ru $@ $$f \
	    ;;\
	  *.a) \
	    (cd $(ARCH); \
	    list=`$(AR) t ../$$f`; \
	    $(AR) x ../$$f $$list; $(AR) ru ../$@ $$list; \
	    $(RM) $$list ;)\
	    ;; \
	  esac; \
	done;
	@$(RANLIB) $@
	@echo "*** Glommed $@"

include $(top_srcdir)/../../automake/local.am
