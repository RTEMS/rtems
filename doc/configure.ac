dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)

AC_INIT([rtems-doc],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([project.am])
RTEMS_TOP([..])

AM_INIT_AUTOMAKE([no-define foreign 1.7.2])
AM_MAINTAINER_MODE

AC_ARG_ENABLE(html,
[  --disable-html          disable html support ],
[case $enableval in
yes) ;;
no) ;;
*) AC_MSG_ERROR("invalid value");;
esac],
[enable_html="yes"]
)

AC_ARG_ENABLE(dvi,
[  --disable-dvi           disable dvi support ],
[case $enableval in
yes) ;;
no) ;;
*) AC_MSG_ERROR("invalid value");;
esac],
[enable_dvi="yes"]
)

AC_ARG_ENABLE(pdf,
[  --disable-pdf           disable pdf support ],
[case $enableval in
yes) ;;
no) ;;
*) AC_MSG_ERROR("invalid value");;
esac],
[enable_pdf="yes"]
)

AC_ARG_ENABLE(ps,
[  --disable-ps            disable ps support ],
[case $enableval in
yes) ;;
no) ;;
*) AC_MSG_ERROR("invalid value");;
esac],
[enable_ps="yes"]
)

AC_ARG_ENABLE(papersize,
[  --enable-papersize=[letter|a4|]    set papersize [default:system defaults]],
[case $enableval in
a4) papersize=a4 ;;
letter) papersize=letter ;;
yes) papersize= ;;
no) papersize= ;;
*) AC_MSG_ERROR([Invalid papersize])
;;
esac],
[papersize=]
)

pkgdocdir="\$(datadir)/rtems"
AC_SUBST(pkgdocdir)

htmldir="\$(pkgdocdir)/html"
AC_SUBST(htmldir)

dnl Checks for programs.
AC_PROG_LN_S

AC_CHECK_PROGS(PERL,perl)

TEXI2WWW='$(PERL) $(top_srcdir)/tools/texi2www/texi2www'
AC_SUBST(TEXI2WWW)

AC_CHECK_PROGS(GS,gs)
AM_CONDITIONAL(GS,test x"$GS" != x"")

AC_CHECK_PROGS(TEXI2DVI,texi2dvi)
AM_CONDITIONAL(TEXI2DVI,test x"$TEXI2DVI" != x"")

AC_CHECK_PROGS(EPSTOPDF,epstopdf)
AM_CONDITIONAL(EPSTOPDF,
  test "$enable_pdf" = "yes" \
    && test x"$EPSTOPDF" != x"" )

AC_CHECK_PROGS(TEXI2PDF,texi2pdf)
AM_CONDITIONAL(TEXI2PDF,
  test "$enable_pdf" = "yes" \
    && test x"$TEXI2PDF" != x"")

AM_CONDITIONAL(USE_HTML,
  test "$enable_html" = "yes" \
     && test x"PERL" != x"" )

AM_CONDITIONAL(USE_DVI,
  test "$enable_dvi" = "yes" \
    && test x"$TEXI2DVI" != x"" )

AM_CONDITIONAL(USE_PS,
  test "$enable_ps" = "yes" \
    && test x"$TEXI2DVI" != x"" )

AM_CONDITIONAL(USE_PDF, 
  test "$enable_pdf" = "yes" \
    && test x"$TEXI2DVI" != x"" \
    && test x"$TEXI2PDF" != x"" )

case $papersize in
a4) 
  TEXI2DVI="${TEXI2DVI} --texinfo=@afourpaper"
  TEXI2PDF="${TEXI2PDF} --texinfo=@afourpaper"
  DVIPS="dvips -t a4"
;;
letter)
  DVIPS="dvips -t letter"
  ;;
*)
  DVIPS="dvips"
;;
esac
AC_SUBST(DVIPS)

BMENU2='$(top_builddir)/tools/bmenu/bmenu2'
AC_SUBST(BMENU2)

