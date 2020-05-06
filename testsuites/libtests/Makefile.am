#
# Library Testsuite
#

ACLOCAL_AMFLAGS = -I ../aclocal

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am

lib_tests =
lib_screens =
lib_docs =

support_includes = -I$(top_srcdir)/../support/include
test_includes = -I$(top_srcdir)/support

if TEST_block01
lib_tests += block01
lib_screens += block01/block01.scn
lib_docs += block01/block01.doc
block01_SOURCES = block01/init.c
block01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_block01) \
	$(support_includes)
endif

if TEST_block02
lib_tests += block02
lib_screens += block02/block02.scn
lib_docs += block02/block02.doc
block02_SOURCES = block02/init.c
block02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_block02) \
	$(support_includes)
endif

if TEST_block03
lib_tests += block03
lib_screens += block03/block03.scn
lib_docs += block03/block03.doc
block03_SOURCES = block03/init.c
block03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_block03) \
	$(support_includes)
endif

if TEST_block04
lib_tests += block04
lib_screens += block04/block04.scn
lib_docs += block04/block04.doc
block04_SOURCES = block04/init.c
block04_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_block04) \
	$(support_includes)
endif

if TEST_block05
lib_tests += block05
lib_screens += block05/block05.scn
lib_docs += block05/block05.doc block05/block05.png
block05_SOURCES = block05/init.c
block05_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_block05) \
	$(support_includes)
endif

if TEST_block06
lib_tests += block06
lib_screens += block06/block06.scn
lib_docs += block06/block06.doc
block06_SOURCES = block06/init.c
block06_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_block06) \
	$(support_includes)
endif

if TEST_block07
lib_tests += block07
lib_screens += block07/block07.scn
lib_docs += block07/block07.doc
block07_SOURCES = block07/init.c
block07_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_block07) \
	$(support_includes)
endif

if TEST_block08
lib_tests += block08
lib_screens += block08/block08.scn
lib_docs += block08/block08.doc
block08_SOURCES = block08/init.c block08/system.h \
	block08/bdbuf_tests.c block08/bdbuf_tests.h block08/test_disk.c \
	block08/bdbuf_test1_1.c block08/bdbuf_test1_2.c \
	block08/bdbuf_test1_3.c block08/bdbuf_test1_4.c \
	block08/bdbuf_test1_5.c block08/bdbuf_test2_1.c \
	block08/bdbuf_test2_2.c block08/bdbuf_test3_1.c \
	block08/bdbuf_test3_2.c block08/bdbuf_test3_3.c \
	block08/bdbuf_test4_1.c block08/bdbuf_test4_2.c \
	block08/bdbuf_test4_3.c
block08_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_block08) \
	$(support_includes)
endif

if TEST_block09
lib_tests += block09
lib_screens += block09/block09.scn
lib_docs += block09/block09.doc
block09_SOURCES = block09/init.c
block09_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_block09) \
	$(support_includes)
endif

if TEST_block10
lib_tests += block10
lib_screens += block10/block10.scn
lib_docs += block10/block10.doc block10/block10.png
block10_SOURCES = block10/init.c
block10_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_block10) \
	$(support_includes)
endif

if TEST_block11
lib_tests += block11
lib_screens += block11/block11.scn
lib_docs += block11/block11.doc
block11_SOURCES = block11/init.c
block11_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_block11) \
	$(support_includes)
endif

if TEST_block12
lib_tests += block12
lib_screens += block12/block12.scn
lib_docs += block12/block12.doc
block12_SOURCES = block12/init.c
block12_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_block12) \
	$(support_includes)
endif

if TEST_block13
lib_tests += block13
lib_screens += block13/block13.scn
lib_docs += block13/block13.doc
block13_SOURCES = block13/init.c
block13_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_block13) \
	$(support_includes)
endif

if TEST_block14
lib_tests += block14
lib_screens += block14/block14.scn
lib_docs += block14/block14.doc
block14_SOURCES = block14/init.c
block14_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_block14) \
	$(support_includes)
endif

if TEST_block15
lib_tests += block15
lib_screens += block15/block15.scn
lib_docs += block15/block15.doc
block15_SOURCES = block15/init.c
block15_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_block15) \
	$(support_includes)
endif

if TEST_block16
lib_tests += block16
lib_screens += block16/block16.scn
lib_docs += block16/block16.doc
block16_SOURCES = block16/init.c
block16_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_block16) \
	$(support_includes)
endif

if TEST_block17
lib_tests += block17
lib_screens += block17/block17.scn
lib_docs += block17/block17.doc
block17_SOURCES = block17/init.c
block17_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_block17) \
	$(support_includes)
endif

if TEST_bspcmdline01
lib_tests += bspcmdline01
lib_screens += bspcmdline01/bspcmdline01.scn
lib_docs += bspcmdline01/bspcmdline01.doc
bspcmdline01_SOURCES = bspcmdline01/init.c
bspcmdline01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_bspcmdline01) \
	$(support_includes)
endif

if TEST_calloc
lib_tests += calloc.norun
calloc_norun_SOURCES = POSIX/calloc.c
calloc_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_capture01
lib_tests += capture01
lib_screens += capture01/capture01.scn
lib_docs += capture01/capture01.doc
capture01_SOURCES = capture01/init.c capture01/test1.c \
	capture01/system.h
capture01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_capture01) \
	$(support_includes)
endif

if TEST_clock_gettime
lib_tests += clock_gettime.norun
clock_gettime_norun_SOURCES = POSIX/clock_gettime.c
clock_gettime_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_close
lib_tests += close.norun
close_norun_SOURCES = POSIX/close.c
close_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_complex
lib_tests += complex
lib_screens += complex/complex.scn
complex_SOURCES = complex/init.c complex/docomplex.c \
	complex/docomplexf.c complex/docomplexl.c
complex_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_complex) \
	$(support_includes)
complex_LDADD = -lm $(LDADD)
endif

if TEST_cpuuse
lib_tests += cpuuse
lib_screens += cpuuse/cpuuse.scn
cpuuse_SOURCES = cpuuse/init.c cpuuse/task1.c cpuuse/task2.c \
	cpuuse/task3.c cpuuse/tswitch.c cpuuse/system.h
cpuuse_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_cpuuse) \
	$(support_includes)
endif

if TEST_crypt01
lib_tests += crypt01
lib_screens += crypt01/crypt01.scn
lib_docs += crypt01/crypt01.doc
crypt01_SOURCES = crypt01/init.c
crypt01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_crypt01) \
	$(support_includes)
endif

if DEBUGGERTESTS
if TEST_debugger01
lib_tests += debugger01
lib_screens += debugger01/debugger01.scn
debugger01_SOURCES = debugger01/init.c debugger01/remote.c \
	debugger01/system.h
debugger01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_debugger01) \
	$(support_includes)
debugger01_LDADD = $(RTEMS_ROOT)cpukit/libdebugger.a $(LDADD)
endif
endif

