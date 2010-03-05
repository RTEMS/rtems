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

%define binutils_pkgvers 2.20.1
%define binutils_version 2.20.1
%define binutils_rpmvers %{expand:%(echo "2.20.1" | tr - _ )}

Name:		sparc-sun-solaris2.7-binutils
Summary:	Binutils for target sparc-sun-solaris2.7
Group:		Development/Tools
Version:	%{binutils_rpmvers}
Release:	0.20100305.1%{?dist}
License:	GPL/LGPL
URL: 		http://sources.redhat.com/binutils
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:	%{_host_rpmprefix}gcc

%if "%{binutils_version}" >= "2.18"
# Bug in bfd: Doesn't build without texinfo installed
BuildRequires:	texinfo >= 4.2
%else
%if "sparc-sun-solaris2.7" == "i686-pc-cygwin"
BuildRequires:	texinfo >= 4.2
%endif
%endif
BuildRequires:	flex
BuildRequires:	bison

Source0: http://ftp.gnu.org/gnu/binutils/binutils-%{binutils_pkgvers}.tar.bz2
# Patch0:  ftp://ftp.rtems.org/pub/rtems/SOURCES/4.10/binutils-2.19-rtems4.10-20081023.diff

%description
Cross binutils for target sparc-sun-solaris2.7
%prep
%setup -q -c -T -n %{name}-%{version}

%setup -q -D -T -n %{name}-%{version} -a0
cd binutils-%{binutils_pkgvers}
%{?PATCH0:%patch0 -p1}
cd ..

%build
  mkdir -p build
  cd build
%if "%{_build}" != "%{_host}"
  CFLAGS_FOR_BUILD="-g -O2 -Wall" \
%endif
  CFLAGS="$RPM_OPT_FLAGS" \
  ../binutils-%{binutils_pkgvers}/configure \
    --build=%_build --host=%_host \
    --target=sparc-sun-solaris2.7 \
    --verbose --disable-nls \
    --without-included-gettext \
    --disable-win32-registry \
    --disable-werror \
    --with-sysroot=%{_prefix}/sparc-sun-solaris2.7/sys-root \
    --prefix=%{_prefix} --bindir=%{_bindir} \
    --exec-prefix=%{_exec_prefix} \
    --includedir=%{_includedir} --libdir=%{_libdir} \
    --mandir=%{_mandir} --infodir=%{_infodir}

  make %{?_smp_mflags} all
  cd ..

%install
  rm -rf $RPM_BUILD_ROOT

  cd build
  make DESTDIR=$RPM_BUILD_ROOT install


# Conflict with a native binutils' infos
  rm -rf $RPM_BUILD_ROOT%{_infodir}

# We don't ship host files
  rm -f ${RPM_BUILD_ROOT}%{_libdir}/libiberty*

# manpages without corresponding tools
  if test ! -f ${RPM_BUILD_ROOT}%{_bindir}/sparc-sun-solaris2.7-dlltool%{_exeext}; then 
    rm -f ${RPM_BUILD_ROOT}%{_mandir}/man1/sparc-sun-solaris2.7-dlltool*
  fi
  if test ! -f ${RPM_BUILD_ROOT}%{_bindir}/sparc-sun-solaris2.7-nlmconv%{_exeext}; then 
    rm -f ${RPM_BUILD_ROOT}%{_mandir}/man1/sparc-sun-solaris2.7-nlmconv*
  fi
  if test ! -f ${RPM_BUILD_ROOT}%{_bindir}/sparc-sun-solaris2.7-windres%{_exeext}; then 
    rm -f ${RPM_BUILD_ROOT}%{_mandir}/man1/sparc-sun-solaris2.7-windres*
  fi
  if test ! -f ${RPM_BUILD_ROOT}%{_bindir}/sparc-sun-solaris2.7-windmc%{_exeext}; then 
    rm -f ${RPM_BUILD_ROOT}%{_mandir}/man1/sparc-sun-solaris2.7-windmc*
  fi

  cd ..

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

%clean
  rm -rf $RPM_BUILD_ROOT

# ==============================================================
# sparc-sun-solaris2.7-binutils
# ==============================================================
# %package -n sparc-sun-solaris2.7-binutils
# Summary:      rtems binutils for sparc-sun-solaris2.7
# Group: Development/Tools
# %if %build_infos
# Requires: binutils-common
# %endif

%description -n sparc-sun-solaris2.7-binutils
GNU binutils targetting sparc-sun-solaris2.7.

%files -n sparc-sun-solaris2.7-binutils
%defattr(-,root,root)

%{_mandir}/man1/sparc-sun-solaris2.7-*.1*

%{_bindir}/sparc-sun-solaris2.7-*

%dir %{_exec_prefix}/sparc-sun-solaris2.7
%dir %{_exec_prefix}/sparc-sun-solaris2.7/bin
%{_exec_prefix}/sparc-sun-solaris2.7/bin/*

%dir %{_exec_prefix}/sparc-sun-solaris2.7/lib
%{_exec_prefix}/sparc-sun-solaris2.7/lib/ldscripts

