dnl aclocal.m4 generated automatically by aclocal 1.2

dnl
dnl $Id$
dnl

dnl macro to detect mkdir
AC_DEFUN(RTEMS_PATH_MKDIR,
[AC_PATH_PROG(MKDIR,mkdir)
])

dnl macro to check for mkdir -p
AC_DEFUN(RTEMS_PROG_MKDIR_P,
[
AC_REQUIRE([RTEMS_PATH_MKDIR])
AC_MSG_CHECKING([for working $MKDIR -p])
AC_CACHE_VAL(rtems_cv_prog_mkdir_p,
[rm -rf conftestdata
if $MKDIR -p conftestdata 2>/dev/null ;then
rtems_cv_prog_MKDIR_P="yes"
else
rtems_cv_prog_MKDIR_P="no"
fi])dnl
rm -rf conftestdata
AC_MSG_RESULT($rtems_cv_prog_MKDIR_P)
])

dnl macro to check for mkdir -m 0755
AC_DEFUN(RTEMS_PROG_MKDIR_M,
[
AC_REQUIRE([RTEMS_PATH_MKDIR])
AC_MSG_CHECKING([for working $MKDIR -m 0755])
AC_CACHE_VAL(rtems_cv_prog_MKDIR_P,
[rm -rf conftestdata
if $MKDIR -m 0775 conftestdata 2>/dev/null; then
rtems_cv_prog_MKDIR_M="yes"
else
rtems_cv_prog_MKDIR_M="no"
fi])dnl
rm -rf conftestdata
AC_MSG_RESULT($rtems_cv_prog_MKDIR_M)
])


dnl
dnl $Id$
dnl

dnl canonicalize target name
dnl NOTE: Most rtems targets do not fullfil autoconf's
dnl target naming conventions "processor-vendor-os"
dnl Therefore autoconf's AC_CANONICAL_TARGET will fail for them
dnl and we have to fix it for rtems ourselves 

AC_DEFUN(RTEMS_CANONICAL_TARGET_CPU,
[AC_MSG_CHECKING(rtems target cpu)
changequote(<<, >>)dnl
target_cpu=`echo $target | sed 's%^\([^-]*\)-\(.*\)$%\1%'`
changequote([, ])dnl
AC_MSG_RESULT($target_cpu)
])

dnl
dnl  $Id$
dnl 
dnl Set program_prefix
dnl
dnl 98/05/20 Ralf Corsepius	(corsepiu@faw.uni-ulm.de)
dnl				Extracted from configure

AC_DEFUN(RTEMS_TOOL_PREFIX,
[AC_REQUIRE([AC_CANONICAL_TARGET])dnl
AC_REQUIRE([AC_CANONICAL_BUILD])dnl

if [[ "${program_prefix}" = "NONE" ]] ; then
  if [[ "${target}" = "${host}" ]] ; then
    program_prefix=
  else
    program_prefix=${target}-
  fi
fi
])

dnl
dnl $Id$
dnl 
dnl Check for target gcc
dnl
dnl 98/05/20 Ralf Corsepius 	(corsepiu@faw.uni-ulm.de)
dnl				Completely reworked

