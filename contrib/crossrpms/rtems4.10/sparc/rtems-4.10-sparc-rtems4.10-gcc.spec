#
# Please send bugfixes or comments to
# 	http://www.rtems.org/bugzilla
#

%define _prefix                 /opt/rtems-4.10
%define _exec_prefix            %{_prefix}
%define _bindir                 %{_exec_prefix}/bin
%define _sbindir                %{_exec_prefix}/sbin
%define _libexecdir             %{_exec_prefix}/libexec
%define _datarootdir            %{_prefix}/share
%define _datadir                %{_datarootdir}
%define _sysconfdir             %{_prefix}/etc
%define _sharedstatedir         %{_prefix}/com
%define _localstatedir          %{_prefix}/var
%define _includedir             %{_prefix}/include
%define _libdir                 %{_exec_prefix}/%{_lib}
%define _mandir                 %{_datarootdir}/man
%define _infodir                %{_datarootdir}/info
%define _localedir              %{_datarootdir}/locale

%ifos cygwin cygwin32 mingw mingw32
%define _exeext .exe
%else
%define _exeext %{nil}
%endif

%ifos cygwin cygwin32
%define optflags -O3 -pipe -march=i486 -funroll-loops
%define _libdir			%{_exec_prefix}/lib
%define debug_package		%{nil}
%endif

%if "%{_build}" != "%{_host}"
%define _host_rpmprefix rtems-4.10-%{_host}-
%else
%define _host_rpmprefix %{nil}
%endif


%define gcc_pkgvers 4.3.2
%define gcc_version 4.3.2
%define gcc_rpmvers %{expand:%(echo "4.3.2" | tr - _ )}

%define newlib_pkgvers		1.16.0.cvs.20081205
%define newlib_version		1.16.0
%define gccnewlib_version	gcc%{gcc_version}newlib%{newlib_version}

%define mpfr_version	2.3.1

Name:         	rtems-4.10-sparc-rtems4.10-gcc
Summary:      	sparc-rtems4.10 gcc

Group:	      	Development/Tools
Version:        %{gcc_rpmvers}
Release:      	17%{?dist}
License:      	GPL
URL:		http://gcc.gnu.org
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

%define _use_internal_dependency_generator 0

BuildRequires:  %{_host_rpmprefix}gcc

%if "%{gcc_version}" >= "4.3.0"
BuildRequires:  gmp-devel >= 4.1
%if "%{_build}" != "%{_host}"
BuildRequires:  %{_host_rpmprefix}gmp-devel
BuildRequires:  %{_host_rpmprefix}mpfr-devel
%endif
%if 0%{?fedora} >= 8
BuildRequires:  mpfr-devel >= 2.3.0
%endif
%if "%{?suse}" > "10.3"
BuildRequires:  mpfr-devel >= 2.3.0
%endif
# These distros ship an insufficient mpfr
%{?el4:%define 	_build_mpfr 	1}
%{?suse10_2:%define 	_build_mpfr 	1}
%{?suse10_3:%define 	_build_mpfr 	1}
%endif

%if "%{_build}" != "%{_host}"
BuildRequires:  rtems-4.10-sparc-rtems4.10-gcc
%endif

%if "%{gcc_version}" >= "4.2.0"
BuildRequires:	flex bison
%endif


BuildRequires:	texinfo >= 4.2
BuildRequires:	rtems-4.10-sparc-rtems4.10-binutils

Requires:	rtems-4.10-gcc-common
Requires:	rtems-4.10-sparc-rtems4.10-binutils
Requires:	rtems-4.10-sparc-rtems4.10-newlib = %{newlib_version}-%{release}


%if "%{gcc_version}" >= "3.4"
%define gcclib %{_libdir}/gcc
%define gccexec %{_libexecdir}/gcc
%else
%define gcclib %{_libdir}/gcc-lib
%define gccexec %{_libdir}/gcc-lib
%endif

%if "%{gcc_version}" == "4.3.2"
Source0:	ftp://ftp.gnu.org/pub/gnu/gcc/%{gcc_pkgvers}/gcc-core-%{gcc_pkgvers}.tar.bz2
Patch0:		ftp://ftp.rtems.org/pub/rtems/SOURCES/4.10/gcc-core-4.3.2-rtems4.10-20081205.diff
%endif
%{?_without_sources:NoSource:	0}

%if "%{gcc_version}" == "4.3.2" 
Source1:        ftp://ftp.gnu.org/pub/gnu/gcc/%{gcc_pkgvers}/gcc-g++-%{gcc_pkgvers}.tar.bz2
%endif
%{?_without_sources:NoSource:	1}

