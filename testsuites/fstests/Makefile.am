#
# File System Testsuite
#

ACLOCAL_AMFLAGS = -I ../aclocal

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am

fs_tests =
fs_screens =
fs_docs =

support_includes = -I$(top_srcdir)/../support/include
test_includes = -I$(top_srcdir)/support \
	-I$(top_srcdir)/../psxtests/include

if TEST_fsbdpart01
fs_tests += fsbdpart01
fs_screens += fsbdpart01/fsbdpart01.scn
fs_docs += fsbdpart01/fsbdpart01.doc
fsbdpart01_SOURCES = fsbdpart01/init.c
fsbdpart01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_fsbdpart01) \
	$(support_includes)
endif

if TEST_fsclose01
fs_tests += fsclose01
fs_screens += fsclose01/fsclose01.scn
fs_docs += fsclose01/fsclose01.doc
fsclose01_SOURCES = fsclose01/init.c
fsclose01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_fsclose01) \
	$(support_includes)
endif

if TEST_fsdosfsformat01
fs_tests += fsdosfsformat01
fs_screens += fsdosfsformat01/fsdosfsformat01.scn
fs_docs += fsdosfsformat01/fsdosfsformat01.doc
fsdosfsformat01_SOURCES = fsdosfsformat01/init.c
fsdosfsformat01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_fsdosfsformat01) $(support_includes)
endif

if TEST_fsdosfsname01
fs_tests += fsdosfsname01
fs_screens += fsdosfsname01/fsdosfsname01.scn
fs_docs += fsdosfsname01/fsdosfsname01.doc
fsdosfsname01_SOURCES = fsdosfsname01/init.c support/ramdisk_support.c
fsdosfsname01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_fsdosfsname01) \
	$(support_includes) $(test_includes)
endif

if TEST_fsdosfsname02
fs_tests += fsdosfsname02
fs_screens += fsdosfsname02/fsdosfsname02.scn
fs_docs += fsdosfsname02/fsdosfsname02.doc
fsdosfsname02_SOURCES = fsdosfsname02/init.c
fsdosfsname02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_fsdosfsname02) \
	$(support_includes)
endif

if TEST_fsdosfssync01
fs_tests += fsdosfssync01
fs_screens += fsdosfssync01/fsdosfssync01.scn
fs_docs += fsdosfssync01/fsdosfssync01.doc
fsdosfssync01_SOURCES = fsdosfssync01/init.c
fsdosfssync01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_fsdosfssync01) \
	$(support_includes)
endif

if TEST_fsdosfswrite01
fs_tests += fsdosfswrite01
fs_screens += fsdosfswrite01/fsdosfswrite01.scn
fs_docs += fsdosfswrite01/fsdosfswrite01.doc
fsdosfswrite01_SOURCES = fsdosfswrite01/init.c
fsdosfswrite01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_fsdosfswrite01) \
	$(support_includes)
endif

if TEST_fsfseeko01
fs_tests += fsfseeko01
fs_screens += fsfseeko01/fsfseeko01.scn
fs_docs += fsfseeko01/fsfseeko01.doc
fsfseeko01_SOURCES = fsfseeko01/init.c
fsfseeko01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_fsfseeko01) \
	$(support_includes)
endif

if TEST_fsimfsconfig01
fs_tests += fsimfsconfig01
fs_screens += fsimfsconfig01/fsimfsconfig01.scn
fs_docs += fsimfsconfig01/fsimfsconfig01.doc
fsimfsconfig01_SOURCES = fsimfsconfig01/init.c
fsimfsconfig01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_fsimfsconfig01) \
	$(support_includes)
endif

if TEST_fsimfsconfig02
fs_tests += fsimfsconfig02
fs_screens += fsimfsconfig02/fsimfsconfig02.scn
fs_docs += fsimfsconfig02/fsimfsconfig02.doc
fsimfsconfig02_SOURCES = fsimfsconfig02/init.c
fsimfsconfig02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_fsimfsconfig02) \
	$(support_includes)
endif

if TEST_fsimfsconfig03
fs_tests += fsimfsconfig03
fs_screens += fsimfsconfig03/fsimfsconfig03.scn
fs_docs += fsimfsconfig03/fsimfsconfig03.doc
fsimfsconfig03_SOURCES = fsimfsconfig03/init.c
fsimfsconfig03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_fsimfsconfig03) \
	$(support_includes)
