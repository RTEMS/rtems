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

Name:		rtems-4.10-lm32-rtems4.10-binutils
Summary:	Binutils for target lm32-rtems4.10
Group:		Development/Tools
Version:	%{binutils_rpmvers}
Release:	3%{?dist}
License:	GPL/LGPL
URL: 		http://sources.redhat.com/binutils
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:	%{_host_rpmprefix}gcc

%if "%{binutils_version}" >= "2.18"
# Bug in bfd: Doesn't build without texinfo installed
BuildRequires:	texinfo >= 4.2
%else
# Required for building the infos
BuildRequires:	/sbin/install-info
BuildRequires:	texinfo >= 4.2
%endif
BuildRequires:	flex
BuildRequires:	bison

Requires:	rtems-4.10-binutils-common

Source0: ftp://ftp.gnu.org/gnu/binutils/binutils-%{binutils_pkgvers}.tar.bz2
Patch0:  ftp://ftp.rtems.org/pub/rtems/SOURCES/4.10/binutils-2.20.1-rtems4.10-20100826.diff

%description
Cross binutils for target lm32-rtems4.10
%prep
%setup -q -c -T -n %{name}-%{version}

%setup -q -D -T -n %{name}-%{version} -a0
cd binutils-%{binutils_pkgvers}
%{?PATCH0:%patch0 -p1}
cd ..

%build
  export PATH="%{_bindir}:${PATH}"
  mkdir -p build
  cd build
%if "%{_build}" != "%{_host}"
  CFLAGS_FOR_BUILD="-g -O2 -Wall" \
%endif
  CFLAGS="$RPM_OPT_FLAGS" \
  ../binutils-%{binutils_pkgvers}/configure \
    --build=%_build --host=%_host \
    --target=lm32-rtems4.10 \
    --verbose --disable-nls \
    --without-included-gettext \
    --disable-win32-registry \
    --disable-werror \
    --prefix=%{_prefix} --bindir=%{_bindir} \
    --exec-prefix=%{_exec_prefix} \
    --includedir=%{_includedir} --libdir=%{_libdir} \
    --mandir=%{_mandir} --infodir=%{_infodir}

  make %{?_smp_mflags} all
  make info
  cd ..

%install
  export PATH="%{_bindir}:${PATH}"
  rm -rf $RPM_BUILD_ROOT

  cd build
  make DESTDIR=$RPM_BUILD_ROOT install

  make prefix=$RPM_BUILD_ROOT%{_prefix} \
    bindir=$RPM_BUILD_ROOT%{_bindir} \
    includedir=$RPM_BUILD_ROOT%{_includedir} \
    libdir=$RPM_BUILD_ROOT%{_libdir} \
    infodir=$RPM_BUILD_ROOT%{_infodir} \
    mandir=$RPM_BUILD_ROOT%{_mandir} \
    exec_prefix=$RPM_BUILD_ROOT%{_exec_prefix} \
    install-info

# Dropped in FSF-binutils-2.9.5, but Cygwin still ships it.
  rm -rf $RPM_BUILD_ROOT%{_infodir}/configure.info*

  rm -f $RPM_BUILD_ROOT%{_infodir}/dir
  touch $RPM_BUILD_ROOT%{_infodir}/dir

# binutils does not install share/locale, however it uses it
  mkdir -p $RPM_BUILD_ROOT%{_prefix}/share/locale

# We don't ship host files
  rm -f ${RPM_BUILD_ROOT}%{_libdir}/libiberty*

# manpages without corresponding tools
  if test ! -f ${RPM_BUILD_ROOT}%{_bindir}/lm32-rtems4.10-dlltool%{_exeext}; then 
    rm -f ${RPM_BUILD_ROOT}%{_mandir}/man1/lm32-rtems4.10-dlltool*
  fi
  if test ! -f ${RPM_BUILD_ROOT}%{_bindir}/lm32-rtems4.10-nlmconv%{_exeext}; then 
    rm -f ${RPM_BUILD_ROOT}%{_mandir}/man1/lm32-rtems4.10-nlmconv*
  fi
  if test ! -f ${RPM_BUILD_ROOT}%{_bindir}/lm32-rtems4.10-windres%{_exeext}; then 
    rm -f ${RPM_BUILD_ROOT}%{_mandir}/man1/lm32-rtems4.10-windres*
  fi
  if test ! -f ${RPM_BUILD_ROOT}%{_bindir}/lm32-rtems4.10-windmc%{_exeext}; then 
    rm -f ${RPM_BUILD_ROOT}%{_mandir}/man1/lm32-rtems4.10-windmc*
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

%clean
  rm -rf $RPM_BUILD_ROOT

# ==============================================================
# lm32-rtems4.10-binutils
# ==============================================================
# %package -n rtems-4.10-lm32-rtems4.10-binutils
# Summary:      rtems binutils for lm32-rtems4.10
# Group: Development/Tools
# %if %build_infos
# Requires: rtems-4.10-binutils-common
# %endif

%description -n rtems-4.10-lm32-rtems4.10-binutils
GNU binutils targetting lm32-rtems4.10.

%files -n rtems-4.10-lm32-rtems4.10-binutils
%defattr(-,root,root)
%dir %{_prefix}
%dir %{_datarootdir}

%dir %{_mandir}
%dir %{_mandir}/man1
%{_mandir}/man1/lm32-rtems4.10-*.1*

%dir %{_bindir}
%{_bindir}/lm32-rtems4.10-*

%dir %{_exec_prefix}/lm32-rtems4.10
%dir %{_exec_prefix}/lm32-rtems4.10/bin
%{_exec_prefix}/lm32-rtems4.10/bin/*

%dir %{_exec_prefix}/lm32-rtems4.10/lib
%{_exec_prefix}/lm32-rtems4.10/lib/ldscripts
# ==============================================================
# rtems-4.10-binutils-common
# ==============================================================
%package -n rtems-4.10-binutils-common
Summary:      Base package for RTEMS binutils
Group: Development/Tools
%{?_with_noarch_subpackages:BuildArch: noarch}

Requires(post):		/sbin/install-info
Requires(preun):	/sbin/install-info

%description -n rtems-4.10-binutils-common

RTEMS is an open source operating system for embedded systems.

This is the base for binutils regardless of target CPU.

%post -n rtems-4.10-binutils-common
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/as.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/bfd.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/binutils.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/ld.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/standards.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/gprof.info.gz || :

%preun -n rtems-4.10-binutils-common
if [ $1 -eq 0 ]; then
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/as.info.gz || :
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/bfd.info.gz || :
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/binutils.info.gz || :
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/ld.info.gz || :
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/standards.info.gz || :
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/gprof.info.gz || :
fi

%files -n rtems-4.10-binutils-common
%defattr(-,root,root)
%dir %{_prefix}
%dir %{_infodir}
%ghost %{_infodir}/dir
%{_infodir}/as.info*
%{_infodir}/bfd.info*
%{_infodir}/binutils.info*
%{_infodir}/ld.info*
%{_infodir}/standards.info*
%{_infodir}/gprof.info*

%dir %{_prefix}/share
%dir %{_prefix}/share/locale