AC_DEFUN(RTEMS_PROG_CC,
[
AC_BEFORE([$0], [AC_PROG_CPP])dnl
AC_BEFORE([$0], [AC_PROG_CC])dnl
AC_REQUIRE([RTEMS_TOOL_PREFIX])dnl

dnl Only accept gcc and cc
dnl NOTE: This might be too restrictive for native compilation
AC_PATH_PROGS(CC_FOR_TARGET, "$program_prefix"gcc "$program_prefix"cc )
test -z "$CC_FOR_TARGET" \
  && AC_MSG_ERROR([no acceptable cc found in \$PATH])

dnl backup 
rtems_save_CC=$CC
rtems_save_CFLAGS=$CFLAGS

dnl temporarily set CC
CC=$CC_FOR_TARGET

AC_PROG_CC_WORKS
AC_PROG_CC_GNU

if test $ac_cv_prog_gcc = yes; then
  GCC=yes
dnl Check whether -g works, even if CFLAGS is set, in case the package
dnl plays around with CFLAGS (such as to build both debugging and
dnl normal versions of a library), tasteless as that idea is.
  ac_test_CFLAGS="${CFLAGS+set}"
  ac_save_CFLAGS="$CFLAGS"
  CFLAGS=
  AC_PROG_CC_G
  if test "$ac_test_CFLAGS" = set; then
    CFLAGS="$ac_save_CFLAGS"
  elif test $ac_cv_prog_cc_g = yes; then
    CFLAGS="-g -O2"
  else
    CFLAGS="-O2"
  fi
else
  GCC=
  test "${CFLAGS+set}" = set || CFLAGS="-g"
fi

rtems_cv_prog_gcc=$ac_cv_prog_gcc
rtems_cv_prog_cc_g=$ac_cv_prog_cc_g
rtems_cv_prog_cc_works=$ac_cv_prog_cc_works
rtems_cv_prog_cc_cross=$ac_cv_prog_cc_cross

dnl restore initial values
CC=$rtems_save_CC
CFLAGS=$rtems_save_CFLAGS

unset ac_cv_prog_gcc
unset ac_cv_prog_cc_g
unset ac_cv_prog_cc_works
unset ac_cv_prog_cc_cross
])

dnl
dnl $Id$
dnl
dnl Check whether the target compiler accepts -specs
dnl
dnl 98/02/11 Ralf Corsepius 	corsepiu@faw.uni-ulm.de
dnl

AC_DEFUN(RTEMS_GCC_SPECS,
[AC_REQUIRE([RTEMS_PROG_CC])
AC_CACHE_CHECK(whether $CC_FOR_TARGET accepts -specs,rtems_cv_gcc_specs,
[
rtems_cv_gcc_specs=no
if test "$rtems_cv_prog_gcc" = "yes"; then
  touch confspec
  echo 'void f(){}' >conftest.c
  if test -z "`${CC_FOR_TARGET} -specs confspec -c conftest.c 2>&1`";then
    rtems_cv_gcc_specs=yes
  fi
fi
rm -f confspec conftest*
])])

dnl
dnl $Id$
dnl
dnl Check whether the target compiler accepts -pipe
dnl
dnl 98/02/11 Ralf Corsepius     corsepiu@faw.uni-ulm.de
dnl

AC_DEFUN(RTEMS_GCC_PIPE,
[AC_REQUIRE([RTEMS_PROG_CC]) 
AC_REQUIRE([AC_CANONICAL_HOST])
AC_CACHE_CHECK(whether $CC_FOR_TARGET accepts --pipe,rtems_cv_gcc_pipe,
[
rtems_cv_gcc_pipe=no
if test "$rtems_cv_prog_gcc" = "yes"; then
case "$host_os" in
  cygwin32*)
    ;;
  *)
    echo 'void f(){}' >conftest.c
    if test -z "`${CC_FOR_TARGET} --pipe -c conftest.c 2>&1`";then
      rtems_cv_gcc_pipe=yes
    fi
    rm -f conftest*
    ;;
esac
fi
])
])

dnl
dnl $Id$
dnl 
dnl Check for target g++
dnl 
dnl 98/05/20 Ralf Corsepius 	(corsepiu@faw.uni-ulm.de)
dnl				Completely reworked

