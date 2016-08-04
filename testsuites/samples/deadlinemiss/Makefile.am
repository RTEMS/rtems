
rtems_tests_PROGRAMS = deadlinemiss
deadlinemiss_SOURCES = system.h \
	init.c \
	tasks.c	

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


LINK_OBJS = $(deadlinemiss_OBJECTS)
LINK_LIBS = $(deadlinemiss_LDLIBS) -lm

deadlinemiss$(EXEEXT): $(deadlinemiss_OBJECTS) $(deadlinemiss_DEPENDENCIES)
	@rm -f deadlinemiss$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
