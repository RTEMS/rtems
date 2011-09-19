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

%define gdb_version 7.2
%define gdb_rpmvers %{expand:%(echo 7.2 | tr - _)} 

Name:		rtems-4.10-mipstx39-rtems4.10-gdb
Summary:	Gdb for target mipstx39-rtems4.10
Group:		Development/Tools
Version:	%{gdb_rpmvers}
Release:	2%{?dist}
License:	GPL/LGPL
URL: 		http://sources.redhat.com/gdb
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:  %{_host_rpmprefix}gcc

%global build_sim --enable-sim

# Whether to build against system readline
# Default: yes
%bcond_without system_readline

# Whether to build python support
%if "%{_build}" != "%{_host}"
# Can't build python Cdn-X
%bcond_with python
%else
%bcond_without python
%endif
%{?with_python:BuildRequires: %{_host_rpmprefix}python-devel}

%global build_sim --enable-sim --enable-sim-trace
%ifos mingw mingw32
# Mingw lacks functions required by the mipstx39 simulator
%define build_sim --disable-sim
%endif

# suse
%if "%{?suse}" >= "10.3"
BuildRequires: libexpat-devel
%else
# Fedora/CentOS/Cygwin/MinGW
BuildRequires: %{_host_rpmprefix}expat-devel
%endif

%{?with_system_readline:BuildRequires: %{_host_rpmprefix}readline-devel}
BuildRequires:  %{_host_rpmprefix}ncurses-devel

# Required for building the infos
BuildRequires:	/sbin/install-info
BuildRequires:	texinfo >= 4.2
Requires:	rtems-4.10-gdb-common

%if "%{gdb_version}" == "7.2"
Source0: ftp://ftp.gnu.org/gnu/gdb/gdb-7.2.tar.bz2
Patch0: ftp://ftp.rtems.org/pub/rtems/SOURCES/4.11/gdb-7.2-rtems4.10-20100903.diff
%endif
%if "%{gdb_version}" == "7.1"
Source0: ftp://ftp.gnu.org/gnu/gdb/gdb-7.1.tar.bz2
Patch0: ftp://ftp.rtems.org/pub/rtems/SOURCES/4.10/gdb-7.1-rtems4.10-20100812.diff
%endif


%description
GDB for target mipstx39-rtems4.10
%prep
%setup -q -c -T -n %{name}-%{version}

%setup -q -D -T -n %{name}-%{version} -a0
cd gdb-%{gdb_version}
%{?PATCH0:%patch0 -p1}
cd ..

# Force using a system-provided libreadline
%{?with_system_readline:rm -f gdb-%{gdb_version}/readline/configure}
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
    --target=mipstx39-rtems4.10 \
    --verbose --disable-nls \
    --without-included-gettext \
    --disable-win32-registry \
    --disable-werror \
    %{build_sim} \
    %{?with_system_readline:--with-system-readline} \
    --with-expat \
    %{?with_python:--with-python}%{!?with_python:--without-python} \
    --with-gdb-datadir=%{_datadir}/mipstx39-rtems4.10-gdb \
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
  rm -f ${RPM_BUILD_ROOT}%{_libdir}/libmipstx39-rtems4.10-sim.a

# Bug in gdb-7.0, bogusly installs linux-only files
  somethinguseful=0
  for f in ${RPM_BUILD_ROOT}%{_datadir}/mipstx39-rtems4.10-gdb/syscalls/*.xml; do
    case $f in
    *linux.xml) rm -f $f;;
    *.xml) somethinguseful=1;;
    esac
  done
  if test $somethinguseful -eq 0; then
    rm -rf "${RPM_BUILD_ROOT}%{_datadir}/mipstx39-rtems4.10-gdb/syscalls"
  fi

%if "{gdb_version}" >= "7.3"
%if ! %{with python}
# gdb-7.3 doesn't honor --without-python correctly
  rm -rf ${RPM_BUILD_ROOT}%{_datadir}/mipstx39-rtems4.10-gdb/python
%endif
%endif
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
# mipstx39-rtems4.10-gdb
# ==============================================================
# %package -n rtems-4.10-mipstx39-rtems4.10-gdb
# Summary:      rtems gdb for mipstx39-rtems4.10
# Group: Development/Tools
# %if %build_infos
# Requires: rtems-4.10-gdb-common
# %endif

%description -n rtems-4.10-mipstx39-rtems4.10-gdb
GNU gdb targetting mipstx39-rtems4.10.

%files -n rtems-4.10-mipstx39-rtems4.10-gdb
%defattr(-,root,root)
%dir %{_prefix}
%dir %{_prefix}/share
%{?with_python:%{_datadir}/mipstx39-rtems4.10-gdb}

%dir %{_mandir}
%dir %{_mandir}/man1
%{_mandir}/man1/mipstx39-rtems4.10-*.1*

%dir %{_bindir}
%{_bindir}/mipstx39-rtems4.10-*

# ==============================================================
# rtems-4.10-gdb-common
# ==============================================================
%package -n rtems-4.10-gdb-common
Summary:      Base package for RTEMS gdbs
Group: Development/Tools
Requires(post):		/sbin/install-info
Requires(preun):	/sbin/install-info
%{?_with_noarch_subpackages:BuildArch: noarch}

%description -n rtems-4.10-gdb-common

GDB files shared by all targets.

%post -n rtems-4.10-gdb-common
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/gdb.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/gdbint.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/stabs.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/annotate.info.gz || :

%preun -n rtems-4.10-gdb-common
if [ $1 -eq 0 ]; then
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/gdb.info.gz || :
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/gdbint.info.gz || :
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/stabs.info.gz || :
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/annotate.info.gz || :
fi

%files -n rtems-4.10-gdb-common
%defattr(-,root,root)
%dir %{_prefix}
%dir %{_prefix}/share

%dir %{_infodir}
%ghost %{_infodir}/dir
%{_infodir}/gdb.info*

%{_infodir}/gdbint.info*
%{_infodir}/stabs.info*
%{_infodir}/annotate.info*

