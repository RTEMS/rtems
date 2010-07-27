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

Name:		i586-pc-freebsd8.1-binutils
Summary:	Binutils for target i586-pc-freebsd8.1
Group:		Development/Tools
Version:	%{binutils_rpmvers}
Release:	0.20100727.0%{?dist}
License:	GPL/LGPL
URL: 		http://sources.redhat.com/binutils
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:	%{_host_rpmprefix}gcc

%if "%{binutils_version}" >= "2.18"
# Bug in bfd: Doesn't build without texinfo installed
BuildRequires:	texinfo >= 4.2
%else
%endif
BuildRequires:	flex
BuildRequires:	bison

Source0: http://ftp.gnu.org/gnu/binutils/binutils-%{binutils_pkgvers}.tar.bz2

%description
Cross binutils for target i586-pc-freebsd8.1
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
    --target=i586-pc-freebsd8.1 \
    --verbose --disable-nls \
    --without-included-gettext \
    --disable-win32-registry \
    --disable-werror \
    --with-sysroot=%{_prefix}/i586-pc-freebsd8.1/sys-root \
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
  if test ! -f ${RPM_BUILD_ROOT}%{_bindir}/i586-pc-freebsd8.1-dlltool%{_exeext}; then 
    rm -f ${RPM_BUILD_ROOT}%{_mandir}/man1/i586-pc-freebsd8.1-dlltool*
  fi
  if test ! -f ${RPM_BUILD_ROOT}%{_bindir}/i586-pc-freebsd8.1-nlmconv%{_exeext}; then 
    rm -f ${RPM_BUILD_ROOT}%{_mandir}/man1/i586-pc-freebsd8.1-nlmconv*
  fi
  if test ! -f ${RPM_BUILD_ROOT}%{_bindir}/i586-pc-freebsd8.1-windres%{_exeext}; then 
    rm -f ${RPM_BUILD_ROOT}%{_mandir}/man1/i586-pc-freebsd8.1-windres*
  fi
  if test ! -f ${RPM_BUILD_ROOT}%{_bindir}/i586-pc-freebsd8.1-windmc%{_exeext}; then 
    rm -f ${RPM_BUILD_ROOT}%{_mandir}/man1/i586-pc-freebsd8.1-windmc*
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
# i586-pc-freebsd8.1-binutils
# ==============================================================
# %package -n i586-pc-freebsd8.1-binutils
# Summary:      rtems binutils for i586-pc-freebsd8.1
# Group: Development/Tools
# %if %build_infos
# Requires: binutils-common
# %endif

%description -n i586-pc-freebsd8.1-binutils
GNU binutils targetting i586-pc-freebsd8.1.

%files -n i586-pc-freebsd8.1-binutils
%defattr(-,root,root)

%{_mandir}/man1/i586-pc-freebsd8.1-*.1*

%{_bindir}/i586-pc-freebsd8.1-*

%dir %{_exec_prefix}/i586-pc-freebsd8.1
%dir %{_exec_prefix}/i586-pc-freebsd8.1/bin
%{_exec_prefix}/i586-pc-freebsd8.1/bin/*

%dir %{_exec_prefix}/i586-pc-freebsd8.1/lib
%{_exec_prefix}/i586-pc-freebsd8.1/lib/ldscripts