endif

if TEST_fsimfsgeneric01
fs_tests += fsimfsgeneric01
fs_screens += fsimfsgeneric01/fsimfsgeneric01.scn
fs_docs += fsimfsgeneric01/fsimfsgeneric01.doc
fsimfsgeneric01_SOURCES = fsimfsgeneric01/init.c
fsimfsgeneric01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_fsimfsgeneric01) $(support_includes)
endif

if TEST_fsjffs2gc01
fs_tests += fsjffs2gc01
fs_screens += fsjffs2gc01/fsjffs2gc01.scn
fs_docs += fsjffs2gc01/fsjffs2gc01.doc
fsjffs2gc01_SOURCES = fsjffs2gc01/init.c support/fstest_support.c \
	support/fstest_support.h support/fstest.h \
	../psxtests/include/pmacros.h jffs2_support/fs_support.c \
	jffs2_support/fs_config.h
fsjffs2gc01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_fsjffs2gc01) \
	$(support_includes) $(test_includes) -I$(top_srcdir)/jffs2_support
fsjffs2gc01_LDADD = $(RTEMS_ROOT)cpukit/libjffs2.a $(LDADD)
endif

if TEST_fsnofs01
fs_tests += fsnofs01
fs_screens += fsnofs01/fsnofs01.scn
fs_docs += fsnofs01/fsnofs01.doc
fsnofs01_SOURCES = fsnofs01/init.c
fsnofs01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_fsnofs01) \
	$(support_includes)
endif

if TEST_fsrfsbitmap01
fs_tests += fsrfsbitmap01
fs_screens += fsrfsbitmap01/fsrfsbitmap01.scn
fs_docs += fsrfsbitmap01/fsrfsbitmap01.doc
fsrfsbitmap01_SOURCES = fsrfsbitmap01/test.c support/ramdisk_support.c \
	support/fstest_support.c support/fstest_support.h \
	support/ramdisk_support.h support/fstest.h \
	../psxtests/include/pmacros.h mrfs_support/fs_support.c \
	mrfs_support/fs_config.h
fsrfsbitmap01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_fsrfsbitmap01) \
	$(support_includes) $(test_includes) -I$(top_srcdir)/mrfs_support
endif

if TEST_fsrofs01
fs_tests += fsrofs01
fs_screens += fsrofs01/fsrofs01.scn
fs_docs += fsrofs01/fsrofs01.doc
fsrofs01_SOURCES = fsrofs01/init.c
fsrofs01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_fsrofs01) \
	$(support_includes)
endif

if TEST_imfs_fserror
fs_tests += imfs_fserror
fs_screens += fserror/imfs_fserror.scn
imfs_fserror_SOURCES = fserror/test.c support/fstest_support.c \
	support/fstest_support.h support/fstest.h \
	../psxtests/include/pmacros.h imfs_support/fs_support.c \
	imfs_spupport/fs_config.h
imfs_fserror_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_imfs_fserror) \
	$(support_includes) $(test_includes) -I$(top_srcdir)/imfs_support
endif

if TEST_imfs_fslink
fs_tests += imfs_fslink
fs_screens += fslink/imfs_fslink.scn
imfs_fslink_SOURCES = fslink/test.c support/fstest_support.c \
	support/fstest_support.h support/fstest.h \
	../psxtests/include/pmacros.h imfs_support/fs_support.c \
	imfs_support/fs_config.h
imfs_fslink_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_imfs_fslink) \
	$(support_includes) $(test_includes) -I$(top_srcdir)/imfs_support
endif

if TEST_imfs_fspatheval
fs_tests += imfs_fspatheval
fs_screens += fspatheval/imfs_fspatheval.scn
imfs_fspatheval_SOURCES = fspatheval/test.c support/fstest_support.c \
	support/fstest_support.h support/fstest.h \
	../psxtests/include/pmacros.h imfs_support/fs_support.c \
	imfs_support/fs_config.h
imfs_fspatheval_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_imfs_fspatheval) $(support_includes) $(test_includes) \
	-I$(top_srcdir)/imfs_support
endif

if TEST_imfs_fspermission
fs_tests += imfs_fspermission
fs_screens += fspermission/imfs_fspermission.scn
imfs_fspermission_SOURCES = fspermission/test.c \
	support/fstest_support.c support/fstest_support.h support/fstest.h \
	../psxtests/include/pmacros.h imfs_support/fs_support.c \
	imfs_support/fs_config.h
