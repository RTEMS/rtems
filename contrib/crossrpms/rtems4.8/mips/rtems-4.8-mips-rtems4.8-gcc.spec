#
# Please send bugfixes or comments to
# 	http://www.rtems.org/bugzilla
#

%define _prefix			/opt/rtems-4.8
%define _infodir		%{_prefix}/info
%define _mandir			%{_prefix}/man

%ifos cygwin cygwin32 mingw mingw32
%define _exeext .exe
%else
%define _exeext %{nil}
%endif


%define gcc_pkgvers 4.1.2
%define gcc_version 4.1.2
%define gcc_rpmvers %{expand:%(echo "4.1.2" | tr - _ )}

%define newlib_version		1.15.0
%define gccnewlib_version	gcc%{gcc_version}newlib%{newlib_version}

Name:         	rtems-4.8-mips-rtems4.8-gcc
Summary:      	mips-rtems4.8 gcc

Group:	      	Development/Tools
Version:        %{gcc_rpmvers}
Release:      	12%{?dist}
License:      	GPL
URL:		http://gcc.gnu.org
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

%define _use_internal_dependency_generator 0

%if "%{gcc_version}" >= "4.2.0"
BuildRequires:	flex bison
%endif

BuildRequires:	texinfo >= 4.2
BuildRequires:	rtems-4.8-mips-rtems4.8-binutils

Requires:	rtems-4.8-gcc-common
Requires:	rtems-4.8-mips-rtems4.8-binutils
Requires:	rtems-4.8-mips-rtems4.8-newlib = %{newlib_version}-%{release}


%if "%{gcc_version}" >= "3.4"
%define gcclib %{_libdir}/gcc
%define gccexec %{_libexecdir}/gcc
%else
%define gcclib %{_libdir}/gcc-lib
%define gccexec %{_libdir}/gcc-lib
%endif

Source0: 	ftp://ftp.gnu.org/gnu/gcc/gcc-%{gcc_version}/gcc-core-%{gcc_pkgvers}.tar.bz2
%if "%{gcc_version}" == "4.0.3"
Patch0:		gcc-core-4.0.3-rtems-20060822.diff
%endif
%if "%{gcc_version}" == "4.1.1"
Patch0:		gcc-core-4.1.1-rtems4.8-20070201.diff
%endif
%if "%{gcc_version}" == "4.1.2"
Patch0:		gcc-core-4.1.2-rtems4.8-20070405.diff
%endif
%{?_without_sources:NoSource:	0}

Source1: 	ftp://ftp.gnu.org/gnu/gcc/gcc-%{gcc_version}/gcc-g++-%{gcc_pkgvers}.tar.bz2
%{?_without_sources:NoSource:	1}

Source50:	ftp://sources.redhat.com/pub/newlib/newlib-%{newlib_version}.tar.gz
%if "%{newlib_version}" == "1.15.0"
Patch50:	newlib-1.15.0-rtems4.8-20070413.diff
%endif
%{?_without_sources:NoSource:	50}

%description
Cross gcc for mips-rtems4.8.

%prep
%setup -c -T -n %{name}-%{version}

%setup -q -T -D -n %{name}-%{version} -a0
%{?PATCH0:%patch0 -p0}

%setup -q -T -D -n %{name}-%{version} -a1
%{?PATCH1:%patch1 -p0}





%setup -q -T -D -n %{name}-%{version} -a50
cd newlib-%{newlib_version}
%{?PATCH50:%patch50 -p1}
cd ..
  # Copy the C library into gcc's source tree
  ln -s ../newlib-%{newlib_version}/newlib gcc-%{gcc_pkgvers}

%if "%{gcc_version}" < "4.1.0"
  sed -e 's/\(version_string.* = \"[^\"]*\)/\1 (RTEMS gcc-%{gcc_version}\/newlib-%{newlib_version}-%release)/' \
  gcc-%{gcc_pkgvers}/gcc/version.c > gcc-%{gcc_pkgvers}/gcc/version.c~
%else
  sed -e 's/^#define VERSUFFIX.*$/#define VERSUFFIX \" (RTEMS gcc-%{gcc_version}\/newlib-%{newlib_version}-%{release})\"/' \
    gcc-%{gcc_pkgvers}/gcc/version.c > gcc-%{gcc_pkgvers}/gcc/version.c~
%endif
  mv gcc-%{gcc_pkgvers}/gcc/version.c~ gcc-%{gcc_pkgvers}/gcc/version.c


  # Fix timestamps
  cd gcc-%{gcc_pkgvers}
  contrib/gcc_update --touch
  cd ..
