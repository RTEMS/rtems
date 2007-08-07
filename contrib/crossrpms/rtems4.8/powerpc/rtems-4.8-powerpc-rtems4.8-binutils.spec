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

%define binutils_pkgvers 2.17.90
%define binutils_version 2.17.90
%define binutils_rpmvers %{expand:%(echo "2.17.90" | tr - _ )}

Name:		rtems-4.8-powerpc-rtems4.8-binutils
Summary:	Binutils for target powerpc-rtems4.8
Group:		Development/Tools
Version:	%{binutils_rpmvers}
Release:	2%{?dist}
License:	GPL/LGPL
URL: 		http://sources.redhat.com/binutils
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

# Required for building the infos
BuildRequires:	/sbin/install-info
BuildRequires:	texinfo >= 4.2
BuildRequires:	flex
BuildRequires:	bison

Requires:	rtems-4.8-binutils-common

Source0:	ftp://ftp.gnu.org/pub/gnu/binutils/binutils-%{binutils_pkgvers}.tar.bz2
%{?_without_sources:NoSource:	0}
%if "%{binutils_version}" == "2.17"
Patch0:		binutils-2.17-rtems4.8-20061021.diff
%endif
%if "%{binutils_version}" == "2.17.90"
Patch0:		binutils-2.17.90-rtems4.8-20070807.diff
%endif

%description
Cross binutils for target powerpc-rtems4.8
%prep
%setup -q -c -T -n %{name}-%{version}

%setup -q -D -T -n %{name}-%{version} -a0
cd binutils-%{binutils_pkgvers}
%{?PATCH0:%patch0 -p1}
cd ..

%build
%if "powerpc-rtems4.8" == "i686-pc-cygwin"
# The cygwin sources are leaking memory
  RPM_OPT_FLAGS="$(echo "$RPM_OPT_FLAGS"|sed -e 's; -Wp,-D_FORTIFY_SOURCE=2;;')"
%endif
  mkdir -p build
  cd build
  CFLAGS="$RPM_OPT_FLAGS" \
  ../binutils-%{binutils_pkgvers}/configure \
    --build=%_build --host=%_host \
    --target=powerpc-rtems4.8 \
    --verbose --disable-nls \
    --without-included-gettext \
    --disable-win32-registry \
    --disable-werror \
    --prefix=%{_prefix} --bindir=%{_bindir} \
    --exec-prefix=%{_exec_prefix} \
    --includedir=%{_includedir} --libdir=%{_libdir} \
    --mandir=%{_mandir} --infodir=%{_infodir}

  make all
  make info
  cd ..

%install
  rm -rf $RPM_BUILD_ROOT

  cd build
  make prefix=$RPM_BUILD_ROOT%{_prefix} \
    bindir=$RPM_BUILD_ROOT%{_bindir} \
    includedir=$RPM_BUILD_ROOT%{_includedir} \
    libdir=$RPM_BUILD_ROOT%{_libdir} \
    infodir=$RPM_BUILD_ROOT%{_infodir} \
    mandir=$RPM_BUILD_ROOT%{_mandir} \
    exec_prefix=$RPM_BUILD_ROOT%{_exec_prefix} \
    install

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
  if test ! -f ${RPM_BUILD_ROOT}%{_bindir}/powerpc-rtems4.8-dlltool%{_exeext}; then 
    rm -f ${RPM_BUILD_ROOT}%{_mandir}/man1/powerpc-rtems4.8-dlltool*
  fi
  if test ! -f ${RPM_BUILD_ROOT}%{_bindir}/powerpc-rtems4.8-nlmconv%{_exeext}; then 
    rm -f ${RPM_BUILD_ROOT}%{_mandir}/man1/powerpc-rtems4.8-nlmconv*
  fi
  if test ! -f ${RPM_BUILD_ROOT}%{_bindir}/powerpc-rtems4.8-windres%{_exeext}; then 
    rm -f ${RPM_BUILD_ROOT}%{_mandir}/man1/powerpc-rtems4.8-windres*
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
# powerpc-rtems4.8-binutils
# ==============================================================
# %package -n rtems-4.8-powerpc-rtems4.8-binutils
# Summary:      rtems binutils for powerpc-rtems4.8
# Group: Development/Tools
# %if %build_infos
# Requires: rtems-4.8-binutils-common
# %endif

%description -n rtems-4.8-powerpc-rtems4.8-binutils

GNU binutils targetting powerpc-rtems4.8.

%files -n rtems-4.8-powerpc-rtems4.8-binutils
%defattr(-,root,root)
%dir %{_mandir}
%dir %{_mandir}/man1
%{_mandir}/man1/powerpc-rtems4.8-*.1*

%dir %{_bindir}
%{_bindir}/powerpc-rtems4.8-*

%dir %{_exec_prefix}/powerpc-rtems4.8
%dir %{_exec_prefix}/powerpc-rtems4.8/bin
%{_exec_prefix}/powerpc-rtems4.8/bin/*

%dir %{_exec_prefix}/powerpc-rtems4.8/lib
%{_exec_prefix}/powerpc-rtems4.8/lib/ldscripts
# ==============================================================
# rtems-4.8-binutils-common
# ==============================================================
%package -n rtems-4.8-binutils-common
Summary:      Base package for RTEMS binutils
Group: Development/Tools
Requires(post):		/sbin/install-info
Requires(preun):	/sbin/install-info

Provides:	rtems-4.8-rtems4.7-base-binutils = %{binutils_version}-%{release}
Obsoletes:	rtems-4.8-rtems4.7-base-binutils < %{binutils_version}-%{release}
Provides:	rtems-4.8-rtems-base-binutils = %{binutils_version}-%{release}
Obsoletes:	rtems-4.8-rtems-base-binutils < %{binutils_version}-%{release}

%description -n rtems-4.8-binutils-common

RTEMS is an open source operating system for embedded systems.

This is the base for binutils regardless of target CPU.

%post -n rtems-4.8-binutils-common
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/as.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/bfd.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/binutils.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/ld.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/standards.info.gz || :
%if "%{binutils_version}" > "2.17"
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/gprof.info.gz || :
%endif
%if "%{binutils_version}" < "2.13"
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/gasp.info.gz || :
%endif

%preun -n rtems-4.8-binutils-common
if [ $1 -eq 0 ]; then
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/as.info.gz || :
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/bfd.info.gz || :
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/binutils.info.gz || :
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/ld.info.gz || :
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/standards.info.gz || :
%if "%{binutils_version}" > "2.17"
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/gprof.info.gz || :
%endif
%if "%{binutils_version}" < "2.13"
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/gasp.info.gz || :
%endif
fi

%files -n rtems-4.8-binutils-common
%defattr(-,root,root)
%dir %{_infodir}
%ghost %{_infodir}/dir
%{_infodir}/as.info*
%{_infodir}/bfd.info*
%{_infodir}/binutils.info*
%{_infodir}/ld.info*
%{_infodir}/standards.info*
%if "%{binutils_version}" > "2.17"
%{_infodir}/gprof.info*
%endif
%if "%{binutils_version}" < "2.13"
%{_infodir}/gasp.info*
%endif

%dir %{_prefix}/share
%dir %{_prefix}/share/locale