imfs_fspermission_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_imfs_fspermission) $(support_includes) $(test_includes) \
	-I$(top_srcdir)/imfs_support
endif

if TEST_imfs_fsrdwr
fs_tests += imfs_fsrdwr
fs_screens += fsrdwr/imfs_fsrdwr.scn
imfs_fsrdwr_SOURCES = fsrdwr/init.c support/fstest_support.c \
	support/fstest_support.h support/fstest.h \
	../psxtests/include/pmacros.h imfs_support/fs_support.c \
	imfs_support/fs_config.h
imfs_fsrdwr_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_imfs_fsrdwr) \
	$(support_includes) $(test_includes) -I$(top_srcdir)/imfs_support
endif

if TEST_imfs_fsscandir01
fs_tests += imfs_fsscandir01
fs_screens += fsscandir01/imfs_fsscandir01.scn
imfs_fsscandir01_SOURCES = fsscandir01/init.c support/fstest_support.c \
	support/fstest_support.h support/fstest.h \
	../psxtests/include/pmacros.h imfs_support/fs_support.c \
	imfs_support/fs_config.h
imfs_fsscandir01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_imfs_fsscandir01) $(support_includes) $(test_includes) \
	-I$(top_srcdir)/imfs_support
endif

if TEST_imfs_fssymlink
fs_tests += imfs_fssymlink
fs_screens += fssymlink/imfs_fssymlink.scn
imfs_fssymlink_SOURCES = fssymlink/test.c support/fstest_support.c \
	support/fstest_support.h support/fstest.h \
	../psxtests/include/pmacros.h imfs_support/fs_support.c \
	imfs_support/fs_config.h
imfs_fssymlink_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_imfs_fssymlink) \
	$(support_includes) $(test_includes) -I$(top_srcdir)/imfs_support
endif

if TEST_imfs_fstime
fs_tests += imfs_fstime
fs_screens += fstime/imfs_fstime.scn
imfs_fstime_SOURCES = fstime/test.c support/fstest_support.c \
	support/fstest_support.h support/fstest.h \
	../psxtests/include/pmacros.h imfs_support/fs_support.c \
	imfs_support/fs_config.h
imfs_fstime_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_imfs_fstime) \
	$(support_includes) $(test_includes) -I$(top_srcdir)/imfs_support
endif

if TEST_jffs2_fserror
fs_tests += jffs2_fserror
fs_screens += fserror/jffs2_fserror.scn
jffs2_fserror_SOURCES = fserror/test.c support/fstest_support.c \
	support/fstest_support.h support/fstest.h \
	../psxtests/include/pmacros.h jffs2_support/fs_support.c \
	jffs2_support/fs_config.h
jffs2_fserror_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_jffs2_fserror) \
	$(support_includes) $(test_includes) -I$(top_srcdir)/jffs2_support
jffs2_fserror_LDADD = $(RTEMS_ROOT)cpukit/libjffs2.a $(LDADD)
endif

if TEST_jffs2_fslink
fs_tests += jffs2_fslink
fs_screens += fslink/jffs2_fslink.scn
jffs2_fslink_SOURCES = fslink/test.c support/fstest_support.c \
	support/fstest_support.h support/fstest.h \
	../psxtests/include/pmacros.h jffs2_support/fs_support.c \
	jffs2_support/fs_config.h
jffs2_fslink_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_jffs2_fslink) \
	$(support_includes) $(test_includes) -I$(top_srcdir)/jffs2_support
jffs2_fslink_LDADD = $(RTEMS_ROOT)cpukit/libjffs2.a $(LDADD)
endif

if TEST_jffs2_fspatheval
fs_tests += jffs2_fspatheval
fs_screens += fspatheval/jffs2_fspatheval.scn
jffs2_fspatheval_SOURCES = fspatheval/test.c support/fstest_support.c \
	support/fstest_support.h support/fstest.h \
	../psxtests/include/pmacros.h jffs2_support/fs_support.c \
	jffs2_support/fs_config.h
jffs2_fspatheval_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_jffs2_fspatheval) $(support_includes) $(test_includes) \
	-I$(top_srcdir)/jffs2_support
jffs2_fspatheval_LDADD = $(RTEMS_ROOT)cpukit/libjffs2.a $(LDADD)
endif