%build
  mkdir -p build

  cd build

  languages="c"
  languages="$languages,c++"
  export PATH="%{_bindir}:${PATH}"

  CC="%{__cc} ${RPM_OPT_FLAGS}" \
  ../gcc-%{gcc_pkgvers}/configure \
    --prefix=%{_prefix} \
    --bindir=%{_bindir} \
    --exec_prefix=%{_exec_prefix} \
    --includedir=%{_includedir} \
    --libdir=%{_libdir} \
    --libexecdir=%{_libexecdir} \
    --mandir=%{_mandir} \
    --infodir=%{_infodir} \
    --datadir=%{_datadir} \
    --build=%_build --host=%_host \
    --target=mips-rtems4.8 \
    --with-gnu-as --with-gnu-ld --verbose \
    --with-newlib \
    --with-system-zlib \
    --disable-nls --without-included-gettext \
    --disable-win32-registry \
    --enable-version-specific-runtime-libs \
    --enable-threads \
    --enable-languages="$languages" $optargs

%if "%_host" != "%_build"
  # Bug in gcc-3.2.1:
  # Somehow, gcc doesn't get syslimits.h right for Cdn-Xs
  mkdir -p gcc/include
  cp ../gcc-%{gcc_pkgvers}/gcc/gsyslimits.h gcc/include/syslimits.h
%endif

  make all
  make info
  cd ..

%install
  export PATH="%{_bindir}:${PATH}"
  rm -rf $RPM_BUILD_ROOT

  cd build

  make DESTDIR=$RPM_BUILD_ROOT install
  cd ..

  cd build/mips-rtems4.8/newlib
  make DESTDIR=$RPM_BUILD_ROOT install-info
  cd ../../..

%if "%{gcc_version}" <= "4.1.2"
# Misplaced header file
  if test -f $RPM_BUILD_ROOT%{_includedir}/mf-runtime.h; then
    mv $RPM_BUILD_ROOT%{_includedir}/mf-runtime.h \
      $RPM_BUILD_ROOT%{gcclib}/mips-rtems4.8/%{gcc_version}/include/
  fi
%endif

  # host library
  rm -f  ${RPM_BUILD_ROOT}%{_libdir}/libiberty.a

  # We use the version from binutils
  rm -f $RPM_BUILD_ROOT%{_bindir}/mips-rtems4.8-c++filt%{_exeext}


  # We don't ship info/dir
  rm -f $RPM_BUILD_ROOT%{_infodir}/dir
  touch $RPM_BUILD_ROOT%{_infodir}/dir


%if "%{gcc_version}" >= "3.4"
  # Bug in gcc-3.4.0pre
  rm -f $RPM_BUILD_ROOT%{_bindir}/mips-rtems4.8-mips-rtems4.8-gcjh%{_exeext}
%endif

%if "%{gcc_version}" >= "3.3"
  # Bug in gcc-3.3.x/gcc-3.4.x: Despite we don't need fixincludes, it installs
  # the fixinclude-install-tools
  rm -rf ${RPM_BUILD_ROOT}%{gcclib}/mips-rtems4.8/%{gcc_version}/install-tools
  rm -rf ${RPM_BUILD_ROOT}%{gccexec}/mips-rtems4.8/%{gcc_version}/install-tools
%endif

  # Collect multilib subdirectories
  f=`build/gcc/xgcc -Bbuild/gcc/ --print-multi-lib | sed -e 's,;.*$,,'`

  echo "%defattr(-,root,root,-)" > build/files.newlib
  TGTDIR="%{_exec_prefix}/mips-rtems4.8/lib"
  for i in $f; do
    case $i in
    \.) echo "%dir ${TGTDIR}" >> build/files.newlib
      ;;
    *)  echo "%dir ${TGTDIR}/$i" >> build/files.newlib
      ;;
    esac
  done

  rm -f dirs ;
  echo "%defattr(-,root,root,-)" >> dirs
  echo "%dir %{_prefix}" >> dirs
  echo "%dir %{_libdir}" >> dirs
%if "%{gcc_version}" >= "3.4"
  echo "%dir %{_libexecdir}" >> dirs