if TEST_defaultconfig01
lib_tests += defaultconfig01
lib_screens += defaultconfig01/defaultconfig01.scn
lib_docs += defaultconfig01/defaultconfig01.doc
defaultconfig01_SOURCES = defaultconfig01/init.c
defaultconfig01_CPPFLAGS = $(AM_CPPFLAGS) \
	$(TEST_FLAGS_defaultconfig01) $(support_includes)
defaultconfig01_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_devfs01
lib_tests += devfs01
lib_screens += devfs01/devfs01.scn
lib_docs += devfs01/devfs01.doc
devfs01_SOURCES = devfs01/init.c
devfs01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_devfs01) \
	$(support_includes)
endif

if TEST_devfs04
lib_tests += devfs04
lib_screens += devfs04/devfs04.scn
lib_docs += devfs04/devfs04.doc
devfs04_SOURCES = devfs04/init.c devfs04/test_driver.c \
	devfs04/test_driver.h
devfs04_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_devfs04) \
	$(support_includes)
endif

if TEST_deviceio01
lib_tests += deviceio01
lib_screens += deviceio01/deviceio01.scn
lib_docs += deviceio01/deviceio01.doc
deviceio01_SOURCES = deviceio01/init.c deviceio01/test_driver.c \
	deviceio01/test_driver.h
deviceio01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_deviceio01) \
	$(support_includes)
endif

if TEST_devnullfatal01
lib_tests += devnullfatal01
lib_screens += devnullfatal01/devnullfatal01.scn
lib_docs += devnullfatal01/devnullfatal01.doc
devnullfatal01_SOURCES = devnullfatal01/init.c
devnullfatal01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_devnullfatal01) \
	$(support_includes)
endif

if DLTESTS
if TEST_dl01
lib_tests += dl01
lib_screens += dl01/dl01.scn
lib_docs += dl01/dl01.doc
dl01_SOURCES = dl01/init.c dl01/dl-load.c dl01-tar.c dl01-tar.h
dl01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_dl01) $(support_includes)
dl01/init.c: dl01-tar.o
dl01.pre: $(dl01_OBJECTS) $(dl01_DEPENDENCIES)
	@rm -f dl01.pre
	$(AM_V_CCLD)$(LINK.c) $(CPU_CFLAGS) $(AM_CFLAGS) $(AM_LDFLAGS) -o $@ $+
dl01-o1.o: dl01/dl01-o1.c Makefile
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl01.tar: dl01-o1.o
	@rm -f $@
	$(AM_V_GEN)$(PAX) -w -f $@ $<
dl01-tar.c: dl01.tar
	$(AM_V_GEN)$(BIN2C) -C $< $@
dl01-tar.h: dl01.tar
	$(AM_V_GEN)$(BIN2C) -H $< $@
dl01-tar.o: dl01-tar.c dl01-tar.h
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl01-sym.o: dl01.pre
	$(AM_V_GEN)rtems-syms -e -C $(CC) -c "$(CFLAGS)" -o $@ $<
dl01$(EXEEXT):  $(dl01_OBJECTS) $(dl01_DEPENDENCIES) dl01-sym.o
	@rm -f $@
	$(AM_V_CCLD)$(LINK.c) $(CPU_CFLAGS) $(AM_CFLAGS) $(AM_LDFLAGS) -o $@ $+
CLEANFILES += dl01.pre dl01-sym.o dl02-o1.o dl01.tar dl01-tar.h
endif
endif

if DLTESTS
if TEST_dl02
lib_tests += dl02
lib_screens += dl02/dl02.scn
lib_docs += dl02/dl02.doc
dl02_SOURCES = dl02/init.c dl02/dl-load.c dl02-tar.c dl02-tar.h
dl02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_dl02) $(support_includes)
dl02/init.c: dl02-tar.o
dl02.pre: $(dl02_OBJECTS) $(dl02_DEPENDENCIES)
	@rm -f dl02.pre
	$(AM_V_CCLD)$(LINK.c) $(CPU_CFLAGS) $(AM_CFLAGS) $(AM_LDFLAGS) -o $@ $+
dl02-o1.o: dl02/dl02-o1.c Makefile
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl02-o2.o: dl02/dl02-o2.c Makefile
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl02.tar: dl02-o1.o dl02-o2.o
	@rm -f $@
	$(AM_V_GEN)$(PAX) -w -f $@ $+
dl02-tar.c: dl02.tar
	$(AM_V_GEN)$(BIN2C) -C $< $@
dl02-tar.h: dl02.tar
	$(AM_V_GEN)$(BIN2C) -H $< $@
dl02-tar.o: dl02-tar.c dl02-tar.h
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl02-sym.o: dl02.pre
	$(AM_V_GEN)rtems-syms -e -C $(CC) -c "$(CFLAGS)" -o $@ $<
dl02$(EXEEXT):  $(dl02_OBJECTS) $(dl02_DEPENDENCIES) dl02-sym.o
	@rm -f $@
	$(AM_V_CCLD)$(LINK.c) $(CPU_CFLAGS) $(AM_CFLAGS) $(AM_LDFLAGS) -o $@ $+
CLEANFILES += dl02.pre dl02-sym.o dl02-o1.o dl02-o2.o dl02.tar dl02-tar.h
endif
endif

if DLTESTS
if TEST_dl03
lib_tests += dl03
lib_screens += dl03/dl03.scn
lib_docs += dl03/dl03.doc
dl03_SOURCES = dl03/init.c dl03/dl-cache.c dl03/dl-bit-alloc.c
dl03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_dl03) $(support_includes) \
	-I$(top_srcdir)/../../cpukit/libdl
endif
endif

if DLTESTS
if HAS_CXX
if TEST_dl04
lib_tests += dl04
lib_screens += dl04/dl04.scn
lib_docs += dl04/dl04.doc
dl04_SOURCES = dl04/init.c dl04/dl-load.c dl04/dl-cpp.cc dl04-tar.c \
	dl04-tar.h
dl04_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_dl04) $(support_includes)
dl04/init.c: dl04-tar.o
dl04.pre: $(dl04_OBJECTS) $(dl04_DEPENDENCIES)
	@rm -f dl04.pre
	$(AM_V_CXXLD)$(LINK.cc) $(CPU_CFLAGS) $(AM_CFLAGS) $(AM_LDFLAGS) -o $@ $+
dl04-o4.o: dl04/dl04-o4.cc Makefile
	$(AM_V_GEN)$(CXXCOMPILE) -c -o $@ $<
dl04.tar: dl04-o4.o
	@rm -f $@
	$(AM_V_GEN)$(PAX) -w -f $@ $<
dl04-tar.c: dl04.tar
	$(AM_V_GEN)$(BIN2C) -C $< $@
dl04-tar.h: dl04.tar
	$(AM_V_GEN)$(BIN2C) -H $< $@
dl04-tar.o: dl04-tar.c dl04-tar.h
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl04-sym.o: dl04.pre
	$(AM_V_GEN)rtems-syms -e -C $(CC) -c "$(CFLAGS)" -o $@ $<
dl04$(EXEEXT):  $(dl04_OBJECTS) $(dl04_DEPENDENCIES) dl04-sym.o
	@rm -f $@
	$(AM_V_CXXLD)$(LINK.cc) $(CPU_CFLAGS) $(AM_CFLAGS) $(AM_LDFLAGS) -o $@ $+