AC_DEFUN(RTEMS_PROG_CXX,
[
AC_BEFORE([$0], [AC_PROG_CXXCPP])dnl
AC_BEFORE([$0], [AC_PROG_CXX])dnl
AC_REQUIRE([RTEMS_TOOL_PREFIX])dnl

dnl Only accept g++ and c++
dnl NOTE: This might be too restrictive for native compilation
AC_PATH_PROGS(CXX_FOR_TARGET, "$program_prefix"g++ "$program_prefix"c++)
test -z "$CXX_FOR_TARGET" \
  && AC_MSG_ERROR([no acceptable c++ found in \$PATH])

dnl backup 
rtems_save_CXX=$CXX
rtems_save_CXXFLAGS=$CXXFLAGS

dnl temporarily set CXX
CXX=$CXX_FOR_TARGET

AC_PROG_CXX_WORKS
AC_PROG_CXX_GNU

if test $ac_cv_prog_gxx = yes; then
  GXX=yes
dnl Check whether -g works, even if CXXFLAGS is set, in case the package
dnl plays around with CXXFLAGS (such as to build both debugging and
dnl normal versions of a library), tasteless as that idea is.
  ac_test_CXXFLAGS="${CXXFLAGS+set}"
  ac_save_CXXFLAGS="$CXXFLAGS"
  CXXFLAGS=
  AC_PROG_CXX_G
  if test "$ac_test_CXXFLAGS" = set; then
    CXXFLAGS="$ac_save_CXXFLAGS"
  elif test $ac_cv_prog_cxx_g = yes; then
    CXXFLAGS="-g -O2"
  else
    CXXFLAGS="-O2"
  fi
else
  GXX=
  test "${CXXFLAGS+set}" = set || CXXFLAGS="-g"
fi

rtems_cv_prog_gxx=$ac_cv_prog_gxx
rtems_cv_prog_cxx_g=$ac_cv_prog_cxx_g
rtems_cv_prog_cxx_works=$ac_cv_prog_cxx_works
rtems_cv_prog_cxx_cross=$ac_cv_prog_cxx_cross

CXX=$rtems_save_CXX
CXXFLAGS=$rtems_save_CXXFLAGS

dnl restore initial values
unset ac_cv_prog_gxx
unset ac_cv_prog_cc_g
unset ac_cv_prog_cxx_works
unset ac_cv_prog_cxx_cross
])

dnl
dnl $Id$
dnl
dnl Set target tools
dnl
dnl 98/06/23 Ralf Corsepius	(corsepiu@faw.uni-ulm.de)
dnl		fixing cache/environment variable handling
dnl		adding checks for cygwin/egcs '\\'-bug
dnl		adding checks for ranlib/ar -s problem 
dnl
dnl 98/02/12 Ralf Corsepius	(corsepiu@faw.uni-ulm.de)
dnl

AC_DEFUN(RTEMS_GCC_PRINT,
[ case $host_os in
  *cygwin32*)
    dnl FIXME: Hack for cygwin/egcs reporting mixed '\\' and '/'
    dnl        Should be removed once cygwin/egcs reports '/' only
    $1=`$CC_FOR_TARGET --print-prog-name=$2 | sed -e 's%\\\\%/%g' `
    ;;
  *)
    $1=`$CC_FOR_TARGET --print-prog-name=$2`
    ;;
  esac
])

