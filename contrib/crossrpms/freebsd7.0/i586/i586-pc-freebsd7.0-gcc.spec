#
# Please send bugfixes or comments to
# 	http://www.rtems.org/bugzilla
#


%ifos cygwin cygwin32 mingw mingw32
%define _exeext .exe
%define debug_package           %{nil}
%define _libdir                 %{_exec_prefix}/lib
%else
%define _exeext %{nil}
%endif

%ifos cygwin cygwin32
%define optflags -O3 -pipe -march=i486 -funroll-loops
%endif

%ifos mingw mingw32
%if %{defined _mingw32_cflags}
%define optflags %{_mingw32_cflags}
%else
%define optflags -O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions --param=ssp-buffer-size=4 -mms-bitfields
%endif
%endif

%if "%{_build}" != "%{_host}"
%define _host_rpmprefix %{_host}-
%else
%define _host_rpmprefix %{nil}
%endif


%define gcc_pkgvers 4.3.3
%define gcc_version 4.3.3
%define gcc_rpmvers %{expand:%(echo "4.3.3" | tr - _ )}


Name:         	i586-pc-freebsd7.0-gcc
Summary:      	i586-pc-freebsd7.0 gcc

Group:	      	Development/Tools
Version:        %{gcc_rpmvers}
Release:      	0.20090506.1%{?dist}
License:      	GPL
URL:		http://gcc.gnu.org
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

%define _use_internal_dependency_generator 0

BuildRequires:  %{_host_rpmprefix}gcc

%if "%{gcc_version}" >= "4.3.0"
%define _gmp_minvers		4.1
%else
%if "%{gcc_version}" >= "4.2.0"
%define _gmp_minvers		4.1
%endif
%endif

%if %{defined _gmp_minvers}
BuildRequires: gmp-devel >= %{_gmp_minvers}
%if "%{_build}" != "%{_host}"
BuildRequires:  %{_host_rpmprefix}gmp-devel >= %{_gmp_minvers}
%endif
%endif


%if "%{gcc_version}" >= "4.4.0"
%define _mpfr_minvers	2.3.2
%define mpfr_version	2.4.1
%else
%if "%{gcc_version}" >= "4.3.0"
%define _mpfr_minvers	2.3.1
%define mpfr_version	2.3.2
%else
%if "%{gcc_version}" >= "4.2.0"
%define _mpfr_minvers	2.2.1
%define mpfr_version	2.3.2
%endif
%endif
%endif

%if %{defined _mpfr_minvers}
# FIXME: This is an ugly cludge
%{?fc9:%global mpfr_provided 2.3.1}
%{?fc10:%global mpfr_provided 2.3.2}
%{?fc11:%global mpfr_provided 2.4.1}
%{?suse10_3:%global mpfr_provided 2.2.1}
%{?cygwin:%global mpfr_provided 2.4.1}
%{?mingw32:%global mpfr_provided %{nil}}

%if %{defined mpfr_provided}
%if "%{mpfr_provided}" < "%{_mpfr_minvers}"
%define _build_mpfr 1
%else
%if "%{_build}" != "%{_host}"
BuildRequires:  %{_host_rpmprefix}mpfr-devel >= %{_mpfr_minvers}
%else
BuildRequires: mpfr-devel >= %{_mpfr_minvers}
%endif
%endif
%else
%define _build_mpfr 1
%endif

%endif

%if "%{_build}" != "%{_host}"
BuildRequires:  i586-pc-freebsd7.0-gcc = %{gcc_rpmvers}
%endif

%if "%{gcc_version}" >= "4.2.0"
BuildRequires:	flex bison
%endif


BuildRequires:	texinfo >= 4.2
BuildRequires:	i586-pc-freebsd7.0-binutils
BuildRequires:	i586-pc-freebsd7.0-sys-root

Requires:	i586-pc-freebsd7.0-binutils
Requires:	i586-pc-freebsd7.0-sys-root


%if "%{gcc_version}" >= "3.4"
%define gcclib %{_libdir}/gcc
%define gccexec %{_libexecdir}/gcc
%else
%define gcclib %{_libdir}/gcc-lib
%define gccexec %{_libdir}/gcc-lib
%endif

Source0: 	ftp://ftp.gnu.org/gnu/gcc/gcc-%{gcc_version}/gcc-core-%{gcc_pkgvers}.tar.bz2
%{?_without_sources:NoSource:	0}

Source1: 	ftp://ftp.gnu.org/gnu/gcc/gcc-%{gcc_version}/gcc-g++-%{gcc_pkgvers}.tar.bz2
%{?_without_sources:NoSource:	1}

