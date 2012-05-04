##
## Copyright (C) 2010 OKTET Labs, St.-Petersburg, Russia
## Author: Oleg Kravtsov <Oleg.Kravtsov@oktetlabs.ru>
##
## The license and distribution terms for this file may be
## found in the file LICENSE in this distribution or at
## http://www.rtems.com/license/LICENSE.
##


rtems_tests_PROGRAMS = block08
block08_SOURCES = init.c               \
                       system.h        \
                       bdbuf_tests.c   \
	               bdbuf_tests.h   \
		       test_disk.c     \
		       bdbuf_test1_1.c \
                       bdbuf_test1_2.c \
                       bdbuf_test1_3.c \
                       bdbuf_test1_4.c \
                       bdbuf_test1_5.c \
                       bdbuf_test2_1.c \
                       bdbuf_test2_2.c \
                       bdbuf_test3_1.c \
                       bdbuf_test3_2.c \
                       bdbuf_test3_3.c \
                       bdbuf_test4_1.c \
                       bdbuf_test4_2.c \
                       bdbuf_test4_3.c

dist_rtems_tests_DATA = block08.scn block08.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(block08_OBJECTS)
LINK_LIBS = $(block08_LDLIBS)

block08$(EXEEXT): $(block08_OBJECTS) $(block08_DEPENDENCIES)
	@rm -f block08$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