if TEST_jffs2_fspermission
fs_tests += jffs2_fspermission
fs_screens += fspermission/jffs2_fspermission.scn
jffs2_fspermission_SOURCES = fspermission/test.c \
	support/fstest_support.c support/fstest_support.h support/fstest.h \
	../psxtests/include/pmacros.h jffs2_support/fs_support.c \
	jffs2_support/fs_config.h
jffs2_fspermission_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_jffs2_fspermission) $(support_includes) $(test_includes) \
	-I$(top_srcdir)/jffs2_support
jffs2_fspermission_LDADD = $(RTEMS_ROOT)cpukit/libjffs2.a $(LDADD)
endif

if TEST_jffs2_fsrdwr
fs_tests += jffs2_fsrdwr
fs_screens += fsrdwr/jffs2_fsrdwr.scn
jffs2_fsrdwr_SOURCES = fsrdwr/init.c support/fstest_support.c \
	support/fstest_support.h support/fstest.h \
	../psxtests/include/pmacros.h jffs2_support/fs_support.c \
	jffs2_support/fs_config.h
jffs2_fsrdwr_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_jffs2_fsrdwr) \
	$(support_includes) $(test_includes) -I$(top_srcdir)/jffs2_support
jffs2_fsrdwr_LDADD = $(RTEMS_ROOT)cpukit/libjffs2.a $(LDADD)
endif

if TEST_jffs2_fsscandir01
fs_tests += jffs2_fsscandir01
fs_screens += fsscandir01/jffs2_fsscandir01.scn
jffs2_fsscandir01_SOURCES = fsscandir01/init.c \
	support/fstest_support.c support/fstest_support.h support/fstest.h \
	../psxtests/include/pmacros.h jffs2_support/fs_support.c \
	jffs2_support/fs_config.h
jffs2_fsscandir01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_jffs2_fsscandir01) $(support_includes) $(test_includes) \
	-I$(top_srcdir)/jffs2_support
jffs2_fsscandir01_LDADD = $(RTEMS_ROOT)cpukit/libjffs2.a $(LDADD)
endif

if TEST_jffs2_fssymlink
fs_tests += jffs2_fssymlink
fs_screens += fssymlink/jffs2_fssymlink.scn
jffs2_fssymlink_SOURCES = fssymlink/test.c support/fstest_support.c \
	support/fstest_support.h support/fstest.h \
	../psxtests/include/pmacros.h jffs2_support/fs_support.c \
	jffs2_support/fs_config.h
jffs2_fssymlink_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_jffs2_fssymlink) $(support_includes) $(test_includes) \
	-I$(top_srcdir)/jffs2_support
jffs2_fssymlink_LDADD = $(RTEMS_ROOT)cpukit/libjffs2.a $(LDADD)
endif

if TEST_jffs2_fstime
fs_tests += jffs2_fstime
fs_screens += fstime/jffs2_fstime.scn
jffs2_fstime_SOURCES = fstime/test.c support/fstest_support.c \
	support/fstest_support.h support/fstest.h \
	../psxtests/include/pmacros.h jffs2_support/fs_support.c \
	jffs2_support/fs_config.h
jffs2_fstime_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_jffs2_fstime) \
	$(support_includes) $(test_includes) -I$(top_srcdir)/jffs2_support
jffs2_fstime_LDADD = $(RTEMS_ROOT)cpukit/libjffs2.a $(LDADD)
endif

if TEST_mdosfs_fserror
fs_tests += mdosfs_fserror
fs_screens += fserror/mdosfs_fserror.scn
mdosfs_fserror_SOURCES = fserror/test.c support/ramdisk_support.c \
	support/fstest_support.c support/fstest_support.h \
	support/ramdisk_support.h support/fstest.h \
	../psxtests/include/pmacros.h mdosfs_support/fs_support.c \
	mdosfs_support/fs_config.h
mdosfs_fserror_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mdosfs_fserror) \
	$(support_includes) $(test_includes) -I$(top_srcdir)/mdosfs_support
endif

if TEST_mdosfs_fspatheval
fs_tests += mdosfs_fspatheval
fs_screens += fspatheval/mdosfs_fspatheval.scn
mdosfs_fspatheval_SOURCES = fspatheval/test.c \
	support/ramdisk_support.c support/fstest_support.c \
	support/fstest_support.h support/ramdisk_support.h support/fstest.h \
	../psxtests/include/pmacros.h mdosfs_support/fs_support.c \
	mdosfs_support/fs_config.h