Source2:	ftp://ftp.gnu.org/gnu/gcc/gcc-%{gcc_version}/gcc-fortran-%{gcc_pkgvers}.tar.bz2
%{?_without_sources:NoSource:	2}

Source5:	ftp://ftp.gnu.org/gnu/gcc/gcc-%{gcc_version}/gcc-objc-%{gcc_pkgvers}.tar.bz2
%{?_without_sources:NoSource:	5}

%if "%{gcc_version}" >= "4.3.0"
Source60:    http://www.mpfr.org/mpfr-current/mpfr-%{mpfr_version}.tar.bz2
%endif

%description
Cross gcc for i586-pc-freebsd7.0.

%prep
%setup -c -T -n %{name}-%{version}

%setup -q -T -D -n %{name}-%{version} -a0
%{?PATCH0:%patch0 -p0}

%setup -q -T -D -n %{name}-%{version} -a1
%{?PATCH1:%patch1 -p0}

%setup -q -T -D -n %{name}-%{version} -a2
%{?PATCH2:%patch2 -p0}



%setup -q -T -D -n %{name}-%{version} -a5
%{?PATCH5:%patch5 -p0}


%if 0%{?_build_mpfr}
%setup -q -T -D -n %{name}-%{version} -a60
%{?PATCH60:%patch60 -p1}
  # Build mpfr one-tree style
  ln -s ../mpfr-%{mpfr_version} gcc-%{gcc_pkgvers}/mpfr
%endif


  # Fix timestamps
  cd gcc-%{gcc_pkgvers}
  contrib/gcc_update --touch
  cd ..
%build
  mkdir -p build

  cd build

  languages="c"
  languages="$languages,c++"
  languages="$languages,f95"
  languages="$languages,objc"
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
    --target=i586-pc-freebsd7.0 \
    --disable-libstdcxx-pch \
    --with-gnu-as --with-gnu-ld --verbose \
    --with-system-zlib \
    --disable-nls --without-included-gettext \
    --disable-win32-registry \
    --enable-version-specific-runtime-libs \
    --enable-threads \
    --with-sysroot=%{_exec_prefix}/i586-pc-freebsd7.0/sys-root \
    --enable-languages="$languages" $optargs

%if "%_host" != "%_build"
  # Bug in gcc-3.2.1:
  # Somehow, gcc doesn't get syslimits.h right for Cdn-Xs
  mkdir -p gcc/include
  cp ../gcc-%{gcc_pkgvers}/gcc/gsyslimits.h gcc/include/syslimits.h
%endif

  make %{?_smp_mflags} all
  cd ..

%install
  rm -rf $RPM_BUILD_ROOT

  cd build

  make DESTDIR=$RPM_BUILD_ROOT install
  cd ..


%if "%{gcc_version}" <= "4.1.2"
# Misplaced header file
  if test -f $RPM_BUILD_ROOT%{_includedir}/mf-runtime.h; then
    mv $RPM_BUILD_ROOT%{_includedir}/mf-runtime.h \
      $RPM_BUILD_ROOT%{gcclib}/i586-pc-freebsd7.0/%{gcc_version}/include/
  fi
%endif

  # host library
  rm -f  ${RPM_BUILD_ROOT}%{_libdir}/libiberty.a

  # We use the version from binutils
  rm -f $RPM_BUILD_ROOT%{_bindir}/i586-pc-freebsd7.0-c++filt%{_exeext}


# Conflict with a native GCC's infos
  rm -rf $RPM_BUILD_ROOT%{_infodir}

# Conflict with a native GCC's man pages
  rm -rf $RPM_BUILD_ROOT%{_mandir}/man7

#  # Bug in gcc-4.0.0pre
#  mv $RPM_BUILD_ROOT%{_bindir}/gfortran $RPM_BUILD_ROOT%{_bindir}/i586-pc-freebsd7.0-gfortran

%if "%{gcc_version}" >= "3.4"
  # Bug in gcc-3.4.0pre
  rm -f $RPM_BUILD_ROOT%{_bindir}/i586-pc-freebsd7.0-i586-pc-freebsd7.0-gcjh%{_exeext}
%endif

%if "%{gcc_version}" >= "3.3"
  # Bug in gcc-3.3.x/gcc-3.4.x: Despite we don't need fixincludes, it installs
  # the fixinclude-install-tools
  rm -rf ${RPM_BUILD_ROOT}%{gcclib}/i586-pc-freebsd7.0/%{gcc_version}/install-tools
  rm -rf ${RPM_BUILD_ROOT}%{gccexec}/i586-pc-freebsd7.0/%{gcc_version}/install-tools
