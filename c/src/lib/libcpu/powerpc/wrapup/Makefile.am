##
## $Id$
##


GENERIC_FILES = shared

# Use two variables so that I can make two ar command.
# So far FAMILY_OBJS is empty and ar dislike it...
CPU_SPECIFIC_OBJS = $(wildcard ../$(RTEMS_CPU_MODEL)/*/$(ARCH)/*.o)
FAMILY_OBJS = \
    $(wildcard ../shared/$(ARCH)/*.o ../shared/*/$(ARCH)/*.o \
	../old_exception_processing/*/$(ARCH)/*.o \
	../new_exception_processing/*/$(ARCH)/*.o \
	../mpc6xx/*/$(ARCH)/*.o)

LIB = $(ARCH)/libcpu.a

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/compile.am
include $(top_srcdir)/../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

libcpu_a_OBJECTS = $(CPU_SPECIFIC_OBJS) $(FAMILY_OBJS)

$(LIB): $(libcpu_a_OBJECTS)
#	${make-library}

#$(PROJECT_RELEASE)/lib/libcpu$(LIB_VARIANT).a: $(LIB)
#	$(INSTALL_DATA) $< $@
#
#TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/libcpu$(LIB_VARIANT).a

all-local: ${ARCH} $(LIB) $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../automake/local.am
