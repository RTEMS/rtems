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


%define gcc_pkgvers 3.4.4
%define gcc_version 3.4.4
%define gcc_rpmvers %{expand:%(echo "3.4.4" | tr - _ )}


Name:         	i686-pc-cygwin-gcc
Summary:      	i686-pc-cygwin gcc

Group:	      	Development/Tools
Version:        %{gcc_rpmvers}
Release:      	0.20100711.0%{?dist}
License:      	GPL
URL:		http://gcc.gnu.org
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

%define _use_internal_dependency_generator 0

BuildRequires:  %{_host_rpmprefix}gcc

# FIXME: Disable lto for now, to avoid dependencies on libelf
%bcond_with lto

# FIXME: Disable python gdb scripts
# ATM, no idea how to package them
%bcond_with pygdb

# FIXME: Disable GCC-plugin
# Bug in gcc-4.5-20100318, doesn't build them on x86_84 hosts.
%bcond_with plugin

# versions of libraries, we conditionally bundle if necessary
%global mpc_version	0.8.1
%global mpfr_version	2.4.2
%global gmp_version	4.3.2
%global libelf_version  0.8.13

# versions of libraries these distros are known to ship
%if 0%{?fc13}
%global mpc_provided 0.8.1
%global mpfr_provided 2.4.2
%global gmp_provided 4.3.1
%endif

%if 0%{?fc12}
%global mpc_provided 0.8
%global mpfr_provided 2.4.1
%global gmp_provided 4.3.1
%endif

%if 0%{?fc11}
%global mpc_provided %{nil}
%global mpfr_provided 2.4.1
%global gmp_provided 4.2.4
%endif

%if 0%{?el6}
%global mpc_provided %{nil}
%global mpfr_provided 2.4.1
%global gmp_provided 4.3.1
%endif

%if 0%{?el5}
%global mpc_provided %{nil}
%global mpfr_provided %{nil}
%global gmp_provided 4.1.4
%endif

%if 0%{?suse11_0}
%global mpc_provided %{nil}
%global mpfr_provided 2.3.1
%global gmp_provided 4.2.2
%endif

%if 0%{?suse11_1}
%global mpc_provided %{nil}
%global mpfr_provided 2.3.2
%global gmp_provided 4.2.3
%endif

%if 0%{?suse11_2}
%global mpc_provided 0.7
%global mpfr_provided 2.4.1
%global gmp_provided 4.3.1
%endif

%if 0%{?suse11_3}
%global mpc_provided 0.8.1
%global mpfr_provided 2.4.2
%global gmp_provided 4.3.2
%endif

%if 0%{?cygwin}
%global mpc_provided 0.8
%global mpfr_provided 2.4.1
%global gmp_provided 4.3.1
%endif

%if 0%{?mingw32}
%global mpc_provided 0.8.1
%global mpfr_provided 2.4.1
%global gmp_provided 4.3.2
%endif

%if "%{gcc_version}" >= "4.2.0"
%endif

%if "%{gcc_version}" >= "4.3.0"
%define gmp_required		4.1
%define mpfr_required		2.3.1
%endif

%if "%{gcc_version}" >= "4.3.3"
%define cloog_required 		0.15
%endif

%if "%{gcc_version}" >= "4.4.0"
%define mpfr_required		2.3.2
%endif

%if "%{gcc_version}" >= "4.5.0"
%define mpc_required 		0.8
%if %{with lto}
%define libelf_required 	0.8.12
%endif
%endif

%if %{defined mpc_required}
%if "%{mpc_provided}" >= "%{mpc_required}"
%{?fedora:BuildRequires: libmpc-devel >= %{mpc_required}}
%{?suse:BuildRequires: mpc-devel >= %{mpc_required}}
%if "%{_build}" != "%{_host}"
BuildRequires:  %{_host_rpmprefix}mpc-devel >= %{mpc_required}
%endif
%else
%define _build_mpc 1
%define gmp_required 		4.2
%endif
%endif

%if %{defined gmp_required}
%if "%{gmp_provided}" >= "%{gmp_required}"
BuildRequires: gmp-devel >= %{gmp_required}
%if "%{_build}" != "%{_host}"
BuildRequires:  %{_host_rpmprefix}gmp-devel >= %{gmp_required}
%endif
%else
%define _build_gmp 1
%endif
%endif