mdosfs_fspatheval_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_mdosfs_fspatheval) $(support_includes) $(test_includes) \
	-I$(top_srcdir)/mdosfs_support
endif

if TEST_mdosfs_fsrdwr
fs_tests += mdosfs_fsrdwr
fs_screens += fsrdwr/mdosfs_fsrdwr.scn
mdosfs_fsrdwr_SOURCES = fsrdwr/init.c support/ramdisk_support.c \
	support/fstest_support.c support/fstest_support.h \
	support/ramdisk_support.h support/fstest.h \
	../psxtests/include/pmacros.h mdosfs_support/fs_support.c \
	mdosfs_support/fs_config.h
mdosfs_fsrdwr_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mdosfs_fsrdwr) \
	$(support_includes) $(test_includes) -I$(top_srcdir)/mdosfs_support
endif

if TEST_mdosfs_fsscandir01
fs_tests += mdosfs_fsscandir01
fs_screens += fsscandir01/mdosfs_fsscandir01.scn
mdosfs_fsscandir01_SOURCES = fsscandir01/init.c \
	support/ramdisk_support.c support/fstest_support.c \
	support/fstest_support.h support/ramdisk_support.h support/fstest.h \
	../psxtests/include/pmacros.h mdosfs_support/fs_support.c \
	mdosfs_support/fs_config.h
mdosfs_fsscandir01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_mdosfs_fsscandir01) $(support_includes) $(test_includes) \
	-I$(top_srcdir)/mdosfs_support
endif

if TEST_mdosfs_fsstatvfs
fs_tests += mdosfs_fsstatvfs
fs_screens += fsstatvfs/mdosfs_fsstatvfs.scn
mdosfs_fsstatvfs_SOURCES = fsstatvfs/test.c support/ramdisk_support.c \
	support/fstest_support.c support/fstest_support.h \
	support/ramdisk_support.h support/fstest.h \
	../psxtests/include/pmacros.h mdosfs_support/fs_support.c \
	mdosfs_support/fs_config.h
mdosfs_fsstatvfs_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_mdosfs_fsstatvfs) $(support_includes) $(test_includes) \
	-I$(top_srcdir)/mdosfs_support
endif

if TEST_mdosfs_fstime
fs_tests += mdosfs_fstime
mdosfs_fstime_SOURCES = fstime/test.c support/ramdisk_support.c \
	support/fstest_support.c support/fstest_support.h \
	support/ramdisk_support.h support/fstest.h \
	../psxtests/include/pmacros.h mdosfs_support/fs_support.c \
	mdosfs_support/fs_config.h
mdosfs_fstime_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mdosfs_fstime) \
	$(support_includes) $(test_includes) -I$(top_srcdir)/mdosfs_support
endif

if TEST_mimfs_fserror
fs_tests += mimfs_fserror
fs_screens += fserror/mimfs_fserror.scn
mimfs_fserror_SOURCES = fserror/test.c support/fstest_support.c \
	support/fstest_support.h support/fstest.h \
	../psxtests/include/pmacros.h mimfs_support/fs_support.c \
	mimfs_support/fs_config.h
mimfs_fserror_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mimfs_fserror) \
	$(support_includes) $(test_includes) -I$(top_srcdir)/mimfs_support
endif

if TEST_mimfs_fslink
fs_tests += mimfs_fslink
mimfs_fslink_SOURCES = fslink/test.c support/fstest_support.c \
	support/fstest_support.h support/fstest.h \
	../psxtests/include/pmacros.h mimfs_support/fs_support.c \
	mimfs_support/fs_config.h
mimfs_fslink_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mimfs_fslink) \
	$(support_includes) $(test_includes) -I$(top_srcdir)/mimfs_support
endif

if TEST_mimfs_fspatheval
fs_tests += mimfs_fspatheval
mimfs_fspatheval_SOURCES = fspatheval/test.c support/fstest_support.c \
	support/fstest_support.h support/fstest.h \
	../psxtests/include/pmacros.h mimfs_support/fs_support.c \
	mimfs_support/fs_config.h
mimfs_fspatheval_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_mimfs_fspatheval) $(support_includes) $(test_includes) \
	-I$(top_srcdir)/mimfs_support