CLEANFILES += dl04.pre dl04-sym.o dl04-o4.o dl04.tar dl04-tar.h
endif
endif
endif

if DLTESTS
if HAS_CXX
if TEST_dl05
lib_tests += dl05
lib_screens += dl05/dl05.scn
lib_docs += dl05/dl05.doc
dl05_SOURCES = dl05/init.c dl05/dl-load.c dl05/dl-cpp.cc dl05-tar.c \
	dl05-tar.h
dl05_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_dl05) $(support_includes)
dl05/init.c: dl05-tar.o
dl05.pre: $(dl05_OBJECTS) $(dl05_DEPENDENCIES)
	@rm -f dl05.pre dl05-sym.o
	$(AM_V_CXXLD)$(LINK.cc) $(CPU_CFLAGS) $(AM_CFLAGS) $(AM_LDFLAGS) -o $@ $+
dl05-o5.o: dl05/dl05-o5.cc
	$(AM_V_CXX)$(CXXCOMPILE) -c -o $@ $<
dl05.tar: dl05-o5.o
	@rm -f $@
	$(AM_V_GEN)$(PAX) -w -f $@ $<
dl05-tar.c: dl05.tar
	$(AM_V_GEN)$(BIN2C) -C $< $@
dl05-tar.h: dl05.tar
	$(AM_V_GEN)$(BIN2C) -H $< $@
dl05-tar.o: dl05-tar.c dl05-tar.h
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl05-sym.o: dl05.pre
	$(AM_V_GEN)rtems-syms -e -C $(CC) -c "$(CFLAGS)" -o $@ $<
dl05$(EXEEXT): $(dl05_OBJECTS) $(dl05_DEPENDENCIES) dl05-sym.o
	@rm -f $@
	$(AM_V_CXXLD)$(LINK.cc) $(CPU_CFLAGS) $(AM_CFLAGS) $(AM_LDFLAGS) -o $@ $+
CLEANFILES += dl05.pre dl05-sym.o dl05-o5.o dl05.tar dl05-tar.h
endif
endif
endif

if DLTESTS
if TEST_dl06
lib_tests += dl06
lib_screens += dl06/dl06.scn
lib_docs += dl06/dl06.doc
dl06_SOURCES = dl06/init.c dl06/dl-load.c dl06-tar.c dl06-tar.h
dl06_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_dl06) $(support_includes)
dl06/init.c: dl06-tar.o
dl06-pre.tar: Makefile
	@rm -f $@
	$(AM_V_GEN)$(PAX) -w -f $@ -s ,$(srcdir)/dl06/,, $(srcdir)/dl06/dl06_pre_file
dl06-pre-tar.c: dl06-pre.tar
	$(AM_V_GEN)$(BIN2C) -N dl06_tar -C $< $@
dl06-pre-tar.h: dl06-pre.tar
	$(AM_V_GEN)$(BIN2C) -N dl06_tar -H $< $@
dl06-pre-tar.o: dl06-pre-tar.c dl06-pre-tar.h
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl06-pre-init.o: dl06-pre-tar.o
	$(AM_V_CC)$(COMPILE)  $(dl06_CPPFLAGS) $(CPU_CFLAGS) $(AM_CFLAGS) \
	  -c -o $@ $(srcdir)/dl06/pre-init.c
dl06.pre: dl06-pre-init.o dl06/dl06-dl-load.o dl06-pre-tar.o
	@rm -f $@ dl06-sym.o
	$(AM_V_CCLD)$(LINK.c) $(CPU_CFLAGS) $(AM_CFLAGS) $(AM_LDFLAGS) -o $@ $+ $(LDADD)
dl06-o1.o: dl06/dl06-o1.c Makefile
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl06-o2.o: dl06/dl06-o2.c Makefile
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl06.rap: dl06.pre dl06-o1.o dl06-o2.o
	$(AM_V_GEN)rtems-ld -r $(RTEMS_BUILD_ROOT) \
	  -C $(CC) -c "$(CPU_CFLAGS)" \
	  -O rap -b $< -e rtems_main -s \
	  -o $@ $(filter-out dl06.pre,$+) -lm
dl06.tar: dl06.rap
	@rm -f $@
	$(AM_V_GEN)$(PAX) -w -f $@ $<
dl06-tar.c: dl06.tar
	$(AM_V_GEN)$(BIN2C) -C $< $@
dl06-tar.h: dl06.tar
	$(AM_V_GEN)$(BIN2C) -H $< $@
dl06-tar.o: dl06-tar.c dl06-tar.h
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl06-sym.o: dl06.pre
	$(AM_V_GEN)rtems-syms -e -C $(CC) -c "$(CFLAGS)" -o $@ $<
dl06$(EXEEXT): $(dl06_OBJECTS) dl06-sym.o
	@rm -f $@
	$(AM_V_CCLD)$(LINK.c) $(CPU_CFLAGS) $(AM_CFLAGS) $(AM_LDFLAGS) -o $@ $+ $(LDADD)
CLEANFILES += dl06.pre dl06-sym.o dl06-o1.o dl06-o2.o dl06.tar dl06-tar.h
endif
endif

if DLTESTS
if TEST_dl07
lib_tests += dl07
lib_screens += dl07/dl07.scn
lib_docs += dl07/dl07.doc
dl07_SOURCES = dl07/init.c dl07/dl-load.c dl07-tar.c dl07-tar.h
dl07_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_dl07) $(support_includes)
dl07/init.c: dl07-tar.o
dl07.pre: $(dl07_OBJECTS) $(dl07_DEPENDENCIES)
	@rm -f dl07.pre dl07-sym.o
	$(AM_V_CCLD)$(LINK.c) $(CPU_CFLAGS) $(AM_CFLAGS) $(AM_LDFLAGS) -o $@ $+
dl07-o1.o: dl07/dl07-o1.c Makefile
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl07-o2.o: dl07/dl07-o2.c Makefile
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl07-o3.o: dl07/dl07-o3.c Makefile
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl07-o4.o: dl07/dl07-o4.c Makefile
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl07-o5.o: dl07/dl07-o5.c Makefile
	$(AM_V_CC) $(COMPILE) -c -o $@ $<
dl07.tar: dl07-o1.o dl07-o2.o dl07-o3.o dl07-o4.o dl07-o5.o
	@rm -f $@
	$(AM_V_GEN)$(PAX) -w -f $@ $+
dl07-tar.c: dl07.tar
	$(AM_V_GEN)$(BIN2C) -C $< $@
dl07-tar.h: dl07.tar
	$(AM_V_GEN)$(BIN2C) -H $< $@
dl07-tar.o: dl07-tar.c dl07-tar.h
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl07-sym.o: dl07.pre
	$(AM_V_GEN)rtems-syms -e -C $(CC) -c "$(CFLAGS)" -o $@ $<