%if %{defined libelf_required}
%if "%{libelf_provided}" >= "%{libelf_required}"
BuildRequires: libelf-devel >= %{libelf_required}
%if "%{_build}" != "%{_host}"
BuildRequires:  %{_host_rpmprefix}libelf-devel >= %{libelf_required}
%endif
%else
%define _build_libelf 1
%endif
%endif


%if %{defined cloog_required}
%{?fc11:BuildRequires: cloog-ppl-devel >= %cloog_required}
%{?fc12:BuildRequires: cloog-ppl-devel >= %cloog_required}
%{?fc13:BuildRequires: cloog-ppl-devel >= %cloog_required}
%{?el6:BuildRequires: cloog-ppl-devel >= %cloog_required}
%{?suse11_3:BuildRequires: cloog-devel >= %cloog_required, ppl-devel}
%{?suse11_2:BuildRequires: cloog-devel >= %cloog_required, ppl-devel}
%{?suse11_1:BuildRequires: cloog-devel >= %cloog_required, ppl-devel}
%endif


%if %{defined mpfr_required}
%if "%{mpfr_provided}" >= "%{mpfr_required}"
BuildRequires: mpfr-devel >= %{mpfr_required}
%if "%{_build}" != "%{_host}"
BuildRequires:  %{_host_rpmprefix}mpfr-devel >= %{mpfr_required}
%endif
%else
%define _build_mpfr 1
%endif
%endif

%if "%{_build}" != "%{_host}"
BuildRequires:  i686-pc-cygwin-gcc = %{gcc_rpmvers}
%endif

%if "%{gcc_version}" >= "4.2.0"
BuildRequires:	flex bison
%endif


BuildRequires:	texinfo >= 4.2
BuildRequires:	i686-pc-cygwin-binutils
BuildRequires:	i686-pc-cygwin-sys-root

Requires:	i686-pc-cygwin-binutils
Requires:	i686-pc-cygwin-sys-root
Requires:	i686-pc-cygwin-w32api
Requires:	i686-pc-cygwin-gcc-libgcc = %{gcc_rpmvers}-%{release}

%if "%{gcc_version}" >= "4.5.0"
BuildRequires:  zlib-devel
%if "%{_build}" != "%{_host}"
BuildRequires:  %{_host_rpmprefix}zlib-devel
%endif
%else
%endif

%global _gcclibdir %{_prefix}/lib

Source0: 	ftp://ftp.gnu.org/gnu/gcc/gcc-%{gcc_version}/gcc-core-%{gcc_pkgvers}.tar.bz2
%{?_without_sources:NoSource:	0}

Source1: 	ftp://ftp.gnu.org/gnu/gcc/gcc-%{gcc_version}/gcc-g++-%{gcc_pkgvers}.tar.bz2
%{?_without_sources:NoSource:	1}


%if "%{gcc_version}" == "3.4.4"
# Cygwin patch extracted from
# ftp://sourceware.org/pub/cygwin/release/gcc/gcc-3.4.4-3-src.tar.bz2
Patch70: gcc-3.4.4-3.patch

Patch71: cygwin-gcc-3.4.4-3.diff
%endif
%if "%{gcc_version}" == "4.3.4"
# Cygwin patch extracted from
# ftp://sourceware.org/pub/cygwin/release/gcc4/gcc4-4.3.4-3-src.tar.bz2
Patch70: gcc4-4.3.4-3.src.patch

Patch71: cygwin-gcc4-4.3.4-3.diff
%endif

%if 0%{?_build_mpfr}
Source60:    http://www.mpfr.org/mpfr-current/mpfr-%{mpfr_version}.tar.bz2
%endif

%if 0%{?_build_mpc}
Source61:    http://www.multiprecision.org/mpc/download/mpc-%{mpc_version}.tar.gz
%endif

%if 0%{?_build_gmp}
Source62:    ftp://ftp.gnu.org/gnu/gmp/gmp-%{gmp_version}.tar.bz2
%endif

%if 0%{?_build_libelf}
Source63:    http://www.mr511.de/software/libelf-%{libelf_version}.tar.gz
%endif

%description
Cross gcc for i686-pc-cygwin.

%prep
%setup -c -T -n %{name}-%{version}

%setup -q -T -D -n %{name}-%{version} -a0
cd gcc-%{gcc_pkgvers}
%{?PATCH0:%patch0 -p1}
cd ..