%endif
  echo "%dir %{gcclib}" >> dirs
  echo "%dir %{gcclib}/mips-rtems4.8" >> dirs

  TGTDIR="%{gcclib}/mips-rtems4.8/%{gcc_version}"
  for i in $f; do
    case $i in
    \.) echo "%dir ${TGTDIR}" >> dirs
      ;;
    *)  echo "%dir ${TGTDIR}/$i" >> dirs
      ;;
    esac
  done

  # Collect files to go into different packages
  cp dirs build/files.gcc
  cp dirs build/files.g77
  cp dirs build/files.gfortran
  cp dirs build/files.objc
  cp dirs build/files.gcj
  cp dirs build/files.g++

  TGTDIR="%{gcclib}/mips-rtems4.8/%{gcc_version}"
  f=`find ${RPM_BUILD_ROOT}${TGTDIR} ! -type d -print | sed -e "s,^$RPM_BUILD_ROOT,,g"`;
  for i in $f; do
    case $i in
    *lib*.la) rm ${RPM_BUILD_ROOT}/$i ;; # ignore: gcc produces bogus libtool libs
    *f771) ;;
    *f951) ;;
    *cc1) ;;
    *cc1obj) ;;
    *cc1plus) ;; # ignore: explicitly put into rpm elsewhere
    *collect2) ;;
    *libobjc*) echo "$i" >> build/files.objc ;;
    *include/objc*) ;;
    *include/g++*);;
    *include/c++*);;
    *adainclude*);;
    *adalib*);;
    *gnat1);;
    *jc1) ;;
    *jvgenmain) ;;
    */libgfortran*.*) echo "$i" >> build/files.gfortran ;;
    */libstdc++.*) echo "$i" >> build/files.g++ ;;
    */libsupc++.*) echo "$i" >> build/files.g++ ;;
    *) echo "$i" >> build/files.gcc ;;
    esac
  done

  TGTDIR="%{_exec_prefix}/mips-rtems4.8/lib"
  f=`find ${RPM_BUILD_ROOT}${TGTDIR} ! -type d -print | sed -e "s,^$RPM_BUILD_ROOT,,g"`;
  for i in $f; do
    case $i in
    *lib*.la) rm ${RPM_BUILD_ROOT}/$i;; # ignore - gcc produces bogus libtool libs
    *libiberty.a) rm ${RPM_BUILD_ROOT}/$i ;; # ignore - GPL'ed
# all other files belong to newlib
    *) echo "$i" >> build/files.newlib ;; 
    esac
  done
# Extract %%__os_install_post into os_install_post~
cat << \EOF > os_install_post~
%__os_install_post
EOF

# Generate customized brp-*scripts
cat os_install_post~ | while read a x y; do
case $a in
# Prevent brp-strip* from trying to handle foreign binaries
*/brp-strip*)
  b=$(basename $a)
  sed -e 's,find $RPM_BUILD_ROOT,find $RPM_BUILD_ROOT%_bindir $RPM_BUILD_ROOT%_libexecdir,' $a > $b
  chmod a+x $b
  ;;
# Fix up brp-compress to handle %%_prefix != /usr
*/brp-compress*)
  b=$(basename $a)
  sed -e 's,\./usr/,.%{_prefix}/,g' < $a > $b
  chmod a+x $b
  ;;
esac
done

sed -e 's,^[ ]*/usr/lib/rpm.*/brp-strip,./brp-strip,' \
  -e 's,^[ ]*/usr/lib/rpm.*/brp-compress,./brp-compress,' \
< os_install_post~ > os_install_post 
%define __os_install_post . ./os_install_post


cat << EOF > %{_builddir}/%{name}-%{gcc_rpmvers}/find-provides
#!/bin/sh
grep -E -v '^${RPM_BUILD_ROOT}%{_exec_prefix}/mips-rtems4.8/(lib|include|sys-root)' \
  | grep -v '^${RPM_BUILD_ROOT}%{gcclib}/mips-rtems4.8/' | %__find_provides
EOF
chmod +x %{_builddir}/%{name}-%{gcc_rpmvers}/find-provides
%define __find_provides %{_builddir}/%{name}-%{gcc_rpmvers}/find-provides

cat << EOF > %{_builddir}/%{name}-%{gcc_rpmvers}/find-requires
#!/bin/sh
grep -E -v '^${RPM_BUILD_ROOT}%{_exec_prefix}/mips-rtems4.8/(lib|include|sys-root)' \
  | grep -v '^${RPM_BUILD_ROOT}%{gcclib}/mips-rtems4.8/' | %__find_requires
EOF
chmod +x %{_builddir}/%{name}-%{gcc_rpmvers}/find-requires
%define __find_requires %{_builddir}/%{name}-%{gcc_rpmvers}/find-requires