AC_DEFUN(RTEMS_PATH_TOOL,
[
AC_MSG_CHECKING([target's $2])
AC_CACHE_VAL(ac_cv_path_$1,:)
AC_MSG_RESULT([$ac_cv_path_$1])

if test -n "$ac_cv_path_$1"; then
  dnl retrieve the value from the cache
  $1=$ac_cv_path_$1
else
  dnl the cache was not set
  if test -z "[$]$1" ; then
    if test "$rtems_cv_prog_gcc" = "yes"; then
      # We are using gcc, ask it about its tool
      # NOTE: Necessary if gcc was configured to use the target's 
      # native tools or uses prefixes for gnutools (e.g. gas instead of as)
      RTEMS_GCC_PRINT($1,$2)
    fi
  else
    # The user set an environment variable.
    # Check whether it is an absolute path, otherwise AC_PATH_PROG 
    # will override the environment variable, which isn't what the user
    # intends
    AC_MSG_CHECKING([whether environment variable $1 is an absolute path])
    case "[$]$1" in
    /*) # valid
      AC_MSG_RESULT("yes")
    ;;
    *)  # invalid for AC_PATH_PROG
      AC_MSG_RESULT("no")
      AC_MSG_ERROR([***]
        [Environment variable $1 should ether]
	[be unset (preferred) or contain an absolute path])
    ;;
    esac
  fi

  AC_PATH_PROG($1,"$program_prefix"$2,$3)
fi
])

AC_DEFUN(RTEMS_CANONICALIZE_TOOLS,
[AC_REQUIRE([RTEMS_PROG_CC])dnl

dnl FIXME: What shall be done if these tools are not available?
  RTEMS_PATH_TOOL(AR_FOR_TARGET,ar,no)
  RTEMS_PATH_TOOL(AS_FOR_TARGET,as,no)
  RTEMS_PATH_TOOL(LD_FOR_TARGET,ld,no)
  RTEMS_PATH_TOOL(NM_FOR_TARGET,nm,no)

dnl special treatment of ranlib
  RTEMS_PATH_TOOL(RANLIB_FOR_TARGET,ranlib,no)
  if test "$RANLIB_FOR_TARGET" = "no"; then
    # ranlib wasn't found; check if ar -s is available
    RTEMS_AR_FOR_TARGET_S
    if test $rtems_cv_AR_FOR_TARGET_S = "yes" ; then
      dnl override RANLIB_FOR_TARGET's cache
      ac_cv_path_RANLIB_FOR_TARGET="$AR_FOR_TARGET -s"
      RANLIB_FOR_TARGET=$ac_cv_path_RANLIB_FOR_TARGET
    else
      AC_MSG_ERROR([***]
        [Can't figure out how to build a library index]
	[Nether ranlib nor ar -s seem to be available] )
    fi
  fi

dnl NOTE: These may not be available if not using gnutools
  RTEMS_PATH_TOOL(OBJCOPY_FOR_TARGET,objcopy,no)
  RTEMS_PATH_TOOL(SIZE_FOR_TARGET,size,no)
])

dnl
dnl $Id$
dnl

AC_DEFUN(RTEMS_AR_FOR_TARGET_S,
[
AC_CACHE_CHECK(whether $AR_FOR_TARGET -s works,
rtems_cv_AR_FOR_TARGET_S,
[
cat > conftest.$ac_ext <<EOF
int foo( int b ) 
{ return b; }
EOF
if AC_TRY_COMMAND($CC_FOR_TARGET -o conftest.o -c conftest.$ac_ext) \
  && AC_TRY_COMMAND($AR_FOR_TARGET -sr conftest.a conftest.o) \
  && test -s conftest.a ; \
then
  rtems_cv_AR_FOR_TARGET_S="yes"
else
  rtems_cv_AR_FOR_TARGET_S="no"
fi
  rm -f conftest*
])
])


dnl
dnl  $Id$
dnl 

dnl check for i386 gas supporting 16 bit mode
dnl     - binutils 2.9.1.0.7 and higher

AC_DEFUN(RTEMS_I386_GAS_CODE16,
  if test "${target_cpu}" = "i386"; then
    AC_CACHE_CHECK([for 16 bit mode assembler support],
      rtems_cv_prog_gas_code16,
      [cat > conftest.s << EOF
         .code16
         data32
         addr32
         lgdt 0
EOF
      if AC_TRY_COMMAND($AS_FOR_TARGET -o conftest.o conftest.s); then
        rtems_cv_prog_gas_code16=yes
      else
        rtems_cv_prog_gas_code16=no
      fi])
    RTEMS_GAS_CODE16="$rtems_cv_prog_gas_code16"
  fi
)


dnl
dnl  $Id$
dnl 
dnl Detect the Cygwin32 environment (unix under Win32)
dnl
dnl 98/06/16 David Fiddes   	(D.J.Fiddes@hw.ac.uk)
dnl				Hacked from automake-1.3

# Check to see if we're running under Cygwin32, without using
# AC_CANONICAL_*.  If so, set output variable CYGWIN32 to "yes".
# Otherwise set it to "no".

dnl RTEMS_CYGWIN32()
AC_DEFUN(RTEMS_CYGWIN32,
[AC_CACHE_CHECK(for Cygwin32 environment, rtems_cv_cygwin32,
[AC_TRY_COMPILE(,[return __CYGWIN32__;],
rtems_cv_cygwin32=yes, rtems_cv_cygwin32=no)
rm -f conftest*])
CYGWIN32=
test "$rtems_cv_cygwin32" = yes && CYGWIN32=yes])


dnl
dnl  $Id$
dnl 
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


dnl
dnl $Id$
dnl
dnl Check for System V IPC calls used by Unix simulators
dnl
dnl 98/07/17 Dario Alcocer     alcocer@netcom.com
dnl 	     Ralf Corsepius    corsepiu@faw.uni-ulm.de
dnl
dnl Note: $host_os should probably *not* ever be used here to
dnl determine if host supports System V IPC calls, since some
dnl (e.g. FreeBSD 2.x) are configured by default to include only
dnl a subset of the System V IPC calls.  Therefore, to make sure
dnl all of the required calls are found, test for each call explicitly.
dnl
dnl All of the calls use IPC_PRIVATE, so tests will not unintentionally
dnl modify any existing key sets.  See the man pages for semget, shmget, 
dnl msgget, semctl, shmctl and msgctl for details.

AC_DEFUN(RTEMS_SYSV_SEM,
[AC_REQUIRE([RTEMS_PROG_CC]) 
AC_REQUIRE([AC_CANONICAL_HOST])
AC_CACHE_CHECK(whether $RTEMS_HOST supports System V semaphores,
rtems_cv_sysv_sem,
[
AC_TRY_RUN([
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
int main () {
#if !defined(sun) 
  union semun arg ;
#else
  union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
  } arg;
#endif
  int id=semget(IPC_PRIVATE,1,IPC_CREAT|0400);
  if (id == -1)
    exit(1);
  arg.val = 0; /* avoid implicit type cast to union */
  if (semctl(id, 0, IPC_RMID, arg) == -1)
    exit(1);
  exit(0);
}
],
rtems_cv_sysv_sem="yes", rtems_cv_sysv_sem="no", :)
])
])