%setup -q -T -D -n %{name}-%{version} -a1
cd gcc-%{gcc_pkgvers}
%{?PATCH1:%patch1 -p1}
cd ..






%if 0%{?_build_mpfr}
%setup -q -T -D -n %{name}-%{version} -a60
%{?PATCH60:%patch60 -p1}
  # Build mpfr one-tree style
  ln -s ../mpfr-%{mpfr_version} gcc-%{gcc_pkgvers}/mpfr
%endif

%if 0%{?_build_mpc}
%setup -q -T -D -n %{name}-%{version} -a61
%{?PATCH61:%patch61 -p1}
  # Build mpc one-tree style
  ln -s ../mpc-%{mpc_version} gcc-%{gcc_pkgvers}/mpc
%endif

%if 0%{?_build_gmp}
%setup -q -T -D -n %{name}-%{version} -a62
%{?PATCH62:%patch62 -p1}
  # Build gmp one-tree style
  ln -s ../gmp-%{gmp_version} gcc-%{gcc_pkgvers}/gmp
%endif

%if 0%{?_build_libelf}
%setup -q -T -D -n %{name}-%{version} -a63
%{?PATCH63:%patch63 -p1}
  # Build libelf one-tree style
  ln -s ../libelf-%{libelf_version} gcc-%{gcc_pkgvers}/libelf
%endif

# cygwin patches can't be directly applied through rpm
cd gcc-%{gcc_pkgvers}
%if "%{gcc_version}" < "4.0"
patch -p1 -t < %{PATCH70} || true
%else
patch -p2 -t < %{PATCH70} || true
%endif
%{?PATCH71:patch -p1 < %{PATCH71}}
cd ..

  # Fix timestamps
  cd gcc-%{gcc_pkgvers}
  contrib/gcc_update --touch
  cd ..
%build
  mkdir -p build

  cd build

  languages="c"
  languages="$languages,c++"
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
    --libdir=%{_gcclibdir} \
    --libexecdir=%{_libexecdir} \
    --mandir=%{_mandir} \
    --infodir=%{_infodir} \
    --datadir=%{_datadir} \
    --build=%_build --host=%_host \
    --target=i686-pc-cygwin \
    --disable-libstdcxx-pch \
    --with-gnu-as --with-gnu-ld --verbose \
    --with-newlib \
    --with-system-zlib \
    --disable-nls --without-included-gettext \
    --disable-win32-registry \
    --enable-version-specific-runtime-libs \
    --enable-threads \
    --with-sysroot=%{_exec_prefix}/i686-pc-cygwin/sys-root \
    %{?with_lto:--enable-lto}%{!?with_lto:--disable-lto} \
    %{?with_plugin:--enable-plugin}%{!?with_plugin:--disable-plugin} \
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
      $RPM_BUILD_ROOT%{_gcclibdir}/gcc/i686-pc-cygwin/%{gcc_version}/include/
  fi
%endif

  # host library
%if "%{gcc_version}" >= "4.2.0"
  # libiberty doesn't honor --libdir, but always installs to a 
  # magically guessed _libdir
  rm -f  ${RPM_BUILD_ROOT}%{_libdir}/libiberty.a
%else
  # libiberty installs to --libdir=...
  rm -f ${RPM_BUILD_ROOT}%{_gcclibdir}/libiberty.a
%endif

  # We use the version from binutils
  rm -f $RPM_BUILD_ROOT%{_bindir}/i686-pc-cygwin-c++filt%{_exeext}


# Conflict with a native GCC's infos
  rm -rf $RPM_BUILD_ROOT%{_infodir}

# Conflict with a native GCC's man pages
  rm -rf $RPM_BUILD_ROOT%{_mandir}/man7

  # Bug in gcc-3.4.0pre
  rm -f $RPM_BUILD_ROOT%{_bindir}/i686-pc-cygwin-i686-pc-cygwin-gcjh%{_exeext}

  # Bug in gcc-3.3.x/gcc-3.4.x: Despite we don't need fixincludes, it installs
  # the fixinclude-install-tools
  rm -rf ${RPM_BUILD_ROOT}%{_gcclibdir}/gcc/i686-pc-cygwin/%{gcc_version}/install-tools
  rm -rf ${RPM_BUILD_ROOT}%{_libexecdir}/gcc/i686-pc-cygwin/%{gcc_version}/install-tools

  # Bug in gcc > 4.1.0: Installs an unused, empty directory
  if test -d ${RPM_BUILD_ROOT}%{_prefix}/i686-pc-cygwin/include/bits; then
    rmdir ${RPM_BUILD_ROOT}%{_prefix}/i686-pc-cygwin/include/bits
  fi

  # gcc >= 4.5.0: installs weird libstdc++ python bindings.
