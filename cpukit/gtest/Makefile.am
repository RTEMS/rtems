##
## $Id: $
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

if HAS_CXX
CPPFLAGS += -I $(srcdir)

include_rtemsdir = $(includedir)/rtems
include_gtestdir = $(includedir)/gtest
include_gtest_internaldir = $(include_gtestdir)/internal

include_rtems_HEADERS = \
	include/gtest/gtest-main.h

include_gtest_HEADERS = \
	include/gtest/gtest.h \
	include/gtest/gtest-death-test.h \
	include/gtest/gtest-message.h \
	include/gtest/gtest-param-test.h \
	include/gtest/gtest_pred_impl.h \
	include/gtest/gtest-printers.h \
	include/gtest/gtest_prod.h \
	include/gtest/gtest-spi.h \
	include/gtest/gtest-test-part.h \
	include/gtest/gtest-typed-test.h

include_gtest_internal_HEADERS = \
	include/gtest/internal/gtest-death-test-internal.h \
	include/gtest/internal/gtest-filepath.h \
	include/gtest/internal/gtest-internal.h \
	include/gtest/internal/gtest-linked_ptr.h \
	include/gtest/internal/gtest-param-util-generated.h \
	include/gtest/internal/gtest-param-util.h \
	include/gtest/internal/gtest-port.h \
	include/gtest/internal/gtest-string.h \
	include/gtest/internal/gtest-tuple.h \
	include/gtest/internal/gtest-type-util.h

project_lib_LIBRARIES = \
	libgtest.a

libgtest_a_SOURCES = \
	src/gtest.cc \
	src/gtest-death-test.cc \
	src/gtest-filepath.cc \
	src/gtest-port.cc \
	src/gtest-printers.cc \
	src/gtest-test-part.cc \
	src/gtest-typed-test.cc
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
