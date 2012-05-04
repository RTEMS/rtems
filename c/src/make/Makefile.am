ACLOCAL_AMFLAGS = -I aclocal -I ../aclocal

CLEANFILES =

rtems_makedir = $(pkgdatadir)/make

GENERIC_FILES = host.cfg

MAKE_FILES = README directory.cfg lib.cfg

rtems_make_DATA = $(MAKE_FILES) $(GENERIC_FILES)

noinst_DATA = leaf.cfg
EXTRA_DIST = leaf.cfg

rtemsdir = $(exec_prefix)
rtems_bspdir = $(rtemsdir)/@RTEMS_BSP@
rtems_bsp_DATA = Makefile.inc

rtems_bsp_makedir = $(rtems_bspdir)/make
rtems_bsp_make_DATA = bsp.cfg target.cfg

bsp.cfg: bsp.cfg.in Makefile
	sed \
	-e "s|[@]HAS_MP[@]|$(HAS_MP)|" \
	-e "s|[@]HAS_POSIX_API[@]|$(HAS_POSIX_API)|" \
	-e "s|[@]HAS_CPLUSPLUS[@]|$(HAS_CPLUSPLUS)|" \
	-e "s|[@]HAS_NETWORKING[@]|$(HAS_NETWORKING)|" \
	< $(srcdir)/bsp.cfg.in > bsp.cfg
CLEANFILES += bsp.cfg

target.cfg: target.cfg.in Makefile
	sed \
	-e "s|[@]RTEMS_API[@]|$(RTEMS_API)|" \
	-e "s|[@]CPPFLAGS[@]|$(CPPFLAGS)|" \
	-e "s|[@]CFLAGS[@]|$(CFLAGS)|" \
	-e "s|[@]LIBS[@]|$(LIBS)|" \
	-e "s|[@]CC[@]|$(CC)|" \
	-e "s|[@]CXX[@]|$(CXX)|" \
	-e "s|[@]AS[@]|$(AS)|" \
	-e "s|[@]AR[@]|$(AR)|" \
	-e "s|[@]NM[@]|$(NM)|" \
	-e "s|[@]LD[@]|$(LD)|" \
	-e "s|[@]SIZE[@]|$(SIZE)|" \
	-e "s|[@]STRIP[@]|$(STRIP)|" \
	-e "s|[@]RANLIB[@]|$(RANLIB)|" \
	-e "s|[@]OBJCOPY[@]|$(OBJCOPY)|" \
	-e "s|[@]RTEMS_CPU[@]|$(RTEMS_CPU)|" \
	-e "s|[@]RTEMS_HAS_MULTIPROCESSING[@]|$(HAS_MP)|" \
	-e "s|[@]RTEMS_HAS_POSIX_API[@]|$(HAS_POSIX_API)|" \
	-e "s|[@]RTEMS_HAS_CPLUSPLUS[@]|$(HAS_CPLUSPLUS)|" \
	-e "s|[@]RTEMS_HAS_NETWORKING[@]|$(HAS_NETWORKING)|" \
	< $(srcdir)/target.cfg.in > target.cfg
CLEANFILES += target.cfg

host.cfg: host.cfg.in Makefile
	sed \
	-e "s|[@]CP[@]|$(CP)|" \
	-e "s|[@]MV[@]|$(MV)|" \
	-e "s|[@]LN[@]|$(LN)|" \
	-e "s|[@]SHELL[@]|$(SHELL)|" \
	-e "s|[@]KSH[@]|$(KSH)|" \
	< $(srcdir)/host.cfg.in > host.cfg
CLEANFILES += host.cfg

Makefile.inc: Makefile.inc.in Makefile
	sed \
	-e "s|[@]prefix[@]|$(prefix)|" \
	-e "s|[@]exec_prefix[@]|$(exec_prefix)|" \
	-e "s|[@]pkgdatadir[@]|$(pkgdatadir)|" \
	-e "s|[@]RTEMS_BSP[@]|$(RTEMS_BSP)|" \
	-e "s|[@]CC[@]|$(CC)|" \
	-e "s|[@]CXX[@]|$(CXX)|" \
	-e "s|[@]AS[@]|$(AS)|" \
	-e "s|[@]AR[@]|$(AR)|" \
	-e "s|[@]NM[@]|$(NM)|" \
	-e "s|[@]LD[@]|$(LD)|" \
	-e "s|[@]SIZE[@]|$(SIZE)|" \
	-e "s|[@]OBJCOPY[@]|$(OBJCOPY)|" \
	-e "s|[@]RTEMS_HAS_POSIX_API[@]|$(HAS_POSIX_API)|" \
	-e "s|[@]RTEMS_HAS_CPLUSPLUS[@]|$(HAS_CPLUSPLUS)|" \
	< $(srcdir)/Makefile.inc.in > Makefile.inc
CLEANFILES += Makefile.inc

## At the moment all actively supported configurations
## use gcc-target-default.cfg only.
rtems_make_compilersdir = $(rtems_makedir)/compilers
dist_rtems_make_compilers_DATA = compilers/gcc-target-default.cfg

include $(top_srcdir)/../automake/local.am
