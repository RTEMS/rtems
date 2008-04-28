#
# Please send bugfixes or comments to
# 	http://www.rtems.org/bugzilla
#

%define _prefix			/opt/rtems-4.9
%define _infodir		%{_prefix}/info
%define _mandir			%{_prefix}/man

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
%define _host_rpmprefix rtems-4.9-%{_host}-
%else
%define _host_rpmprefix %{nil}
%endif

%define gdb_version 6.8
%define gdb_rpmvers %{expand:%(echo 6.8 | tr - _)} 

Name:		rtems-4.9-i386-rtems4.9-gdb
Summary:	Gdb for target i386-rtems4.9
Group:		Development/Tools
Version:	%{gdb_rpmvers}
Release:	2%{?dist}
License:	GPL/LGPL
URL: 		http://sources.redhat.com/gdb
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:  %{_host_rpmprefix}gcc

%if "%{gdb_version}" >= "6.6"
# suse
%if "%{?suse}"
%if "%{?suse}" >= "10.3"
BuildRequires: libexpat-devel
%else
BuildRequires: expat
%endif
%else
# fedora/redhat/cygwin
BuildRequires: %{_host_rpmprefix}expat-devel
%endif
%endif

%if "%{gdb_version}" < "6.6"
%if "%{_build}" != "%{_host}"
BuildRequires:  %{_host_rpmprefix}termcap-devel
%endif
%endif
BuildRequires:  %{_host_rpmprefix}readline-devel
BuildRequires:  %{_host_rpmprefix}ncurses-devel

# Required for building the infos
BuildRequires:	/sbin/install-info
BuildRequires:	texinfo >= 4.2
%if "i386-rtems4.9" == "sparc-rtems4.9"
BuildConflicts:	libtermcap-devel termcap-devel
%endif


Requires:	rtems-4.9-gdb-common

Source0:	ftp://ftp.gnu.org/pub/gnu/gdb/gdb-%{gdb_version}.tar.bz2
%{?_without_sources:NoSource:	0}
%if "%{gdb_version}" == "6.8"
Patch0:		gdb-6.8-rtems4.9-20080428.diff
%endif

%description
GDB for target i386-rtems4.9
%prep
%setup -q -c -T -n %{name}-%{version}

%setup -q -D -T -n %{name}-%{version} -a0
cd gdb-%{gdb_version}
%{?PATCH0:%patch0 -p1}
%{?PATCH1:%patch1 -p1}
cd ..

%if "%{gdb_version}" >= "6.8"
# Force using a system-provided libreadline
rm -f gdb-%{gdb_version}/readline/configure
%endif
%build
  export PATH="%{_bindir}:${PATH}"
  mkdir -p build
  cd build
%if "%{_build}" != "%{_host}"
  CFLAGS_FOR_BUILD="-g -O2 -Wall" \
%endif
  CFLAGS="$RPM_OPT_FLAGS" \
  ../gdb-%{gdb_version}/configure \
    --build=%_build --host=%_host \
    --target=i386-rtems4.9 \
    --verbose --disable-nls \
    --without-included-gettext \
    --disable-win32-registry \
    --disable-werror \
    --enable-sim \
%if "%{gdb_version}" >= "6.6"
    --with-system-readline \
    --with-expat \
%endif
    --prefix=%{_prefix} --bindir=%{_bindir} \
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

  rm -f $RPM_BUILD_ROOT%{_infodir}/dir
  touch $RPM_BUILD_ROOT%{_infodir}/dir

# These come from other packages
  rm -rf $RPM_BUILD_ROOT%{_infodir}/bfd*
  rm -rf $RPM_BUILD_ROOT%{_infodir}/configure*
  rm -rf $RPM_BUILD_ROOT%{_infodir}/standards*

# We don't ship host files
  rm -f ${RPM_BUILD_ROOT}%{_libdir}/libiberty*

# host library, installed to a bogus directory
  rm -f ${RPM_BUILD_ROOT}%{_libdir}/libi386-rtems4.9-sim.a

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
# i386-rtems4.9-gdb
# ==============================================================
# %package -n rtems-4.9-i386-rtems4.9-gdb
# Summary:      rtems gdb for i386-rtems4.9
# Group: Development/Tools
# %if %build_infos
# Requires: rtems-4.9-gdb-common
# %endif

%description -n rtems-4.9-i386-rtems4.9-gdb
GNU gdb targetting i386-rtems4.9.

%files -n rtems-4.9-i386-rtems4.9-gdb
%defattr(-,root,root)
%dir %{_mandir}
%dir %{_mandir}/man1
%{_mandir}/man1/i386-rtems4.9-*.1*

%dir %{_bindir}
%{_bindir}/i386-rtems4.9-*

# ==============================================================
# rtems-4.9-gdb-common
# ==============================================================
%package -n rtems-4.9-gdb-common
Summary:      Base package for RTEMS gdbs
Group: Development/Tools
Requires(post):		/sbin/install-info
Requires(preun):	/sbin/install-info

%description -n rtems-4.9-gdb-common

GDB files shared by all targets.

%post -n rtems-4.9-gdb-common
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/gdb.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/gdbint.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/stabs.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/annotate.info.gz || :

%preun -n rtems-4.9-gdb-common
if [ $1 -eq 0 ]; then
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/gdb.info.gz || :
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/gdbint.info.gz || :
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/stabs.info.gz || :
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/annotate.info.gz || :
fi

%files -n rtems-4.9-gdb-common
%defattr(-,root,root)
%dir %{_infodir}
%ghost %{_infodir}/dir
%{_infodir}/gdb.info*

%{_infodir}/gdbint.info*
%{_infodir}/stabs.info*
%{_infodir}/annotate.info*