%endif

  # Bug in gcc > 4.1.0: Installs an unused, empty directory
  if test -d ${RPM_BUILD_ROOT}%{_prefix}/i586-pc-freebsd7.0/include/bits; then
    rmdir ${RPM_BUILD_ROOT}%{_prefix}/i586-pc-freebsd7.0/include/bits
  fi

  # Collect multilib subdirectories
  f=`build/gcc/xgcc -Bbuild/gcc/ --print-multi-lib | sed -e 's,;.*$,,'`


  rm -f dirs ;
  echo "%defattr(-,root,root,-)" >> dirs
  echo "%dir %{gcclib}" >> dirs
  echo "%dir %{gcclib}/i586-pc-freebsd7.0" >> dirs

  TGTDIR="%{gcclib}/i586-pc-freebsd7.0/%{gcc_version}"
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

  TGTDIR="%{gcclib}/i586-pc-freebsd7.0/%{gcc_version}"
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

  TGTDIR="%{_exec_prefix}/i586-pc-freebsd7.0/lib"
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
esac
done

sed -e 's,^[ ]*/usr/lib/rpm.*/brp-strip,./brp-strip,' \
< os_install_post~ > os_install_post 
%define __os_install_post . ./os_install_post


cat << EOF > %{_builddir}/%{name}-%{gcc_rpmvers}/find-provides
#!/bin/sh
grep -E -v '^${RPM_BUILD_ROOT}%{_exec_prefix}/i586-pc-freebsd7.0/(lib|include|sys-root)' \
  | grep -v '^${RPM_BUILD_ROOT}%{gcclib}/i586-pc-freebsd7.0/' | %__find_provides
EOF
chmod +x %{_builddir}/%{name}-%{gcc_rpmvers}/find-provides
%define __find_provides %{_builddir}/%{name}-%{gcc_rpmvers}/find-provides

cat << EOF > %{_builddir}/%{name}-%{gcc_rpmvers}/find-requires
#!/bin/sh
grep -E -v '^${RPM_BUILD_ROOT}%{_exec_prefix}/i586-pc-freebsd7.0/(lib|include|sys-root)' \
  | grep -v '^${RPM_BUILD_ROOT}%{gcclib}/i586-pc-freebsd7.0/' | %__find_requires
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
# i586-pc-freebsd7.0-gcc
# ==============================================================
# %package -n i586-pc-freebsd7.0-gcc
# Summary:        GNU cc compiler for i586-pc-freebsd7.0
# Group:          Development/Tools
# Version:        %{gcc_rpmvers}
# Requires:       i586-pc-freebsd7.0-binutils
# License:	GPL

# %if %build_infos
# Requires:      gcc-common
# %endif

%description -n i586-pc-freebsd7.0-gcc
GNU cc compiler for i586-pc-freebsd7.0.

%files -n i586-pc-freebsd7.0-gcc -f build/files.gcc
%defattr(-,root,root)
%{_mandir}/man1/i586-pc-freebsd7.0-gcc.1*
%if "%{gcc_version}" >= "3.4"
%{_mandir}/man1/i586-pc-freebsd7.0-cpp.1*
%{_mandir}/man1/i586-pc-freebsd7.0-gcov.1*
%endif

%{_bindir}/i586-pc-freebsd7.0-cpp%{_exeext}
%{_bindir}/i586-pc-freebsd7.0-gcc%{_exeext}
%if "%{gcc_version}" >= "3.3"
%{_bindir}/i586-pc-freebsd7.0-gcc-%{gcc_version}%{_exeext}
%endif
%{_bindir}/i586-pc-freebsd7.0-gcov%{_exeext}
%{_bindir}/i586-pc-freebsd7.0-gccbug

%dir %{gcclib}/i586-pc-freebsd7.0/%{gcc_version}/include
%if "%{gcc_version}" > "4.0.3"
%if "i586-pc-freebsd7.0" != "bfin-rtems4.10"
%if "i586-pc-freebsd7.0" != "avr-rtems4.10"
%dir %{gcclib}/i586-pc-freebsd7.0/%{gcc_version}/include/ssp
%endif
%endif
%endif

%if "%{gcc_version}" >= "4.3.0"
%dir %{gcclib}/i586-pc-freebsd7.0/%{gcc_version}/include-fixed
%endif

%dir %{gccexec}
%dir %{gccexec}/i586-pc-freebsd7.0
%dir %{gccexec}/i586-pc-freebsd7.0/%{gcc_version}
%{gccexec}/i586-pc-freebsd7.0/%{gcc_version}/cc1%{_exeext}
%{gccexec}/i586-pc-freebsd7.0/%{gcc_version}/collect2%{_exeext}