%clean
  rm -rf $RPM_BUILD_ROOT

# ==============================================================
# rtems-4.8-mips-rtems4.8-gcc
# ==============================================================
# %package -n rtems-4.8-mips-rtems4.8-gcc
# Summary:        GNU cc compiler for mips-rtems4.8
# Group:          Development/Tools
# Version:        %{gcc_rpmvers}
# Requires:       rtems-4.8-mips-rtems4.8-binutils
# Requires:       rtems-4.8-mips-rtems4.8-newlib = %{newlib_version}-%{release}
# License:	GPL

# %if %build_infos
# Requires:      rtems-4.8-gcc-common
# %endif

%description -n rtems-4.8-mips-rtems4.8-gcc
GNU cc compiler for mips-rtems4.8.

%files -n rtems-4.8-mips-rtems4.8-gcc -f build/files.gcc
%defattr(-,root,root)
%dir %{_mandir}
%dir %{_mandir}/man1
%{_mandir}/man1/mips-rtems4.8-gcc.1*
%if "%{gcc_version}" >= "3.4"
%{_mandir}/man1/mips-rtems4.8-cpp.1*
%{_mandir}/man1/mips-rtems4.8-gcov.1*
%endif

%dir %{_bindir}
%{_bindir}/mips-rtems4.8-cpp%{_exeext}
%{_bindir}/mips-rtems4.8-gcc%{_exeext}
%if "%{gcc_version}" >= "3.3"
%{_bindir}/mips-rtems4.8-gcc-%{gcc_version}%{_exeext}
%endif
%{_bindir}/mips-rtems4.8-gcov%{_exeext}
%{_bindir}/mips-rtems4.8-gccbug

%dir %{gcclib}/mips-rtems4.8/%{gcc_version}/include
%if "%{gcc_version}" > "4.0.3"
%if "mips-rtems4.8" != "bfin-rtems4.7"
%dir %{gcclib}/mips-rtems4.8/%{gcc_version}/include/ssp
%endif
%endif

%dir %{gccexec}
%dir %{gccexec}/mips-rtems4.8
%dir %{gccexec}/mips-rtems4.8/%{gcc_version}
%{gccexec}/mips-rtems4.8/%{gcc_version}/cc1%{_exeext}
%{gccexec}/mips-rtems4.8/%{gcc_version}/collect2%{_exeext}

# ==============================================================
# rtems-4.8-rtems4.7-base-gcc
# ==============================================================
%package -n rtems-4.8-gcc-common
Summary:	Base package for rtems gcc and newlib C Library
Group:          Development/Tools
Version:        %{gcc_rpmvers}
License:	GPL

Requires(post): 	/sbin/install-info
Requires(preun):	/sbin/install-info

Provides:	rtems-4.8-rtems4.7-base-gcc = %{gcc_version}-%{release}
Obsoletes:	rtems-4.8-rtems4.7-base-gcc < %{gcc_rpmvers}-%{release}
Provides:	rtems-4.8-rtems-base-gcc = %{gcc_version}-%{release}
Obsoletes:	rtems-4.8-rtems-base-gcc < %{gcc_rpmvers}-%{release}

%description -n rtems-4.8-gcc-common

GCC files that are shared by all targets.

%files -n rtems-4.8-gcc-common
%defattr(-,root,root)
%dir %{_infodir}
%ghost %{_infodir}/dir
%{_infodir}/cpp.info*
%{_infodir}/cppinternals.info*
%{_infodir}/gcc.info*
%{_infodir}/gccint.info*
%if "%{gcc_version}" >= "3.4"
%{_infodir}/gccinstall.info*
%endif

%dir %{_mandir}
%if "%{gcc_version}" < "3.4"
%dir %{_mandir}/man1
%{_mandir}/man1/cpp.1*
%{_mandir}/man1/gcov.1*
%endif
%dir %{_mandir}/man7
%{_mandir}/man7/fsf-funding.7*
%{_mandir}/man7/gfdl.7*
%{_mandir}/man7/gpl.7*

%post -n rtems-4.8-gcc-common
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/cpp.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/cppinternals.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/gcc.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/gccint.info.gz || :
%if "%{gcc_version}" >= "3.4"
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/gccinstall.info.gz || :
%endif

%preun -n rtems-4.8-gcc-common
if [ $1 -eq 0 ]; then
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/cpp.info.gz || :
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/cppinternals.info.gz || :
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/gcc.info.gz || :
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/gccint.info.gz || :
%if "%{gcc_version}" >= "3.4"
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/gccinstall.info.gz || :
%endif
fi