%if ! %{with pygdb}
  if test -d ${RPM_BUILD_ROOT}%{_datadir}/gcc-%{gcc_version}/python; then
    rm -rf ${RPM_BUILD_ROOT}%{_datadir}/gcc-%{gcc_version}/python/libstdcxx
  fi
%endif

  # Collect multilib subdirectories
  multilibs=`build/gcc/xgcc -Bbuild/gcc/ --print-multi-lib | sed -e 's,;.*$,,'`


  rm -f dirs ;
  echo "%defattr(-,root,root,-)" >> dirs
  TGTDIR="%{_gcclibdir}/gcc/i686-pc-cygwin/%{gcc_version}"
  for i in $multilibs; do
    case $i in
    \.) ;; # ignore, handled elsewhere
    *)  echo "%dir ${TGTDIR}/$i" >> dirs
      ;;
    esac
  done

  # Collect files to go into different packages
  cp dirs build/files.gcc
  cp dirs build/files.gfortran
  cp dirs build/files.objc
  cp dirs build/files.gcj
  cp dirs build/files.g++

  TGTDIR="%{_gcclibdir}/gcc/i686-pc-cygwin/%{gcc_version}"
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
    *include-fixed/*);;
    *finclude/*);;
    *adainclude*);;
    *adalib*);;
    *gnat1);;
    *jc1) ;;
    *jvgenmain) ;;
    */libgfortran*.*) echo "$i" >> build/files.gfortran ;;
    %{!?with_pygdb:*/libstdc++*gdb.py*) rm ${RPM_BUILD_ROOT}/$i ;;} # ignore for now
    %{?with_pygdb:*/libstdc++*gdb.py*) >> build/files.g++ ;;}
    */libstdc++.*) echo "$i" >> build/files.g++ ;;
    */libsupc++.*) echo "$i" >> build/files.g++ ;;
    *) echo "$i" >> build/files.gcc ;;
    esac
  done

  TGTDIR="%{_exec_prefix}/i686-pc-cygwin/lib"
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
grep -E -v '^${RPM_BUILD_ROOT}%{_exec_prefix}/i686-pc-cygwin/(lib|include|sys-root)' \
  %{?_gcclibdir:| grep -v '^${RPM_BUILD_ROOT}%{_gcclibdir}/gcc/i686-pc-cygwin/'} | %__find_provides
EOF
chmod +x %{_builddir}/%{name}-%{gcc_rpmvers}/find-provides
%define __find_provides %{_builddir}/%{name}-%{gcc_rpmvers}/find-provides

cat << EOF > %{_builddir}/%{name}-%{gcc_rpmvers}/find-requires
#!/bin/sh
grep -E -v '^${RPM_BUILD_ROOT}%{_exec_prefix}/i686-pc-cygwin/(lib|include|sys-root)' \
  %{?_gcclibdir:| grep -v '^${RPM_BUILD_ROOT}%{_gcclibdir}/gcc/i686-pc-cygwin/'} | %__find_requires
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
# i686-pc-cygwin-gcc
# ==============================================================
# %package -n i686-pc-cygwin-gcc
# Summary:        GNU cc compiler for i686-pc-cygwin
# Group:          Development/Tools
# Version:        %{gcc_rpmvers}
# Requires:       i686-pc-cygwin-binutils
# License:	GPL

# %if %build_infos
# Requires:      gcc-common
# %endif

%description -n i686-pc-cygwin-gcc
GNU cc compiler for i686-pc-cygwin.

# ==============================================================
# i686-pc-cygwin-gcc-libgcc
# ==============================================================
%package -n i686-pc-cygwin-gcc-libgcc
Summary:        libgcc for i686-pc-cygwin-gcc
Group:          Development/Tools
Version:        %{gcc_rpmvers}
%{?_with_noarch_subpackages:BuildArch: noarch}
License:	GPL

%description -n i686-pc-cygwin-gcc-libgcc
libgcc i686-pc-cygwin-gcc.


%files -n i686-pc-cygwin-gcc
%defattr(-,root,root)

