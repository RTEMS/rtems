rtems_tests_PROGRAMS = spresource01
spresource01_SOURCES = init.c

dist_rtems_tests_DATA = spresource01.scn spresource01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spresource01_OBJECTS)
LINK_LIBS = $(spresource01_LDLIBS)

spresource01$(EXEEXT): $(spresource01_OBJECTS) $(spresource01_DEPENDENCIES)
	@rm -f spresource01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