# ==============================================================
# i586-pc-freebsd7.0-gcc-c++
# ==============================================================
%package -n i586-pc-freebsd7.0-gcc-c++
Summary:	GCC c++ compiler for i586-pc-freebsd7.0
Group:		Development/Tools
Version:        %{gcc_rpmvers}
License:	GPL

%if "%{_build}" != "%{_host}"
BuildRequires:  i586-pc-freebsd7.0-gcc-c++ = %{gcc_rpmvers}
%endif
Provides:	i586-pc-freebsd7.0-c++ = %{gcc_rpmvers}-%{release}
Obsoletes:	i586-pc-freebsd7.0-c++ < %{gcc_rpmvers}-%{release}

Requires:       i586-pc-freebsd7.0-gcc = %{gcc_rpmvers}-%{release}

%description -n i586-pc-freebsd7.0-gcc-c++
GCC c++ compiler for i586-pc-freebsd7.0.

%files -n i586-pc-freebsd7.0-gcc-c++ -f build/files.g++
%defattr(-,root,root)
%{_mandir}/man1/i586-pc-freebsd7.0-g++.1*

%{_bindir}/i586-pc-freebsd7.0-c++%{_exeext}
%{_bindir}/i586-pc-freebsd7.0-g++%{_exeext}

%dir %{gccexec}
%dir %{gccexec}/i586-pc-freebsd7.0
%dir %{gccexec}/i586-pc-freebsd7.0/%{gcc_version}
%{gccexec}/i586-pc-freebsd7.0/%{gcc_version}/cc1plus%{_exeext}

%dir %{gcclib}/i586-pc-freebsd7.0/%{gcc_version}/include
%{gcclib}/i586-pc-freebsd7.0/%{gcc_version}/include/c++

# ==============================================================
# i586-pc-freebsd7.0-gcc-gfortran
# ==============================================================
%package -n i586-pc-freebsd7.0-gcc-gfortran
Summary:	Fortran 95 support for i586-pc-freebsd7.0-gcc
Group:          Development/Tools
Version:        %{gcc_rpmvers}
License:	GPL

Provides:	i586-pc-freebsd7.0-gfortran = %{gcc_rpmvers}-%{release}
Obsoletes:	i586-pc-freebsd7.0-gfortran < %{gcc_rpmvers}-%{release}

Requires:       i586-pc-freebsd7.0-gcc = %{gcc_rpmvers}-%{release}
Obsoletes:      i586-pc-freebsd7.0-g77 < %{gcc_rpmvers}-%{release}

%description -n i586-pc-freebsd7.0-gcc-gfortran
GCC fortran compiler for i586-pc-freebsd7.0.

%files -n i586-pc-freebsd7.0-gcc-gfortran -f build/files.gfortran
%defattr(-,root,root)
%{_bindir}/i586-pc-freebsd7.0-gfortran%{_exeext}

%{_mandir}/man1/i586-pc-freebsd7.0-gfortran.1*

%dir %{gccexec}
%dir %{gccexec}/i586-pc-freebsd7.0
%dir %{gccexec}/i586-pc-freebsd7.0/%{gcc_version}
%{gccexec}/i586-pc-freebsd7.0/%{gcc_version}/f951%{_exeext}

# ==============================================================
# i586-pc-freebsd7.0-gcc-objc
# ==============================================================
%package -n i586-pc-freebsd7.0-gcc-objc
Summary:        Objective C support for i586-pc-freebsd7.0-gcc
Group:          Development/Tools
Version:        %{gcc_rpmvers}
License:	GPL
Provides:	i586-pc-freebsd7.0-objc = %{gcc_rpmvers}-%{release}
Obsoletes:	i586-pc-freebsd7.0-objc < %{gcc_rpmvers}-%{release}

Requires:       i586-pc-freebsd7.0-gcc = %{gcc_rpmvers}-%{release}

%description -n i586-pc-freebsd7.0-gcc-objc
GCC objc compiler for i586-pc-freebsd7.0.

%files -n i586-pc-freebsd7.0-gcc-objc -f build/files.objc
%defattr(-,root,root)

%dir %{gccexec}
%dir %{gccexec}/i586-pc-freebsd7.0
%dir %{gccexec}/i586-pc-freebsd7.0/%{gcc_version}
%{gccexec}/i586-pc-freebsd7.0/%{gcc_version}/cc1obj%{_exeext}

%{gcclib}/i586-pc-freebsd7.0/%{gcc_version}/include/objc