dl07$(EXEEXT):  $(dl07_OBJECTS) $(dl07_DEPENDENCIES) dl07-sym.o
	@rm -f $@
	$(AM_V_CCLD)$(LINK.c) $(CPU_CFLAGS) $(AM_CFLAGS) $(AM_LDFLAGS) -o $@ $+
CLEANFILES += dl07.pre dl07-sym.o dl07-o1.o dl07-o2.o dl07-o3.o dl07-o4.o \
		dl07-o5.o dl07.tar dl07-tar.h
endif
endif

if DLTESTS
if TEST_dl08
lib_tests += dl08
lib_screens += dl08/dl08.scn
lib_docs += dl08/dl08.doc
dl08_SOURCES = dl08/init.c dl08/dl-load.c dl08-tar.c dl08-tar.h
dl08_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_dl08) $(support_includes)
dl08/init.c: dl08-tar.o
dl08.pre: $(dl08_OBJECTS) $(dl08_DEPENDENCIES)
	@rm -f dl08.pre dl08-syms.o
	$(AM_V_CCLD)$(LINK.c) $(CPU_CFLAGS) $(AM_CFLAGS) $(AM_LDFLAGS) -o $@ $+
dl08-o1.o: dl08/dl08-o1.c Makefile
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl08-o2.o: dl08/dl08-o2.c Makefile
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl08-o3.o: dl08/dl08-o3.c Makefile
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl08-o4.o: dl08/dl08-o4.c Makefile
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl08-o5.o: dl08/dl08-o5.c Makefile
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl08-o6-123456789-123456789.o: dl08/dl08-o6-123456789-123456789.c Makefile
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
noinst_LIBRARIES = libdl08_1.a libdl08_2.a
libdl08_1_a_SOURCES = dl08-o2.c dl08-o4.c
libdl08_2_a_SOURCES = dl08-o3.c dl08-o5.c \
			dl08-o6-123456789-123456789.c
dl08.tar: dl08-o1.o libdl08_1.a libdl08_2.a
	@rm -f $@
	$(AM_V_GEN)$(PAX) -w -f $@ -s ,$(srcdir)/dl08/,, $(srcdir)/dl08/etc/libdl.conf $+
dl08-tar.c: dl08.tar
	$(AM_V_GEN)$(BIN2C) -C $< $@
dl08-tar.h: dl08.tar
	$(AM_V_GEN)$(BIN2C) -H $< $@
dl08-tar.o: dl08-tar.c dl08-tar.h
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl08-sym.o: dl08.pre
	$(AM_V_GEN)rtems-syms -e -c "$(CFLAGS)" -o $@ $<
dl08$(EXEEXT):  $(dl08_OBJECTS) $(dl08_DEPENDENCIES) dl08-sym.o
	@rm -f $@
	$(AM_V_CCLD)$(LINK.c) $(CPU_CFLAGS) $(AM_CFLAGS) $(AM_LDFLAGS) -o $@ $+
CLEANFILES += dl08.pre dl08-sym.o libdl08_1.a libdl08_2.a dl08-o1.o dl08-o2.o \
		dl08-o3.o dl08-o4.o dl08-o5.o dl08-o6-123456789-123456789.o \
		dl08.tar dl08-tar.h
endif
endif

if DLTESTS
if TEST_dl09
lib_tests += dl09
lib_screens += dl09/dl09.scn
lib_docs += dl09/dl09.doc
dl09_SOURCES = dl09/init.c dl09/dl-load.c dl09-tar.c dl09-tar.h
dl09_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_dl09) $(support_includes)
dl09/init.c: dl09-tar.o
dl09.pre: $(dl09_OBJECTS) $(dl09_DEPENDENCIES)
	@rm -f dl09.pre dl09-syms.o
	$(AM_V_CCLD)$(LINK.c) $(CPU_CFLAGS) $(AM_CFLAGS) $(AM_LDFLAGS) -o $@ $+
dl09-o1.o: dl09/dl09-o1.c Makefile
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl09-o2.o: dl09/dl09-o2.c Makefile
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl09-o3.o: dl09/dl09-o3.c Makefile
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl09-o4.o: dl09/dl09-o4.c Makefile
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl09-o5.o: dl09/dl09-o5.c Makefile
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl09.tar: dl09-o1.o dl09-o2.o dl09-o3.o dl09-o4.o dl09-o5.o
	@rm -f $@
	$(AM_V_GEN)$(PAX) -w -f $@ $+
dl09-tar.c: dl09.tar
	$(AM_V_GEN)$(BIN2C) -C $< $@
dl09-tar.h: dl09.tar
	$(AM_V_GEN)$(BIN2C) -H $< $@
dl09-tar.o: dl09-tar.c dl09-tar.h
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl09-sym.o: dl09.pre
	$(AM_V_GEN)rtems-syms -e -C $(CC) -c "$(CFLAGS)" -o $@ $<
dl09$(EXEEXT):  $(dl09_OBJECTS) $(dl09_DEPENDENCIES) dl09-sym.o
	@rm -f $@
	$(AM_V_CCLD)$(LINK.c) $(CPU_CFLAGS) $(AM_CFLAGS) $(AM_LDFLAGS) -o $@ $+
CLEANFILES += dl09.pre dl09-sym.o dl09-o1.o dl09-o2.o dl09-o3.o dl09-o4.o \
		dl09-o5.o dl09.tar dl09-tar.h
endif
endif

if DLTESTS
if TEST_dl10
lib_tests += dl10
lib_screens += dl10/dl10.scn
lib_docs += dl10/dl10.doc
dl10_SOURCES = dl10/init.c dl10/dl-load.c dl10-tar.c dl10-tar.h
dl10_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_dl10) $(support_includes)
dl10/init.c: dl10-tar.o
dl10.pre: $(dl10_OBJECTS) $(dl10_DEPENDENCIES)
	@rm -f dl10.pre dl10-syms.o
	$(AM_V_CCLD)$(LINK.c) $(CPU_CFLAGS) $(AM_CFLAGS) $(AM_LDFLAGS) -o $@ $+
dl10-o1.o: dl10/dl10-o1.c Makefile
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl10-o2.o: dl10/dl10-o2.c Makefile
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl10-o3.o: dl10/dl10-o3.c Makefile
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl10-o4.o: dl10/dl10-o4.c Makefile
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl10-o5.o: dl10/dl10-o5.c Makefile
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl10-o6.o: dl10/dl10-o6.c Makefile
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
noinst_LIBRARIES = libdl10_1.a libdl10_2.a
libdl10_1_a_SOURCES = dl10-o2.c dl10-o4.c
libdl10_2_a_SOURCES = dl10-o3.c dl10-o5.c dl10-o6.c
dl10.tar: dl10-o1.o libdl10_1.a libdl10_2.a
	@rm -f $@
	$(AM_V_GEN)$(PAX) -w -f $@ -s ,$(srcdir)/dl10/,, $(srcdir)/dl10/etc/libdl.conf $+
dl10-tar.c: dl10.tar
	$(AM_V_GEN)$(BIN2C) -C $< $@
dl10-tar.h: dl10.tar
	$(AM_V_GEN)$(BIN2C) -H $< $@