%if "%{newlib_version}" == "1.16.0"
Source50:	ftp://sources.redhat.com/pub/newlib/newlib-%{newlib_pkgvers}.tar.gz
Patch50:	ftp://ftp.rtems.org/pub/rtems/SOURCES/4.10/newlib-%{newlib_pkgvers}-rtems4.10-20081205.diff
%endif
%{?_without_sources:NoSource:	50}

%if "%{gcc_version}" >= "4.3.0"
Source60:    http://www.mpfr.org/mpfr-current/mpfr-%{mpfr_version}.tar.bz2
%endif

%description
Cross gcc for sparc-rtems4.10.

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

%if 0%{?_build_mpfr}
%setup -q -T -D -n %{name}-%{version} -a60
%{?PATCH60:%patch60 -p1}
  # Build mpfr one-tree style
  ln -s ../mpfr-%{mpfr_version} gcc-%{gcc_pkgvers}/mpfr
%endif

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
%if "%{_build}" != "%{_host}"
  CFLAGS_FOR_BUILD="-g -O2 -Wall" \
  CC="%{_host}-gcc ${RPM_OPT_FLAGS}" \
%else
# gcc is not ready to be compiled with -std=gnu99
  CC=$(echo "%{__cc} ${RPM_OPT_FLAGS}" | sed -e 's,-std=gnu99 ,,') \
%endif
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
    --target=sparc-rtems4.10 \
    --disable-libstdcxx-pch \
    --with-gnu-as --with-gnu-ld --verbose \
    --with-newlib \
    --with-system-zlib \
    --disable-nls --without-included-gettext \
    --disable-win32-registry \
    --enable-version-specific-runtime-libs \
    --enable-threads \
    --enable-newlib-io-c99-formats \
    --enable-languages="$languages" $optargs

%if "%_host" != "%_build"
  # Bug in gcc-3.2.1:
  # Somehow, gcc doesn't get syslimits.h right for Cdn-Xs
  mkdir -p gcc/include
  cp ../gcc-%{gcc_pkgvers}/gcc/gsyslimits.h gcc/include/syslimits.h
%endif

  make %{?_smp_mflags} all
  make info
  cd ..

%install
  export PATH="%{_bindir}:${PATH}"
  rm -rf $RPM_BUILD_ROOT

  cd build

  make DESTDIR=$RPM_BUILD_ROOT install
  cd ..

  cd build/sparc-rtems4.10/newlib
  make DESTDIR=$RPM_BUILD_ROOT install-info
  cd ../../..

%if "%{gcc_version}" <= "4.1.2"
# Misplaced header file
  if test -f $RPM_BUILD_ROOT%{_includedir}/mf-runtime.h; then
    mv $RPM_BUILD_ROOT%{_includedir}/mf-runtime.h \
      $RPM_BUILD_ROOT%{gcclib}/sparc-rtems4.10/%{gcc_version}/include/
  fi
%endif

  # host library
  rm -f  ${RPM_BUILD_ROOT}%{_libdir}/libiberty.a

  # We use the version from binutils
  rm -f $RPM_BUILD_ROOT%{_bindir}/sparc-rtems4.10-c++filt%{_exeext}


  # We don't ship info/dir
  rm -f $RPM_BUILD_ROOT%{_infodir}/dir
  touch $RPM_BUILD_ROOT%{_infodir}/dir


%if "%{gcc_version}" >= "3.4"
  # Bug in gcc-3.4.0pre
  rm -f $RPM_BUILD_ROOT%{_bindir}/sparc-rtems4.10-sparc-rtems4.10-gcjh%{_exeext}
%endif

%if "%{gcc_version}" >= "3.3"
  # Bug in gcc-3.3.x/gcc-3.4.x: Despite we don't need fixincludes, it installs
  # the fixinclude-install-tools
  rm -rf ${RPM_BUILD_ROOT}%{gcclib}/sparc-rtems4.10/%{gcc_version}/install-tools
  rm -rf ${RPM_BUILD_ROOT}%{gccexec}/sparc-rtems4.10/%{gcc_version}/install-tools
%endif

  # Collect multilib subdirectories
  f=`build/gcc/xgcc -Bbuild/gcc/ --print-multi-lib | sed -e 's,;.*$,,'`

  echo "%defattr(-,root,root,-)" > build/files.newlib
  TGTDIR="%{_exec_prefix}/sparc-rtems4.10/lib"
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
  echo "%dir %{gcclib}/sparc-rtems4.10" >> dirs

  TGTDIR="%{gcclib}/sparc-rtems4.10/%{gcc_version}"
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

  TGTDIR="%{gcclib}/sparc-rtems4.10/%{gcc_version}"
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

  TGTDIR="%{_exec_prefix}/sparc-rtems4.10/lib"
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
grep -E -v '^${RPM_BUILD_ROOT}%{_exec_prefix}/sparc-rtems4.10/(lib|include|sys-root)' \
  | grep -v '^${RPM_BUILD_ROOT}%{gcclib}/sparc-rtems4.10/' | %__find_provides