endif

if TEST_mimfs_fspermission
fs_tests += mimfs_fspermission
mimfs_fspermission_SOURCES = fspermission/test.c \
	support/fstest_support.c support/fstest_support.h support/fstest.h \
	../psxtests/include/pmacros.h mimfs_support/fs_support.c \
	mimfs_support/fs_config.h
mimfs_fspermission_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_mimfs_fspermission) $(support_includes) $(test_includes) \
	-I$(top_srcdir)/mimfs_support
endif

if TEST_mimfs_fsrdwr
fs_tests += mimfs_fsrdwr
mimfs_fsrdwr_SOURCES = fsrdwr/init.c support/fstest_support.c \
	support/fstest_support.h support/fstest.h \
	../psxtests/include/pmacros.h mimfs_support/fs_support.c \
	mimfs_support/fs_config.h
mimfs_fsrdwr_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mimfs_fsrdwr) \
	$(support_includes) $(test_includes) -I$(top_srcdir)/mimfs_support
endif

if TEST_mimfs_fsrename
fs_tests += mimfs_fsrename
fs_screens += fsrename/mimfs_fsrename.scn
mimfs_fsrename_SOURCES = fsrename/test.c support/fstest_support.c \
	support/fstest_support.h support/fstest.h \
	../psxtests/include/pmacros.h mimfs_support/fs_support.c \
	mimfs_support/fs_config.h
mimfs_fsrename_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mimfs_fsrename) \
	$(support_includes) $(test_includes) -I$(top_srcdir)/mimfs_support
endif

if TEST_mimfs_fsscandir01
fs_tests += mimfs_fsscandir01
mimfs_fsscandir01_SOURCES = fsscandir01/init.c \
	support/fstest_support.c support/fstest_support.h support/fstest.h \
	../psxtests/include/pmacros.h mimfs_support/fs_support.c \
	mimfs_support/fs_config.h
mimfs_fsscandir01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_mimfs_fsscandir01) $(support_includes) $(test_includes) \
	-I$(top_srcdir)/mimfs_support
endif

if TEST_mimfs_fssymlink
fs_tests += mimfs_fssymlink
mimfs_fssymlink_SOURCES = fssymlink/test.c support/fstest_support.c \
	support/fstest_support.h support/fstest.h \
	../psxtests/include/pmacros.h mimfs_support/fs_support.c \
	mimfs_support/fs_config.h
mimfs_fssymlink_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_mimfs_fssymlink) $(support_includes) $(test_includes) \
	-I$(top_srcdir)/mimfs_support
endif

if TEST_mimfs_fstime
fs_tests += mimfs_fstime
mimfs_fstime_SOURCES = fstime/test.c support/fstest_support.c \
	support/fstest_support.h support/fstest.h \
	../psxtests/include/pmacros.h mimfs_support/fs_support.c \
	mimfs_support/fs_config.h
mimfs_fstime_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mimfs_fstime) \
	$(support_includes) $(test_includes) -I$(top_srcdir)/mimfs_support
endif

if TEST_mrfs_fserror
fs_tests += mrfs_fserror
fs_screens += fserror/mrfs_fserror.scn
mrfs_fserror_SOURCES = fserror/test.c support/ramdisk_support.c \
	support/fstest_support.c support/fstest_support.h \
	support/ramdisk_support.h support/fstest.h \
	../psxtests/include/pmacros.h mrfs_support/fs_support.c \
	mrfs_support/fs_config.h
mrfs_fserror_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mrfs_fserror) \
	$(support_includes) $(test_includes) -I$(top_srcdir)/mrfs_support
endif

if TEST_mrfs_fsfpathconf
fs_tests += mrfs_fsfpathconf
fs_screens += fsfpathconf/mrfs_fsfpathconf.scn
mrfs_fsfpathconf_SOURCES = fsfpathconf/test.c \
	support/ramdisk_support.c support/fstest_support.c \
	support/fstest_support.h support/ramdisk_support.h support/fstest.h \
	../psxtests/include/pmacros.h mrfs_support/fs_support.c \
	mrfs_support/fs_config.h
mrfs_fsfpathconf_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_mrfs_fsfpathconf) $(support_includes) $(test_includes) \
	-I$(top_srcdir)/mrfs_support
endif