dl10-tar.o: dl10-tar.c dl10-tar.h
	$(AM_V_CC)$(COMPILE) -c -o $@ $<
dl10-sym.o: dl10.pre
	$(AM_V_GEN)rtems-syms -e -c "$(CFLAGS)" -o $@ $<
dl10$(EXEEXT):  $(dl10_OBJECTS) $(dl10_DEPENDENCIES) dl10-sym.o
	@rm -f $@
	$(AM_V_CCLD)$(LINK.c) $(CPU_CFLAGS) $(AM_CFLAGS) $(AM_LDFLAGS) -o $@ $+
CLEANFILES += dl10.pre dl10-sym.o libdl10_1.a libdl10_2.a dl10-o1.o dl10-o2.o \
		dl10-o3.o dl10-o4.o dl10-o5.o dl10-o6.o \
		dl10.tar dl10-tar.h etc/libdl-dl10.conf
endif
endif

if TEST_dumpbuf01
lib_tests += dumpbuf01
lib_screens += dumpbuf01/dumpbuf01.scn
lib_docs += dumpbuf01/dumpbuf01.doc
dumpbuf01_SOURCES = dumpbuf01/init.c
dumpbuf01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_dumpbuf01) \
	$(support_includes)
endif

if TEST_dup2
lib_tests += dup2.norun
dup2_norun_SOURCES = POSIX/dup2.c
dup2_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_exit01
lib_tests += exit01
lib_screens += exit01/exit01.scn
lib_docs += exit01/exit01.doc
exit01_SOURCES = exit01/init.c
exit01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_exit01) \
	$(support_includes)
endif

if TEST_exit02
lib_tests += exit02
lib_screens += exit02/exit02.scn
lib_docs += exit02/exit02.doc
exit02_SOURCES = exit02/init.c
exit02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_exit02) \
	$(support_includes)
endif

if TEST_fcntl
lib_tests += fcntl.norun
fcntl_norun_SOURCES = POSIX/fcntl.c
fcntl_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_flashdisk01
lib_tests += flashdisk01
lib_screens += flashdisk01/flashdisk01.scn
lib_docs += flashdisk01/flashdisk01.doc
flashdisk01_SOURCES = flashdisk01/init.c \
	flashdisk01/test-file-system.c
flashdisk01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_flashdisk01) \
	$(support_includes)
endif

if TEST_flockfile
lib_tests += flockfile.norun
flockfile_norun_SOURCES = POSIX/flockfile.c
flockfile_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_fork
lib_tests += fork.norun
fork_norun_SOURCES = POSIX/fork.c
fork_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_free
lib_tests += free.norun
free_norun_SOURCES = POSIX/free.c
free_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_fstat
lib_tests += fstat.norun
fstat_norun_SOURCES = POSIX/fstat.c
fstat_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if NETTESTS
if TEST_ftp01
lib_tests += ftp01
lib_screens += ftp01/ftp01.scn
lib_docs += ftp01/ftp01.doc
ftp01_SOURCES = ftp01/init.c
ftp01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_ftp01) \
	$(support_includes) -I$(RTEMS_SOURCE_ROOT)/cpukit/libnetworking
ftp01_LDADD = $(RTEMS_ROOT)cpukit/libftpd.a $(RTEMS_ROOT)cpukit/libftpfs.a $(LDADD)
endif
endif

if TEST_ftrylockfile
lib_tests += ftrylockfile.norun
ftrylockfile_norun_SOURCES = POSIX/ftrylockfile.c
ftrylockfile_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_funlockfile
lib_tests += funlockfile.norun
funlockfile_norun_SOURCES = POSIX/funlockfile.c
funlockfile_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_getentropy01
lib_tests += getentropy01
lib_screens += getentropy01/getentropy01.scn
lib_docs += getentropy01/getentropy01.doc
getentropy01_SOURCES = getentropy01/init.c
getentropy01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_getentropy01) \
	$(support_includes)
endif

if TEST_getlogin
lib_tests += getlogin.norun
getlogin_norun_SOURCES = POSIX/getlogin.c
getlogin_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_getpwnam
lib_tests += getpwnam.norun
getpwnam_norun_SOURCES = POSIX/getpwnam.c
getpwnam_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_getpwuid
lib_tests += getpwuid.norun
getpwuid_norun_SOURCES = POSIX/getpwuid.c
getpwuid_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_gettimeofday
lib_tests += gettimeofday.norun
gettimeofday_norun_SOURCES = POSIX/gettimeofday.c
gettimeofday_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_getuid
lib_tests += getuid.norun
getuid_norun_SOURCES = POSIX/getuid.c
getuid_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_gxx01
lib_tests += gxx01
lib_screens += gxx01/gxx01.scn
lib_docs += gxx01/gxx01.doc
gxx01_SOURCES = gxx01/init.c
gxx01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_gxx01) \
	$(support_includes)
endif

if TEST_heapwalk
lib_tests += heapwalk
lib_screens += heapwalk/heapwalk.scn
heapwalk_SOURCES = heapwalk/init.c heapwalk/system.h
heapwalk_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_heapwalk) \
	$(support_includes)
endif

if TEST_htonl
lib_tests += htonl.norun
htonl_norun_SOURCES = POSIX/htonl.c
htonl_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_i2c01
lib_tests += i2c01
lib_screens += i2c01/i2c01.scn
lib_docs += i2c01/i2c01.doc
i2c01_SOURCES = i2c01/init.c
i2c01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_i2c01) \
	$(support_includes)
endif

if TEST_iconv
lib_tests += iconv.norun
iconv_norun_SOURCES = POSIX/iconv.c
iconv_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_iconv_close
lib_tests += iconv_close.norun
iconv_close_norun_SOURCES = POSIX/iconv_close.c
iconv_close_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_iconv_open
lib_tests += iconv_open.norun
iconv_open_norun_SOURCES = POSIX/iconv_open.c
iconv_open_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_kill
lib_tests += kill.norun
kill_norun_SOURCES = POSIX/kill.c
kill_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_libfdt01
lib_tests += libfdt01
lib_screens += libfdt01/libfdt01.scn
lib_docs += libfdt01/libfdt01.doc
libfdt01_SOURCES = libfdt01/init.c libfdt01/some.c
libfdt01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_libfdt01) \
	$(support_includes)
endif

if TEST_longjmp
lib_tests += longjmp.norun
longjmp_norun_SOURCES = POSIX/longjmp.c
longjmp_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_lseek
lib_tests += lseek.norun
lseek_norun_SOURCES = POSIX/lseek.c
lseek_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_lstat
lib_tests += lstat.norun
lstat_norun_SOURCES = POSIX/lstat.c
lstat_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_malloc
lib_tests += malloc.norun
malloc_norun_SOURCES = POSIX/malloc.c
malloc_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_malloc02
lib_tests += malloc02
lib_screens += malloc02/malloc02.scn
lib_docs += malloc02/malloc02.doc
malloc02_SOURCES = malloc02/init.c
malloc02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_malloc02) \
	$(support_includes)
endif