EOF
chmod +x %{_builddir}/%{name}-%{gcc_rpmvers}/find-provides
%define __find_provides %{_builddir}/%{name}-%{gcc_rpmvers}/find-provides

cat << EOF > %{_builddir}/%{name}-%{gcc_rpmvers}/find-requires
#!/bin/sh
grep -E -v '^${RPM_BUILD_ROOT}%{_exec_prefix}/sparc-rtems4.10/(lib|include|sys-root)' \
  | grep -v '^${RPM_BUILD_ROOT}%{gcclib}/sparc-rtems4.10/' | %__find_requires
EOF
chmod +x %{_builddir}/%{name}-%{gcc_rpmvers}/find-requires
%define __find_requires %{_builddir}/%{name}-%{gcc_rpmvers}/find-requires

%ifnarch noarch
# Extract %%__debug_install_post into debug_install_post~
cat << \EOF > debug_install_post~
%__debug_install_post
EOF

# Generate customized debug_install_post script
cat debug_install_post~ | while read a x y; do
case $a in
# Prevent find-debuginfo.sh* from trying to handle foreign binaries
*/find-debuginfo.sh)
  b=$(basename $a)
  sed -e 's,find "$RPM_BUILD_ROOT" !,find "$RPM_BUILD_ROOT"%_bindir "$RPM_BUILD_ROOT"%_libexecdir !,' $a > $b
  chmod a+x $b
  ;;
esac
done

sed -e 's,^[ ]*/usr/lib/rpm/find-debuginfo.sh,./find-debuginfo.sh,' \
< debug_install_post~ > debug_install_post 
%define __debug_install_post . ./debug_install_post

%endif

%clean
  rm -rf $RPM_BUILD_ROOT

# ==============================================================
# rtems-4.10-sparc-rtems4.10-gcc
# ==============================================================
# %package -n rtems-4.10-sparc-rtems4.10-gcc
# Summary:        GNU cc compiler for sparc-rtems4.10
# Group:          Development/Tools
# Version:        %{gcc_rpmvers}
# Requires:       rtems-4.10-sparc-rtems4.10-binutils
# Requires:       rtems-4.10-sparc-rtems4.10-newlib = %{newlib_version}-%{release}
# License:	GPL

# %if %build_infos
# Requires:      rtems-4.10-gcc-common
# %endif

%description -n rtems-4.10-sparc-rtems4.10-gcc
GNU cc compiler for sparc-rtems4.10.

%files -n rtems-4.10-sparc-rtems4.10-gcc -f build/files.gcc
%defattr(-,root,root)
%dir %{_mandir}
%dir %{_mandir}/man1
%{_mandir}/man1/sparc-rtems4.10-gcc.1*
%if "%{gcc_version}" >= "3.4"
%{_mandir}/man1/sparc-rtems4.10-cpp.1*
%{_mandir}/man1/sparc-rtems4.10-gcov.1*
%endif

%dir %{_bindir}
%{_bindir}/sparc-rtems4.10-cpp%{_exeext}
%{_bindir}/sparc-rtems4.10-gcc%{_exeext}
%if "%{gcc_version}" >= "3.3"
%{_bindir}/sparc-rtems4.10-gcc-%{gcc_version}%{_exeext}
%endif
%{_bindir}/sparc-rtems4.10-gcov%{_exeext}
%{_bindir}/sparc-rtems4.10-gccbug

%dir %{gcclib}/sparc-rtems4.10/%{gcc_version}/include
%if "%{gcc_version}" > "4.0.3"
%if "sparc-rtems4.10" != "bfin-rtems4.10"
%if "sparc-rtems4.10" != "avr-rtems4.10"
%dir %{gcclib}/sparc-rtems4.10/%{gcc_version}/include/ssp
%endif
%endif
%endif

%if "%{gcc_version}" >= "4.3.0"
%dir %{gcclib}/sparc-rtems4.10/%{gcc_version}/include-fixed
%endif

%dir %{gccexec}
%dir %{gccexec}/sparc-rtems4.10
%dir %{gccexec}/sparc-rtems4.10/%{gcc_version}
%{gccexec}/sparc-rtems4.10/%{gcc_version}/cc1%{_exeext}
%{gccexec}/sparc-rtems4.10/%{gcc_version}/collect2%{_exeext}

