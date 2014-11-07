rtems_tests_PROGRAMS = i2c01
i2c01_SOURCES = init.c

dist_rtems_tests_DATA = i2c01.scn i2c01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(i2c01_OBJECTS)
LINK_LIBS = $(i2c01_LDLIBS)

i2c01$(EXEEXT): $(i2c01_OBJECTS) $(i2c01_DEPENDENCIES)
	@rm -f i2c01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