# ==============================================================
# rtems-4.8-mips-rtems4.8-gcc-c++
# ==============================================================
%package -n rtems-4.8-mips-rtems4.8-gcc-c++
Summary:	GCC c++ compiler for mips-rtems4.8
Group:		Development/Tools
Version:        %{gcc_rpmvers}
License:	GPL

Provides:	rtems-4.8-mips-rtems4.8-c++ = %{gcc_rpmvers}-%{release}
Obsoletes:	rtems-4.8-mips-rtems4.8-c++ < %{gcc_rpmvers}-%{release}

Requires:       rtems-4.8-gcc-common
Requires:       rtems-4.8-mips-rtems4.8-gcc = %{gcc_rpmvers}-%{release}

%description -n rtems-4.8-mips-rtems4.8-gcc-c++
GCC c++ compiler for mips-rtems4.8.

%files -n rtems-4.8-mips-rtems4.8-gcc-c++ -f build/files.g++
%defattr(-,root,root)
%{_mandir}/man1/mips-rtems4.8-g++.1*

%{_bindir}/mips-rtems4.8-c++%{_exeext}
%{_bindir}/mips-rtems4.8-g++%{_exeext}

%dir %{gccexec}
%dir %{gccexec}/mips-rtems4.8
%dir %{gccexec}/mips-rtems4.8/%{gcc_version}
%{gccexec}/mips-rtems4.8/%{gcc_version}/cc1plus%{_exeext}

%dir %{gcclib}/mips-rtems4.8/%{gcc_version}/include
%if "%{gcc_version}" >= "3.2"
%{gcclib}/mips-rtems4.8/%{gcc_version}/include/c++
%else
%{gcclib}/mips-rtems4.8/%{gcc_version}/include/g++
%endif



# ==============================================================
# rtems-4.8-mips-rtems4.8-newlib
# ==============================================================
%package -n rtems-4.8-mips-rtems4.8-newlib
Summary:      	C Library (newlib) for mips-rtems4.8
Group: 		Development/Tools
License:	Distributable
Version:	%{newlib_version}

Provides:	rtems-4.8-mips-rtems4.8-libc = %{newlib_version}-%{release}
Obsoletes:	rtems-4.8-mips-rtems4.8-libc < %{newlib_version}-%{release}

Requires:	rtems-4.8-newlib-common

%description -n rtems-4.8-mips-rtems4.8-newlib
Newlib C Library for mips-rtems4.8.

%files -n rtems-4.8-mips-rtems4.8-newlib -f build/files.newlib
%defattr(-,root,root)
%dir %{_prefix}
%dir %{_exec_prefix}/mips-rtems4.8
%{_exec_prefix}/mips-rtems4.8/include

# ==============================================================
# rtems-4.8-newlib-common
# ==============================================================
%package -n rtems-4.8-newlib-common
Summary:	Base package for RTEMS newlib C Library
Group:          Development/Tools
Version:        %{newlib_version}
License:	Distributable

Provides:	rtems-4.8-rtems4.7-base-newlib = %{newlib_version}-%{release}
Obsoletes:	rtems-4.8-rtems4.7-base-newlib < %{newlib_version}-%{release}
Provides:	rtems-4.8-rtems-base-newlib = %{newlib_version}-%{release}
Obsoletes:	rtems-4.8-rtems-base-newlib < %{newlib_version}-%{release}

Provides:	rtems-4.8-rtems4.7-base-libc = %{newlib_version}-%{release}
Obsoletes:	rtems-4.8-rtems4.7-base-libc < %{newlib_version}-%{release}
Provides:	rtems-4.8-rtems-base-libc = %{newlib_version}-%{release}
Obsoletes:	rtems-4.8-rtems-base-libc < %{newlib_version}-%{release}


Requires(post): 	/sbin/install-info
Requires(preun):	/sbin/install-info

%description -n rtems-4.8-newlib-common
newlib files that are shared by all targets.

%files -n rtems-4.8-newlib-common
%defattr(-,root,root)
%dir %{_infodir}
%ghost %{_infodir}/dir
%{_infodir}/libc.info*
%{_infodir}/libm.info*

%post -n rtems-4.8-newlib-common
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/libc.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/libm.info.gz || :

%preun -n rtems-4.8-newlib-common
if [ $1 -eq 0 ]; then
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/libc.info.gz || :
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/libm.info.gz || :
fi

