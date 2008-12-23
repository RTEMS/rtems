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
%define debug_package		%{nil}
%else
%define _exeext %{nil}
%endif

%ifos cygwin cygwin32
%define optflags -O3 -pipe -march=i486 -funroll-loops
%define _libdir			%{_exec_prefix}/lib
%endif

%if "%{_build}" != "%{_host}"
%define _host_rpmprefix rtems-4.10-%{_host}-
%else
%define _host_rpmprefix %{nil}
%endif


%define cpukit_pkgvers 4.7.99.1-20070510
%define cpukit_version 4.7.99.1
%define cpukit_rpmvers %{expand:%(echo "4.7.99.1" | tr - _ )}

Name:         	rtems-4.10-mips-rtems4.10-cpukit
Summary:      	mips-rtems4.10 cpukit

Group:	      	Development/Tools
Version:        %{cpukit_rpmvers}
Release:      	1%{?dist}%{?dist}
License:      	GPL
URL:		http://cpukit.gnu.org
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildArch:	noarch

%define _use_internal_dependency_generator 0

BuildRequires:	rtems-4.10-mips-rtems4.10-gcc

Source0: 	ftp://ftp.rtems.org/pub/rtems/SOURCES/rtems-%{cpukit_pkgvers}.tar.bz2
%{?_without_sources:NoSource:   0}

%description
RTEMS cpukit for mips-rtems4.10.
%prep
%setup -c -T -n %{name}-%{version}

%setup -q -T -D -n %{name}-%{version} -a0


%build
  export PATH="%{_bindir}:${PATH}"
  mkdir -p build

  cd build
  ../rtems-%{cpukit_pkgvers}/configure \
    --prefix=%{_prefix} \
    --target=mips-rtems4.10 \
    --enable-multilib \
    --disable-rtemsbsp

  make all RTEMS_BSP=
  cd ..

%install
  rm -rf $RPM_BUILD_ROOT
  export PATH="%{_bindir}:${PATH}"
  cd build
  make DESTDIR=$RPM_BUILD_ROOT install
  cd ..

  rm -rf $RPM_BUILD_ROOT%{_prefix}/share/man
  rm -rf $RPM_BUILD_ROOT%{_prefix}/bin
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


cat << EOF > %{_builddir}/%{name}-%{cpukit_rpmvers}/find-provides
#!/bin/sh
grep -E -v '^${RPM_BUILD_ROOT}%{_exec_prefix}/mips-rtems4.10/(lib|include|sys-root)' \
  | grep -v '^${RPM_BUILD_ROOT}%{cpukitlib}/mips-rtems4.10/' | %__find_provides
EOF
chmod +x %{_builddir}/%{name}-%{cpukit_rpmvers}/find-provides
%define __find_provides %{_builddir}/%{name}-%{cpukit_rpmvers}/find-provides

cat << EOF > %{_builddir}/%{name}-%{cpukit_rpmvers}/find-requires
#!/bin/sh
grep -E -v '^${RPM_BUILD_ROOT}%{_exec_prefix}/mips-rtems4.10/(lib|include|sys-root)' \
  | grep -v '^${RPM_BUILD_ROOT}%{cpukitlib}/mips-rtems4.10/' | %__find_requires
EOF
chmod +x %{_builddir}/%{name}-%{cpukit_rpmvers}/find-requires
%define __find_requires %{_builddir}/%{name}-%{cpukit_rpmvers}/find-requires

%clean
  rm -rf $RPM_BUILD_ROOT

%description -n rtems-4.10-mips-rtems4.10-cpukit
GNU cc compiler for mips-rtems4.10.

%files -n rtems-4.10-mips-rtems4.10-cpukit
%dir %{_prefix}
%{_prefix}/mips-rtems4.10
# Violates the FHS
%exclude %{_prefix}/make
