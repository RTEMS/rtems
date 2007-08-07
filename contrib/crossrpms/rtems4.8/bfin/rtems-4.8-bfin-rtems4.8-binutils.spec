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

Name:		rtems-4.8-bfin-rtems4.8-binutils
Summary:	Binutils for target bfin-rtems4.8
Group:		Development/Tools
Version:	%{binutils_rpmvers}
Release:	1%{?dist}
License:	GPL/LGPL
URL: 		http://sources.redhat.com/binutils
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

%if "bfin-rtems4.8" == "i686-pc-cygwin"
BuildRequires:	texinfo >= 4.2
%endif
BuildRequires:	flex
BuildRequires:	bison

Source0:	ftp://ftp.gnu.org/pub/gnu/binutils/binutils-%{binutils_pkgvers}.tar.bz2
%{?_without_sources:NoSource:	0}
%if "%{binutils_version}" == "2.17"
Patch0:		binutils-2.17-rtems4.8-20061021.diff
%endif
%if "%{binutils_version}" == "2.17.90"
Patch0:		binutils-2.17.90-rtems4.8-20070807.diff
%endif

%description
Cross binutils for target bfin-rtems4.8
%prep
%setup -q -c -T -n %{name}-%{version}

%setup -q -D -T -n %{name}-%{version} -a0
cd binutils-%{binutils_pkgvers}
%{?PATCH0:%patch0 -p1}
cd ..

%build
%if "bfin-rtems4.8" == "i686-pc-cygwin"
# The cygwin sources are leaking memory
  RPM_OPT_FLAGS="$(echo "$RPM_OPT_FLAGS"|sed -e 's; -Wp,-D_FORTIFY_SOURCE=2;;')"
%endif
  mkdir -p build
  cd build
  CFLAGS="$RPM_OPT_FLAGS" \
  ../binutils-%{binutils_pkgvers}/configure \
    --build=%_build --host=%_host \
    --target=bfin-rtems4.8 \
    --verbose --disable-nls \
    --without-included-gettext \
    --disable-win32-registry \
    --disable-werror \
    --prefix=%{_prefix} --bindir=%{_bindir} \
    --exec-prefix=%{_exec_prefix} \
    --includedir=%{_includedir} --libdir=%{_libdir} \
    --mandir=%{_mandir} --infodir=%{_infodir}

  make all
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


# Conflict with a native binutils' infos
  rm -rf $RPM_BUILD_ROOT%{_infodir}

# We don't ship host files
  rm -f ${RPM_BUILD_ROOT}%{_libdir}/libiberty*

# manpages without corresponding tools
  if test ! -f ${RPM_BUILD_ROOT}%{_bindir}/bfin-rtems4.8-dlltool%{_exeext}; then 
    rm -f ${RPM_BUILD_ROOT}%{_mandir}/man1/bfin-rtems4.8-dlltool*
  fi
  if test ! -f ${RPM_BUILD_ROOT}%{_bindir}/bfin-rtems4.8-nlmconv%{_exeext}; then 
    rm -f ${RPM_BUILD_ROOT}%{_mandir}/man1/bfin-rtems4.8-nlmconv*
  fi
  if test ! -f ${RPM_BUILD_ROOT}%{_bindir}/bfin-rtems4.8-windres%{_exeext}; then 
    rm -f ${RPM_BUILD_ROOT}%{_mandir}/man1/bfin-rtems4.8-windres*
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
# bfin-rtems4.8-binutils
# ==============================================================
# %package -n rtems-4.8-bfin-rtems4.8-binutils
# Summary:      rtems binutils for bfin-rtems4.8
# Group: Development/Tools
# %if %build_infos
# Requires: rtems-4.8-binutils-common
# %endif

%description -n rtems-4.8-bfin-rtems4.8-binutils

GNU binutils targetting bfin-rtems4.8.

%files -n rtems-4.8-bfin-rtems4.8-binutils
%defattr(-,root,root)
%dir %{_mandir}
%dir %{_mandir}/man1
%{_mandir}/man1/bfin-rtems4.8-*.1*

%dir %{_bindir}
%{_bindir}/bfin-rtems4.8-*

%dir %{_exec_prefix}/bfin-rtems4.8
%dir %{_exec_prefix}/bfin-rtems4.8/bin
%{_exec_prefix}/bfin-rtems4.8/bin/*

%dir %{_exec_prefix}/bfin-rtems4.8/lib
%{_exec_prefix}/bfin-rtems4.8/lib/ldscripts

