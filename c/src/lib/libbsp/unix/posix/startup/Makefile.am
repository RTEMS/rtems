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

if HAS_CXX
project_lib_DATA = ${ARCH}/libno-ctor$(LIB_VARIANT).a

${ARCH}/libno-ctor$(LIB_VARIANT).a: $(ARCH)/no-ctor.$(OBJEXT)
	$(AR) -cvr $@ $<

${ARCH}/rtems-ctor$(LIB_VARIANT).$(OBJEXT): rtems-ctor.cc $(ARCH)/$(dirstamp)
	${CXXCOMPILE} -o $@ -c $<

project_lib_DATA += ${ARCH}/rtems-ctor$(LIB_VARIANT).$(OBJEXT)
endif

all-local: $(PGM) $(TMPINSTALL_FILES)

EXTRA_DIST = bspclean.c bspstart.c exit.c main.c no-ctor.c rtems-ctor.cc \
    setvec.c

${PROJECT_RELEASE}/lib/$(dirstamp):
	@$(mkdir_p) ${PROJECT_RELEASE}/lib
	@: > ${PROJECT_RELEASE}/lib/$(dirstamp)

if HAS_CXX
${PROJECT_RELEASE}/lib/libno-ctor$(LIB_VARIANT).a: ${ARCH}/libno-ctor$(LIB_VARIANT).a ${PROJECT_RELEASE}/lib/$(dirstamp)
	$(INSTALL_DATA) $< ${PROJECT_RELEASE}/lib/libno-ctor$(LIB_VARIANT).a
TMPINSTALL_FILES += ${PROJECT_RELEASE}/lib/libno-ctor$(LIB_VARIANT).a

${PROJECT_RELEASE}/lib/rtems-ctor$(LIB_VARIANT).$(OBJEXT): ${ARCH}/rtems-ctor$(LIB_VARIANT).$(OBJEXT) ${PROJECT_RELEASE}/lib/$(dirstamp)
	$(INSTALL_DATA) $< ${PROJECT_RELEASE}/lib/rtems-ctor$(LIB_VARIANT).$(OBJEXT)
TMPINSTALL_FILES += ${PROJECT_RELEASE}/lib/rtems-ctor$(LIB_VARIANT).$(OBJEXT)
endif

include $(top_srcdir)/../../../../automake/local.am
