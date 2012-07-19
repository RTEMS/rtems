dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])

AC_INIT([rtems-doc],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([project.am])
RTEMS_TOP([..])

AM_INIT_AUTOMAKE([no-define foreign 1.12.2])
AM_MAINTAINER_MODE

RTEMS_ENABLE_RPMPREFIX

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

AC_CHECK_PROGS(TEXI2HTML,texi2html)
AC_SUBST(TEXI2HTML)

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

AC_CONFIG_SUBDIRS(tools)

AC_CONFIG_FILES([Makefile],[],[
test -d common || mkdir common
cat << EOF > common/rtems.sed~
:t
s/@RTEMSAPI@/_RTEMS_API/;t t
s,@RTEMSPREFIX@,$prefix,;t t
s,@RTEMSRPMPREFIX@,$rpmprefix,;t t
EOF
_RTEMS_UPDATE_CONDITIONAL([common/rtems.sed],[common/rtems.sed~])
])

AC_CONFIG_FILES([
index.html
develenv/Makefile
user/Makefile
bsp_howto/Makefile])

AC_CONFIG_FILES([
porting/Makefile
networking/Makefile
posix_users/Makefile
posix1003.1/Makefile
filesystem/Makefile
ada_user/Makefile
started/Makefile
relnotes/Makefile
new_chapters/Makefile
cpu_supplement/Makefile
shell/Makefile
texi2html_init
])
AC_OUTPUT