# ==============================================================
# rtems-4.10-gcc-common
# ==============================================================
%package -n rtems-4.10-gcc-common
Summary:	Base package for rtems gcc and newlib C Library
Group:          Development/Tools
Version:        %{gcc_rpmvers}
License:	GPL

Requires(post): 	/sbin/install-info
Requires(preun):	/sbin/install-info

%description -n rtems-4.10-gcc-common
GCC files that are shared by all targets.

%files -n rtems-4.10-gcc-common
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

%post -n rtems-4.10-gcc-common
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/cpp.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/cppinternals.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/gcc.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/gccint.info.gz || :
%if "%{gcc_version}" >= "3.4"
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/gccinstall.info.gz || :
%endif

%preun -n rtems-4.10-gcc-common
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
# rtems-4.10-sparc-rtems4.10-gcc-c++
# ==============================================================
%package -n rtems-4.10-sparc-rtems4.10-gcc-c++
Summary:	GCC c++ compiler for sparc-rtems4.10
Group:		Development/Tools
Version:        %{gcc_rpmvers}
License:	GPL

%if "%{_build}" != "%{_host}"
BuildRequires:  rtems-4.10-sparc-rtems4.10-gcc-c++
%endif
Provides:	rtems-4.10-sparc-rtems4.10-c++ = %{gcc_rpmvers}-%{release}
Obsoletes:	rtems-4.10-sparc-rtems4.10-c++ < %{gcc_rpmvers}-%{release}

Requires:       rtems-4.10-gcc-common
Requires:       rtems-4.10-sparc-rtems4.10-gcc = %{gcc_rpmvers}-%{release}

%description -n rtems-4.10-sparc-rtems4.10-gcc-c++
GCC c++ compiler for sparc-rtems4.10.

%files -n rtems-4.10-sparc-rtems4.10-gcc-c++ -f build/files.g++
%defattr(-,root,root)
%{_mandir}/man1/sparc-rtems4.10-g++.1*

%{_bindir}/sparc-rtems4.10-c++%{_exeext}
%{_bindir}/sparc-rtems4.10-g++%{_exeext}

%dir %{gccexec}
%dir %{gccexec}/sparc-rtems4.10
%dir %{gccexec}/sparc-rtems4.10/%{gcc_version}
%{gccexec}/sparc-rtems4.10/%{gcc_version}/cc1plus%{_exeext}

%dir %{gcclib}/sparc-rtems4.10/%{gcc_version}/include
%if "%{gcc_version}" >= "3.2"
%{gcclib}/sparc-rtems4.10/%{gcc_version}/include/c++
%else
%{gcclib}/sparc-rtems4.10/%{gcc_version}/include/g++
%endif



# ==============================================================
# rtems-4.10-sparc-rtems4.10-newlib
# ==============================================================
%package -n rtems-4.10-sparc-rtems4.10-newlib
Summary:      	C Library (newlib) for sparc-rtems4.10
Group: 		Development/Tools
License:	Distributable
Version:	%{newlib_version}

Provides:	rtems-4.10-sparc-rtems4.10-libc = %{newlib_version}-%{release}
Obsoletes:	rtems-4.10-sparc-rtems4.10-libc < %{newlib_version}-%{release}

Requires:	rtems-4.10-newlib-common

%description -n rtems-4.10-sparc-rtems4.10-newlib
Newlib C Library for sparc-rtems4.10.

%files -n rtems-4.10-sparc-rtems4.10-newlib -f build/files.newlib
%defattr(-,root,root)
%dir %{_prefix}
%dir %{_exec_prefix}/sparc-rtems4.10
%{_exec_prefix}/sparc-rtems4.10/include

# ==============================================================
# rtems-4.10-newlib-common
# ==============================================================
%package -n rtems-4.10-newlib-common
Summary:	Base package for RTEMS newlib C Library
Group:          Development/Tools
Version:        %{newlib_version}
License:	Distributable

Requires(post): 	/sbin/install-info
Requires(preun):	/sbin/install-info

%description -n rtems-4.10-newlib-common
newlib files that are shared by all targets.

%files -n rtems-4.10-newlib-common
%defattr(-,root,root)
%dir %{_infodir}
%ghost %{_infodir}/dir
%{_infodir}/libc.info*
%{_infodir}/libm.info*

%post -n rtems-4.10-newlib-common
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/libc.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/libm.info.gz || :

%preun -n rtems-4.10-newlib-common
if [ $1 -eq 0 ]; then
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/libc.info.gz || :
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/libm.info.gz || :
fi