if TEST_malloc03
lib_tests += malloc03
lib_screens += malloc03/malloc03.scn
lib_docs += malloc03/malloc03.doc
malloc03_SOURCES = malloc03/init.c
malloc03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_malloc03) \
	$(support_includes)
endif

if TEST_malloc04
lib_tests += malloc04
lib_screens += malloc04/malloc04.scn
lib_docs += malloc04/malloc04.doc
malloc04_SOURCES = malloc04/init.c
malloc04_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_malloc04) \
	$(support_includes)
endif

if TEST_malloctest
lib_tests += malloctest
lib_screens += malloctest/malloctest.scn
malloctest_SOURCES = malloctest/init.c malloctest/task1.c \
	malloctest/system.h
malloctest_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_malloctest) \
	$(support_includes)
endif

if TEST_math
lib_tests += math
lib_screens += math/math.scn
math_SOURCES = math/init.c math/domath.c
math_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_math) $(support_includes) \
	-I$(top_srcdir)/math
math_LDADD = -lm $(LDADD)
endif

if TEST_mathf
lib_tests += mathf
lib_screens += mathf/mathf.scn
mathf_SOURCES = mathf/init.c mathf/domathf.c
mathf_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mathf) \
	$(support_includes) -I$(top_srcdir)/math
mathf_LDADD = -lm $(LDADD)
endif

if TEST_mathl
lib_tests += mathl
lib_screens += mathl/mathl.scn
mathl_SOURCES = mathl/init.c mathl/domathl.c
mathl_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mathl) \
	$(support_includes) -I$(top_srcdir)/math
mathl_LDADD = -lm $(LDADD)
endif

if TEST_md501
lib_tests += md501
lib_screens += md501/md501.scn
lib_docs += md501/md501.doc
md501_SOURCES = md501/init.c
md501_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_md501) \
	$(support_includes)
endif

if NETTESTS
if HAS_POSIX
if TEST_mghttpd01
lib_tests += mghttpd01
lib_screens += mghttpd01/mghttpd01.scn
lib_docs += mghttpd01/mghttpd01.doc
mghttpd01_SOURCES = mghttpd01/init.c mghttpd01-tar.c \
	mghttpd01-tar.h mghttpd01/test-http-client.c \
	mghttpd01/test-http-client.h
mghttpd01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mghttpd01) \
	$(support_includes) -I$(RTEMS_SOURCE_ROOT)/cpukit/libnetworking
mghttpd01_LDADD = $(RTEMS_ROOT)cpukit/libmghttpd.a $(LDADD)
mghttpd01-tar.c: mghttpd01/mghttpd01.tar
	$(AM_V_GEN)$(BIN2C) -C $< $@
mghttpd01-tar.h: mghttpd01/mghttpd01.tar
	$(AM_V_GEN)$(BIN2C) -H $< $@
mghttpd01-tar.o: mghttpd01-tar.c mghttpd01-tar.h
mghttpd01/init.c: mghttpd01-tar.h
CLEANFILES += mghttpd01.tar mghttpd01-tar.c mghttpd01-tar.h
endif
endif
endif

if TEST_monitor
lib_tests += monitor
monitor_SOURCES = monitor/init.c monitor/system.h
monitor_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_monitor) \
	$(support_includes)
endif

if TEST_monitor02
lib_tests += monitor02
lib_screens += monitor02/monitor02.scn
monitor02_SOURCES = monitor02/init.c monitor02/system.h
monitor02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_monitor02) \
	$(support_includes)
endif

if TEST_mouse01
lib_tests += mouse01
lib_screens += mouse01/mouse01.scn
lib_docs += mouse01/mouse01.doc
mouse01_SOURCES = mouse01/init.c mouse01/serial_mouse_config.c \
	mouse01/msmouse.c termios04/termios_testdriver_intr.c
mouse01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_mouse01) \
	$(support_includes)
endif

if TEST_nanosleep
lib_tests += nanosleep.norun
nanosleep_norun_SOURCES = POSIX/nanosleep.c
nanosleep_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if NETTESTS
if TEST_networking01
lib_tests += networking01
lib_screens += networking01/networking01.scn
lib_docs += networking01/networking01.doc
networking01_SOURCES = networking01/init.c
networking01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_networking01) \
	$(support_includes) -I$(RTEMS_SOURCE_ROOT)/cpukit/libnetworking
endif
endif

if TEST_newlib01
lib_tests += newlib01
lib_screens += newlib01/newlib01.scn
lib_docs += newlib01/newlib01.doc
newlib01_SOURCES = newlib01/init.c
newlib01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_newlib01) \
	$(support_includes)
endif

if TEST_open
lib_tests += open.norun
open_norun_SOURCES = POSIX/open.c
open_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_pipe
lib_tests += pipe.norun
pipe_norun_SOURCES = POSIX/pipe.c
pipe_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_posix_memalign
lib_tests += posix_memalign.norun
posix_memalign_norun_SOURCES = POSIX/posix_memalign.c
posix_memalign_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_putenvtest
lib_tests += putenvtest
lib_screens += putenvtest/putenvtest.scn
putenvtest_SOURCES = putenvtest/init.c
putenvtest_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_putenvtest) \
	$(support_includes)
endif

if TEST_pwdgrp01
lib_tests += pwdgrp01
lib_screens += pwdgrp01/pwdgrp01.scn
lib_docs += pwdgrp01/pwdgrp01.doc
pwdgrp01_SOURCES = pwdgrp01/init.c
pwdgrp01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_pwdgrp01) \
	$(support_includes)
endif

if TEST_pwdgrp02
lib_tests += pwdgrp02
lib_screens += pwdgrp02/pwdgrp02.scn
lib_docs += pwdgrp02/pwdgrp02.doc
pwdgrp02_SOURCES = pwdgrp02/init.c
pwdgrp02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_pwdgrp02) \
	$(support_includes)
endif

if TEST_rbheap01
lib_tests += rbheap01
lib_screens += rbheap01/rbheap01.scn
lib_docs += rbheap01/rbheap01.doc
rbheap01_SOURCES = rbheap01/init.c
rbheap01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_rbheap01) \
	$(support_includes)
endif

if TEST_read
lib_tests += read.norun
read_norun_SOURCES = POSIX/read.c
read_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_readv
lib_tests += readv.norun
readv_norun_SOURCES = POSIX/readv.c
readv_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_realloc
lib_tests += realloc.norun
realloc_norun_SOURCES = POSIX/realloc.c
realloc_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_record01
lib_tests += record01
lib_screens += record01/record01.scn
lib_docs += record01/record01.doc
record01_SOURCES = record01/init.c
record01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_record01) \
	$(support_includes) -I$(RTEMS_SOURCE_ROOT)/cpukit/libnetworking
endif

if TEST_record02
lib_tests += record02
lib_screens += record02/record02.scn
lib_docs += record02/record02.doc
record02_SOURCES = record02/init.c
record02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_record02) \
	$(support_includes)
record02_LDADD = $(RTEMS_ROOT)cpukit/librtemscpu.a $(RTEMS_ROOT)cpukit/libz.a $(LDADD)
endif