dnl Checks for libraries.

dnl Checks for header files.

dnl Checks for typedefs, structures, and compiler characteristics.

dnl Checks for library functions.

AC_MSG_CHECKING([for ../scripts/setup.def])
AS_IF([test -f ${srcdir}/../scripts/setup.def],[
. ${srcdir}/../scripts/setup.def
AC_MSG_RESULT([done])
],[
AC_MSG_ERROR([missing])
])

AS_IF([test -z "$binutils_patch_version"],
[set_binutils_patch="@clear BINUTILSPATCHVERSION"],
[set_binutils_patch="@set BINUTILSPATCHVERSION $binutils_patch_version"])

AS_IF([test -z "$gcc3newlib_patch_version"],
[set_newlib_patch="@clear NEWLIBPATCHVERSION"],
[set_newlib_patch="@set NEWLIBPATCHVERSION $gcc3newlib_patch_version"])

AS_IF([test -z "$gcc3_patch_version"],
[set_gcc3_patch="@clear GCCPATCHVERSION"],
[set_gcc_patch="@set GCCPATCHVERSION $gcc3_patch_version"])

AS_IF([test -z "$gdb_patch_version"],
[set_gdb_patch="@clear GDBPATCHVERSION"],
[set_gdb_patch="@set GDBPATCHVERSION $gdb_patch_version"])

AC_CONFIG_SUBDIRS(tools)

AC_CONFIG_FILES([Makefile],[],[
test -d common || mkdir common
cat << EOF > common/rtems.sed~
:t
s/@RTEMSAPI@/RTEMS_API/;t t
s,@RTEMSPREFIX@,$prefix,;t t
EOF
_RTEMS_UPDATE_CONDITIONAL([common/rtems.sed],[common/rtems.sed~])
])

AC_CONFIG_FILES([
rtems_support.html
index.html
FAQ/Makefile
develenv/Makefile
user/Makefile
bsp_howto/Makefile])

AC_CONFIG_FILES([started/Makefile],[],[
test -d started || mkdir started
cat << EOF > started/tversions.sed~
:t
s/@GCCVERSION@/$gcc3_version/;t t
s/@BINUTILSVERSION@/$binutils_version/;t t
s/@NEWLIBVERSION@/$gcc3newlib_version/;t t
s/@GDBVERSION@/$gdb_version/;t t
s/@SETBINUTILSPATCHVERSION@/$set_binutils_patch/;t t
s/@SETGCCPATCHVERSION@/$set_gcc_patch/;t t
s/@SETNEWLIBPATCHVERSION@/$set_newlib_patch/;t t
s/@SETGDBPATCHVERSION@/$set_gdb_patch/;t t
s/@GCCRPMRELEASE@/$gcc3newlib_rpm_release/;t t
s/@BINUTILSRPMRELEASE@/$binutils_rpm_release/;t t
s/@GDBRPMRELEASE@/$gdb_rpm_release/;t t
EOF
_RTEMS_UPDATE_CONDITIONAL([started/tversions.sed],[started/tversions.sed~])
])

AC_CONFIG_FILES([
porting/Makefile
networking/Makefile
posix_users/Makefile
posix1003.1/Makefile
filesystem/Makefile
itron3.0/Makefile
ada_user/Makefile
started_ada/Makefile
rtems_gdb/Makefile
rgdb_specs/Makefile
relnotes/Makefile
new_chapters/Makefile
supplements/Makefile
supplements/arm/Makefile
supplements/c4x/Makefile
supplements/i386/Makefile
supplements/i960/Makefile
supplements/m68k/Makefile
supplements/mips/Makefile
supplements/mips64orion/Makefile
supplements/powerpc/Makefile
supplements/sh/Makefile
supplements/sparc/Makefile
supplements/template/Makefile
gnu_docs/Makefile
])
AC_OUTPUT