AC_DEFUN(RTEMS_SYSV_SHM,
[AC_REQUIRE([RTEMS_PROG_CC]) 
AC_REQUIRE([AC_CANONICAL_HOST])
AC_CACHE_CHECK(whether $RTEMS_HOST supports System V shared memory,
rtems_cv_sysv_shm,
[
AC_TRY_RUN([
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
int main () {
  int id=shmget(IPC_PRIVATE,1,IPC_CREAT|0400);
  if (id == -1)
    exit(1);
  if (shmctl(id, IPC_RMID, 0) == -1)
    exit(1);
  exit(0);
}
],
rtems_cv_sysv_shm="yes", rtems_cv_sysv_shm="no", :)
])
])

AC_DEFUN(RTEMS_SYSV_MSG,
[AC_REQUIRE([RTEMS_PROG_CC]) 
AC_REQUIRE([AC_CANONICAL_HOST])
AC_CACHE_CHECK(whether $RTEMS_HOST supports System V messages,
rtems_cv_sysv_msg,
[
AC_TRY_RUN([
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
int main () {
  int id=msgget(IPC_PRIVATE,IPC_CREAT|0400);
  if (id == -1)
    exit(1);
  if (msgctl(id, IPC_RMID, 0) == -1)
    exit(1);
  exit(0);
}
],
rtems_cv_sysv_msg="yes", rtems_cv_sysv_msg="no", :)
])
])

dnl
dnl $Id$
dnl

dnl RTEMS_CHECK_MAKEFILE(path)
dnl Search for Makefile.in's within the directory starting
dnl at path and append an entry for Makefile to global variable 
dnl "makefiles" (from configure.in) for each Makefile.in found
dnl 
AC_DEFUN(RTEMS_CHECK_MAKEFILE,
[RTEMS_CHECK_FILES_IN($1,Makefile,makefiles)
])

dnl
dnl $Id$
dnl

dnl RTEMS_CHECK_FILES_IN(path,file,var)
dnl path .. path relative to srcdir, where to start searching for files
dnl file .. name of the files to search for
dnl var  .. shell variable to append files found

AC_DEFUN(RTEMS_CHECK_FILES_IN,
[
AC_MSG_CHECKING(for $2.in in $1)
if test -d $srcdir/$1; then
  rtems_av_save_dir=`pwd`;
  cd $srcdir;
  rtems_av_tmp=`find $1 -name "$2.in" -print | sed "s/$2\.in/%/" | sort | sed "s/%/$2/"`
  $3="$$3 $rtems_av_tmp";
  cd $rtems_av_save_dir;
  AC_MSG_RESULT(done)
else
  AC_MSG_RESULT(no)
fi
])