if TEST_rtmonuse
lib_tests += rtmonuse
lib_screens += rtmonuse/rtmonuse.scn
rtmonuse_SOURCES = rtmonuse/init.c rtmonuse/getall.c rtmonuse/task1.c \
	rtmonuse/system.h
rtmonuse_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_rtmonuse) \
	$(support_includes)
endif

if TEST_setjmp
lib_tests += setjmp.norun
setjmp_norun_SOURCES = POSIX/setjmp.c
setjmp_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_sha
lib_tests += sha
lib_screens += sha/sha.scn
lib_docs += sha/sha.doc
sha_SOURCES = sha/init.c
sha_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sha) $(support_includes)
endif

if TEST_shell01
lib_tests += shell01
lib_screens += shell01/shell01.scn
lib_docs += shell01/shell01.doc
shell01_SOURCES = shell01/init.c
shell01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_shell01) \
	$(support_includes)
endif

if TEST_sigaddset
lib_tests += sigaddset.norun
sigaddset_norun_SOURCES = POSIX/sigaddset.c
sigaddset_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_sigdelset
lib_tests += sigdelset.norun
sigdelset_norun_SOURCES = POSIX/sigdelset.c
sigdelset_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_sigemptyset
lib_tests += sigemptyset.norun
sigemptyset_norun_SOURCES = POSIX/sigemptyset.c
sigemptyset_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_sigfillset
lib_tests += sigfillset.norun
sigfillset_norun_SOURCES = POSIX/sigfillset.c
sigfillset_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_sigismember
lib_tests += sigismember.norun
sigismember_norun_SOURCES = POSIX/sigismember.c
sigismember_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_sigprocmask
lib_tests += sigprocmask.norun
sigprocmask_norun_SOURCES = POSIX/sigprocmask.c
sigprocmask_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_sparsedisk01
lib_tests += sparsedisk01
lib_screens += sparsedisk01/sparsedisk01.scn
lib_docs += sparsedisk01/sparsedisk01.doc
sparsedisk01_SOURCES = sparsedisk01/init.c
sparsedisk01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_sparsedisk01) \
	$(support_includes)
endif

if TEST_spi01
lib_tests += spi01
lib_screens += spi01/spi01.scn
lib_docs += spi01/spi01.doc
spi01_SOURCES = spi01/init.c
spi01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_spi01) \
	$(support_includes)
endif

if TEST_stackchk
lib_tests += stackchk
lib_screens += stackchk/stackchk.scn
stackchk_SOURCES = stackchk/blow.c stackchk/init.c stackchk/task1.c \
	stackchk/system.h
stackchk_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_stackchk) \
	$(support_includes)
endif

if TEST_stackchk01
lib_tests += stackchk01
lib_screens += stackchk01/stackchk01.scn
lib_docs += stackchk01/stackchk01.doc
stackchk01_SOURCES = stackchk01/init.c
stackchk01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_stackchk01) \
	$(support_includes)
endif

if TEST_stat
lib_tests += stat.norun
stat_norun_SOURCES = POSIX/stat.c
stat_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_stringto01
lib_tests += stringto01
lib_screens += stringto01/stringto01.scn
lib_docs += stringto01/stringto01.doc
stringto01_SOURCES = stringto01/init.c \
	stringto01/stringto_test_template.h
stringto01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_stringto01) \
	$(support_includes)
endif

if NETTESTS
if TEST_syscall01
lib_tests += syscall01
lib_screens += syscall01/syscall01.scn
lib_docs += syscall01/syscall01.doc
syscall01_SOURCES = syscall01/init.c
syscall01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_syscall01) \
	$(support_includes) -I$(RTEMS_SOURCE_ROOT)/cpukit/libnetworking
endif
endif

if TARTESTS
if TEST_tar01
lib_tests += tar01
lib_screens += tar01/tar01.scn
lib_docs += tar01/tar01.doc
tar01_SOURCES = tar01/init.c ../psxtests/psxfile01/test_cat.c \
	tar01-tar.c tar01-tar.h tar01-tar-gz.c tar01-tar-gz.h
if TARTEST_XZ
tar01_SOURCES += tar01-tar-xz.c tar01-tar-xz.h
endif
tar01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_tar01) \
	$(support_includes) $(test_includes) -I$(top_srcdir)/include
tar01_LDADD = $(RTEMS_ROOT)cpukit/librtemscpu.a $(RTEMS_ROOT)cpukit/libz.a $(LDADD)
tar01-tar.c: tar01/tar01.tar
	$(AM_V_GEN)$(BIN2C) -C $< $@
tar01-tar.h: tar01/tar01.tar
	$(AM_V_GEN)$(BIN2C) -H $< $@
tar01-tar.o: tar01-tar.c tar01-tar.h
tar01.tar.gz: tar01/tar01.tar
	$(AM_V_GEN)$(GZIP) < $< > $@
tar01-tar-gz.c: tar01.tar.gz
	$(AM_V_GEN)$(BIN2C) -C $< $@
tar01-tar-gz.h: tar01.tar.gz
	$(AM_V_GEN)$(BIN2C) -H $< $@
CLEANFILES += tar01.tar tar01-tar.c tar01-tar.h \
	tar01.tar.gz tar01-tar-gz.c tar01-tar-gz.h
if TARTEST_XZ
tar01.tar.xz: tar01/tar01.tar
	$(AM_V_GEN)$(XZ) < $< > $@
tar01-tar-xz.c: tar01.tar.xz
	$(AM_V_GEN)$(BIN2C) -C $< $@
tar01-tar-xz.h: tar01.tar.xz
	$(AM_V_GEN)$(BIN2C) -H $< $@
TAR01_XZ_H = tar01-tar-xz.h
CLEANFILES += tar01.tar.xz tar01-tar-xz.c tar01-tar-xz.h
endif
tar01/init.c: tar01-tar.h tar01-tar-gz.h $(TAR01_XZ_H)
endif
endif

if TARTESTS
if TEST_tar02
lib_tests += tar02
lib_screens += tar02/tar02.scn
lib_docs += tar02/tar02.doc
tar02_SOURCES = tar02/init.c ../psxtests/psxfile01/test_cat.c \
	tar02-tar.c tar02-tar.h
tar02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_tar02) \
	$(support_includes) $(test_includes) -I$(top_srcdir)/include
tar02-tar.c: tar01/tar01.tar
	$(AM_V_GEN)$(BIN2C) -C $< $@
tar02-tar.h: tar01/tar01.tar
	$(AM_V_GEN)$(BIN2C) -H $< $@
tar02-tar.o: tar02-tar.c tar02-tar.h
tar02/init.c: tar02-tar.h
CLEANFILES += tar02.tar tar02-tar.c tar02-tar.h
endif
endif

if TEST_tar03
lib_tests += tar03
lib_screens += tar03/tar03.scn
lib_docs += tar03/tar03.doc
tar03_SOURCES = tar03/init.c
tar03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_tar03) \
	$(support_includes)
endif

