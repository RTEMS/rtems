##
##  $Id$
##


VPATH = @srcdir@:@srcdir@/../../../shared

PGM = ${ARCH}/startup.rel

if HAS_CXX
C_MAIN_C_FILES =
else
C_MAIN_C_FILES = main.c
endif

# C sources
C_FILES = bspclean.c bsplibc.c bspstart.c setvec.c $(C_MAIN_C_FILES)
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.$(OBJEXT))
startup_rel_OBJECTS = $(C_O_FILES)

# CC sources
CC_FILES = rtems-ctor.cc
CC_O_FILES = $(CC_FILES:%.cc=${ARCH}/%.$(OBJEXT)}

# We install the RTEMS constructor as a separate .$(OBJEXT)
# so it can be easily place correctly by the compiler config file.

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CFLAGS += $(LIBC_DEFINES)

$(PGM): $(startup_rel_OBJECTS)
	$(make-rel)

${ARCH}/libno-ctor.a: $(ARCH)/no-ctor.$(OBJEXT)
	$(AR) -cvr $@ $<

$(PROJECT_RELEASE)/lib/libno-ctor$(LIB_VARIANT).a: ${ARCH}/libno-ctor.a
	$(INSTALL_DATA) $< $@

$(PROJECT_RELEASE)/lib/rtems-ctor$(LIB_VARIANT).$(OBJEXT): $(ARCH)/rtems-ctor.$(OBJEXT)
	$(INSTALL_DATA) $< $@

if HAS_CXX
all-local: $(ARCH) $(PGM) $(PROJECT_RELEASE)/lib/libno-ctor$(LIB_VARIANT).a \
    $(PROJECT_RELEASE)/lib/rtems-ctor$(LIB_VARIANT).$(OBJEXT)
else
all-local: $(ARCH) $(PGM)
endif

EXTRA_DIST = bspclean.c bspstart.c exit.c main.c no-ctor.c rtems-ctor.cc \
    setvec.c

include $(top_srcdir)/../../../../automake/local.am