if TEST_mrfs_fslink
fs_tests += mrfs_fslink
fs_screens += fslink/mrfs_fslink.scn
mrfs_fslink_SOURCES = fslink/test.c support/ramdisk_support.c \
	support/fstest_support.c support/fstest_support.h \
	support/ramdisk_support.h support/fstest.h \
	../psxtests/include/pmacros.h mrfs_support/fs_support.c \
	mrfs_support/fs_config.h
mrfs_fslink_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mrfs_fslink) \
	$(support_includes) $(test_includes) -I$(top_srcdir)/mrfs_support
endif

if TEST_mrfs_fspatheval
fs_tests += mrfs_fspatheval
fs_screens += fspatheval/mrfs_fspatheval.scn
mrfs_fspatheval_SOURCES = fspatheval/test.c support/ramdisk_support.c \
	support/fstest_support.c support/fstest_support.h \
	support/ramdisk_support.h support/fstest.h \
	../psxtests/include/pmacros.h mrfs_support/fs_support.c \
	mrfs_support/fs_config.h
mrfs_fspatheval_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_mrfs_fspatheval) $(support_includes) $(test_includes) \
	-I$(top_srcdir)/mrfs_support
endif

if TEST_mrfs_fspermission
fs_tests += mrfs_fspermission
fs_screens += fspermission/mrfs_fspermission.scn
mrfs_fspermission_SOURCES = fspermission/test.c \
	support/ramdisk_support.c support/fstest_support.c \
	support/fstest_support.h support/ramdisk_support.h support/fstest.h \
	../psxtests/include/pmacros.h mrfs_support/fs_support.c \
	mrfs_support/fs_config.h
mrfs_fspermission_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_mrfs_fspermission) $(support_includes) $(test_includes) \
	-I$(top_srcdir)/mrfs_support
endif

if TEST_mrfs_fsrdwr
fs_tests += mrfs_fsrdwr
fs_screens += fsrdwr/mrfs_fsrdwr.scn
mrfs_fsrdwr_SOURCES = fsrdwr/init.c support/ramdisk_support.c \
	support/fstest_support.c support/fstest_support.h \
	support/ramdisk_support.h support/fstest.h \
	../psxtests/include/pmacros.h mrfs_support/fs_support.c \
	mrfs_support/fs_config.h
mrfs_fsrdwr_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mrfs_fsrdwr) \
	$(support_includes) $(test_includes) -I$(top_srcdir)/mrfs_support
endif

if TEST_mrfs_fsscandir01
fs_tests += mrfs_fsscandir01
fs_screens += fsscandir01/mrfs_fsscandir01.scn
mrfs_fsscandir01_SOURCES = fsscandir01/init.c \
	support/ramdisk_support.c support/fstest_support.c \
	support/fstest_support.h support/ramdisk_support.h support/fstest.h \
	../psxtests/include/pmacros.h mrfs_support/fs_support.c \
	mrfs_support/fs_config.h
mrfs_fsscandir01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_mrfs_fsscandir01) $(support_includes) $(test_includes) \
	-I$(top_srcdir)/mrfs_support
endif

if TEST_mrfs_fssymlink
fs_tests += mrfs_fssymlink
fs_screens += fssymlink/mrfs_fssymlink.scn
mrfs_fssymlink_SOURCES = fssymlink/test.c support/ramdisk_support.c \
	support/fstest_support.c support/fstest_support.h \
	support/ramdisk_support.h support/fstest.h \
	../psxtests/include/pmacros.h mrfs_support/fs_support.c \
	mrfs_support/fs_config.h
mrfs_fssymlink_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mrfs_fssymlink) \
	$(support_includes) $(test_includes) -I$(top_srcdir)/mrfs_support
endif

if TEST_mrfs_fstime
fs_tests += mrfs_fstime
fs_screens += fstime/mrfs_fstime.scn
mrfs_fstime_SOURCES = fstime/test.c support/ramdisk_support.c \
	support/fstest_support.c support/fstest_support.h \
	support/ramdisk_support.h support/fstest.h \
	../psxtests/include/pmacros.h mrfs_support/fs_support.c \
	mrfs_support/fs_config.h
mrfs_fstime_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mrfs_fstime) \
	$(support_includes) $(test_includes) -I$(top_srcdir)/mrfs_support
endif

noinst_PROGRAMS = $(fs_tests)
