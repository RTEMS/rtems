dnl Set the EXE extension
dnl
dnl 98/06/16 David Fiddes   	(D.J.Fiddes@hw.ac.uk)
dnl				Hacked from automake-1.3

# Check to see if we're running under Win32, without using
# AC_CANONICAL_*.  If so, set output variable EXEEXT to ".exe".
# Otherwise set it to "".

dnl RTEMS_EXEEXT()
dnl This knows we add .exe if we're building in the Cygwin32
dnl environment. But if we're not, then it compiles a test program
dnl to see if there is a suffix for executables.
AC_DEFUN(RTEMS_EXEEXT,
[AC_REQUIRE([RTEMS_CYGWIN32])
AC_MSG_CHECKING([for executable suffix])
AC_CACHE_VAL(rtems_cv_exeext,
[if test "$CYGWIN32" = yes; then
rtems_cv_exeext=.exe
else
cat > rtems_c_test.c << 'EOF'
int main() {
/* Nothing needed here */
}
EOF
${CC-cc} -o rtems_c_test $CFLAGS $CPPFLAGS $LDFLAGS rtems_c_test.c $LIBS 1>&5
rtems_cv_exeext=`echo rtems_c_test.* | grep -v rtems_c_test.c | sed -e s/rtems_c_test//`
rm -f rtems_c_test*])
test x"${rtems_cv_exeext}" = x && rtems_cv_exeext=no
fi
EXEEXT=""
test x"${rtems_cv_exeext}" != xno && EXEEXT=${rtems_cv_exeext}
AC_MSG_RESULT(${rtems_cv_exeext})
AC_SUBST(EXEEXT)])