if NETTESTS
if TEST_telnetd01
lib_tests += telnetd01
lib_screens += telnetd01/telnetd01.scn
lib_docs += telnetd01/telnetd01.doc
telnetd01_SOURCES = telnetd01/init.c
telnetd01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_telnetd01) \
	$(support_includes) -I$(RTEMS_SOURCE_ROOT)/cpukit/libnetworking
telnetd01_LDADD = $(RTEMS_ROOT)cpukit/libtelnetd.a $(LDADD)
endif
endif

if TEST_termios
lib_tests += termios
termios_SOURCES = termios/init.c
termios_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_termios) \
	$(support_includes)
endif

if TEST_termios01
lib_tests += termios01
lib_screens += termios01/termios01.scn
lib_docs += termios01/termios01.doc
termios01_SOURCES = termios01/init.c termios01/termios_testdriver.c \
	termios01/termios_testdriver.h
termios01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_termios01) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_termios02
lib_tests += termios02
lib_screens += termios02/termios02.scn
lib_docs += termios02/termios02.doc
termios02_SOURCES = termios02/init.c
termios02_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_termios02) \
	$(support_includes) -I$(top_srcdir)/include
endif

if TEST_termios03
lib_tests += termios03
lib_screens += termios03/termios03.scn
lib_docs += termios03/termios03.doc
termios03_SOURCES = termios03/init.c \
	termios03/termios_testdriver_polled.c \
	termios03/termios_testdriver_polled.h
termios03_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_termios03) \
	$(support_includes)
endif

if TEST_termios04
lib_tests += termios04
lib_screens += termios04/termios04.scn
lib_docs += termios04/termios04.doc
termios04_SOURCES = termios04/init.c \
	termios04/termios_testdriver_intr.c \
	termios04/termios_testdriver_intr.h
termios04_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_termios04) \
	$(support_includes)
endif

if TEST_termios05
lib_tests += termios05
lib_screens += termios05/termios05.scn
lib_docs += termios05/termios05.doc
termios05_SOURCES = termios05/init.c \
	termios05/termios_testdriver_task.c
termios05_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_termios05) \
	$(support_includes)
endif

if TEST_termios06
lib_tests += termios06
lib_screens += termios06/termios06.scn
lib_docs += termios06/termios06.doc
termios06_SOURCES = termios06/init.c \
	termios03/termios_testdriver_polled.c termios06/test_pppd.c
termios06_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_termios06) \
	$(support_includes)
endif

if TEST_termios07
lib_tests += termios07
lib_screens += termios07/termios07.scn
lib_docs += termios07/termios07.doc
termios07_SOURCES = termios07/init.c \
	termios04/termios_testdriver_intr.c
termios07_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_termios07) \
	$(support_includes)
endif

if TEST_termios08
lib_tests += termios08
lib_screens += termios08/termios08.scn
lib_docs += termios08/termios08.doc
termios08_SOURCES = termios08/init.c \
	termios03/termios_testdriver_polled.c
termios08_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_termios08) \
	$(support_includes)
endif

if TEST_termios09
lib_tests += termios09
lib_screens += termios09/termios09.scn
lib_docs += termios09/termios09.doc
termios09_SOURCES = termios09/init.c
termios09_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_termios09) \
	$(support_includes) 
endif

if TEST_termios10
if HAS_POSIX
lib_tests += termios10
lib_screens += termios10/termios10.scn
lib_docs += termios10/termios10.doc
termios10_SOURCES  = termios10/init.c
termios10_SOURCES += termios03/termios_testdriver_polled.c
termios10_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_termios10) \
	$(support_includes)
endif
endif

if TEST_termios11
if HAS_POSIX
lib_tests += termios11
lib_screens += termios11/termios11.scn
lib_docs += termios11/termios11.doc
termios11_SOURCES  = termios11/init.c
termios11_SOURCES += termios04/termios_testdriver_intr.c
termios11_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_termios11) \
	$(support_includes)
endif
endif

if TEST_top
lib_tests += top
lib_screens += top/top.scn
top_SOURCES = top/init.c top/task1.c top/task2.c top/task3.c \
	top/system.h
top_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_top) $(support_includes)
endif

if TEST_ttest01
lib_tests += ttest01
lib_screens += ttest01/ttest01.scn
lib_docs += ttest01/ttest01.doc
ttest01_SOURCES = ttest01/init.c
ttest01_SOURCES += ttest01/test-assert.c
ttest01_SOURCES += ttest01/test-checks.c
ttest01_SOURCES += ttest01/test-destructor.c
ttest01_SOURCES += ttest01/test-eno.c
ttest01_SOURCES += ttest01/test-example.c
ttest01_SOURCES += ttest01/test-fixture.c
ttest01_SOURCES += ttest01/test-leak.c
ttest01_SOURCES += ttest01/test-log.c
ttest01_SOURCES += ttest01/test-malloc.c
ttest01_SOURCES += ttest01/test-plan.c
ttest01_SOURCES += ttest01/test-psx.c
ttest01_SOURCES += ttest01/test-rtems.c
ttest01_SOURCES += ttest01/test-simple.c
ttest01_SOURCES += ttest01/test-step.c
ttest01_SOURCES += ttest01/test-task-context.c
ttest01_SOURCES += ttest01/test-time.c
ttest01_SOURCES += ttest01/test-verbosity.c
ttest01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_ttest01) \
	$(support_includes)
endif

if TEST_tztest
lib_tests += tztest
lib_screens += tztest/tztest.scn
lib_docs += tztest/tztest.doc
tztest_SOURCES = tztest/init.c
tztest_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_tztest) \
	$(support_includes)
endif

if TEST_uid01
lib_tests += uid01
lib_docs += uid01/uid01.doc
uid01_SOURCES = uid01/init.c uid01/serial_mouse_config.c \
	uid01/msmouse.c termios04/termios_testdriver_intr.c
uid01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_uid01) \
	$(support_includes)
endif

if TEST_unlink
lib_tests += unlink.norun
unlink_norun_SOURCES = POSIX/unlink.c
unlink_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_utf8proc01
lib_tests += utf8proc01
lib_screens += utf8proc01/utf8proc01.scn
lib_docs += utf8proc01/utf8proc01.doc
utf8proc01_SOURCES = utf8proc01/init.c
utf8proc01_CPPFLAGS = $(AM_CPPFLAGS) $(TEST_FLAGS_utf8proc01) \
	$(support_includes)
endif

if TEST_vfork
lib_tests += vfork.norun
vfork_norun_SOURCES = POSIX/vfork.c
vfork_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_wait
lib_tests += wait.norun
wait_norun_SOURCES = POSIX/wait.c
wait_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_waitpid
lib_tests += waitpid.norun
waitpid_norun_SOURCES = POSIX/waitpid.c
waitpid_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_write
lib_tests += write.norun
write_norun_SOURCES = POSIX/write.c
write_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

if TEST_writev
lib_tests += writev.norun
writev_norun_SOURCES = POSIX/writev.c
writev_norun_LDADD = $(RTEMS_ROOT)cpukit/librtemsdefaultconfig.a $(LDADD)
endif

noinst_PROGRAMS = $(lib_tests)