%{_mandir}/man1/i686-pc-cygwin-gcc.1*
%{_mandir}/man1/i686-pc-cygwin-cpp.1*
%{_mandir}/man1/i686-pc-cygwin-gcov.1*

%{_bindir}/i686-pc-cygwin-cpp%{_exeext}
%{_bindir}/i686-pc-cygwin-gcc%{_exeext}
%{_bindir}/i686-pc-cygwin-gcc-%{gcc_version}%{_exeext}
%{_bindir}/i686-pc-cygwin-gcov%{_exeext}
%{_bindir}/i686-pc-cygwin-gccbug

%dir %{_libexecdir}/gcc
%dir %{_libexecdir}/gcc/i686-pc-cygwin
%dir %{_libexecdir}/gcc/i686-pc-cygwin/%{gcc_version}
%{_libexecdir}/gcc/i686-pc-cygwin/%{gcc_version}/cc1%{_exeext}
%{_libexecdir}/gcc/i686-pc-cygwin/%{gcc_version}/collect2%{_exeext}
%if "%{gcc_version}" >= "4.5.0"
%{?with_lto:%{_libexecdir}/gcc/i686-pc-cygwin/%{gcc_version}/lto%{_exeext}}
%{_libexecdir}/gcc/i686-pc-cygwin/%{gcc_version}/lto-wrapper%{_exeext}
%endif

%files -n i686-pc-cygwin-gcc-libgcc -f build/files.gcc
%defattr(-,root,root)
%dir %{_gcclibdir}/gcc
%dir %{_gcclibdir}/gcc/i686-pc-cygwin
%dir %{_gcclibdir}/gcc/i686-pc-cygwin/%{gcc_version}
%dir %{_gcclibdir}/gcc/i686-pc-cygwin/%{gcc_version}/include

%if "%{gcc_version}" > "4.0.3"
%dir %{_gcclibdir}/gcc/i686-pc-cygwin/%{gcc_version}/include/ssp
%endif

%if "%{gcc_version}" >= "4.3.0"
%{_gcclibdir}/gcc/i686-pc-cygwin/%{gcc_version}/include-fixed
%endif

# ==============================================================
# i686-pc-cygwin-gcc-c++
# ==============================================================
%package -n i686-pc-cygwin-gcc-c++
Summary:	GCC c++ compiler for i686-pc-cygwin
Group:		Development/Tools
Version:        %{gcc_rpmvers}
License:	GPL
Requires:       i686-pc-cygwin-gcc-libstdc++ = %{gcc_rpmvers}-%{release}

%if "%{_build}" != "%{_host}"
BuildRequires:  i686-pc-cygwin-gcc-c++ = %{gcc_rpmvers}
%endif

Requires:       i686-pc-cygwin-gcc = %{gcc_rpmvers}-%{release}

%description -n i686-pc-cygwin-gcc-c++
GCC c++ compiler for i686-pc-cygwin.


%package -n i686-pc-cygwin-gcc-libstdc++
Summary:	libstdc++ for i686-pc-cygwin
Group:		Development/Tools
Version:        %{gcc_rpmvers}
%{?_with_noarch_subpackages:BuildArch: noarch}
License:	GPL

%description -n i686-pc-cygwin-gcc-libstdc++
%{summary}


%files -n i686-pc-cygwin-gcc-c++
%defattr(-,root,root)

%{_mandir}/man1/i686-pc-cygwin-g++.1*

%{_bindir}/i686-pc-cygwin-c++%{_exeext}
%{_bindir}/i686-pc-cygwin-g++%{_exeext}

%dir %{_libexecdir}/gcc
%dir %{_libexecdir}/gcc/i686-pc-cygwin
%dir %{_libexecdir}/gcc/i686-pc-cygwin/%{gcc_version}
%{_libexecdir}/gcc/i686-pc-cygwin/%{gcc_version}/cc1plus%{_exeext}


%files -n i686-pc-cygwin-gcc-libstdc++ -f build/files.g++
%defattr(-,root,root)
%dir %{_gcclibdir}/gcc
%dir %{_gcclibdir}/gcc/i686-pc-cygwin
%dir %{_gcclibdir}/gcc/i686-pc-cygwin/%{gcc_version}
%dir %{_gcclibdir}/gcc/i686-pc-cygwin/%{gcc_version}/include
%{_gcclibdir}/gcc/i686-pc-cygwin/%{gcc_version}/include/c++



