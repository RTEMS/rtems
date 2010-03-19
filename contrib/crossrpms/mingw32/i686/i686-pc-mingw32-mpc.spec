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

%define mpc_version 0.8.1
%define mpc_rpmvers %{expand:%(echo 0.8.1 | tr - _)} 
%define debug_package %{nil}

Name:         i686-pc-mingw32-mpc
Release:      0.20100319.1%{?dist}
License:      GPL
Group:        Development/Tools

BuildArch:	noarch
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

Version:      	%mpc_rpmvers
Summary:      	MinGW mpc Libraries

Source0:	ftp://ftp.gnu.org/gnu/mpc/mpc-%{mpc_version}.tar.gz

Provides:	i686-pc-mingw32-mpc-devel = %{version}-%{release}
Provides:	i686-pc-mingw32-libmpc-devel = %{version}-%{release}

%define _mingw32_target          i686-pc-mingw32

# Fedora ships a mingw toolchain installed to /usr
%define _mingw32_sysroot /usr/%{_mingw32_target}/sys-root

BuildRequires:  m4
BuildRequires:  i686-pc-mingw32-gmp-devel
BuildRequires:  i686-pc-mingw32-mpfr-devel
BuildRequires:	i686-pc-mingw32-gcc

%description
MinGW mpc libraries.

%prep
%setup -c -q

%build
  %if "%{_prefix}" != "/usr"
    export PATH="%{_bindir}:${PATH}"
  %endif

  mkdir -p build

  cd build

  ../mpc-%{mpc_version}/configure \
    --prefix=%{_mingw32_sysroot}/mingw \
    --bindir=%{_bindir} \
    --exec_prefix=%{_mingw32_sysroot}/mingw \
    --includedir=%{_mingw32_sysroot}/mingw/include \
    --libdir=%{_mingw32_sysroot}/mingw/lib \
    --libexecdir=%{_mingw32_sysroot}/mingw/libexec \
    --mandir=%{_mingw32_sysroot}/mingw/share/man \
    --infodir=%{_mingw32_sysroot}/mingw/share/info \
    --datadir=%{_mingw32_sysroot}/mingw/share \
    --build=%_build --host=i686-pc-mingw32 \
    --enable-static \
    --disable-shared

  cd ..

%install
  rm -rf $RPM_BUILD_ROOT

  cd build

  make DESTDIR=$RPM_BUILD_ROOT install

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


cat << EOF > %{_builddir}/%{name}-%{mpc_rpmvers}/find-provides
#!/bin/sh
grep -E -v '^${RPM_BUILD_ROOT}%{_exec_prefix}/i686-pc-mingw32/(lib|include|sys-root)' \
  %{?_gcclibdir:| grep -v '^${RPM_BUILD_ROOT}%{_gcclibdir}/gcc/i686-pc-mingw32/'} | %__find_provides
EOF
chmod +x %{_builddir}/%{name}-%{mpc_rpmvers}/find-provides
%define __find_provides %{_builddir}/%{name}-%{mpc_rpmvers}/find-provides

cat << EOF > %{_builddir}/%{name}-%{mpc_rpmvers}/find-requires
#!/bin/sh
grep -E -v '^${RPM_BUILD_ROOT}%{_exec_prefix}/i686-pc-mingw32/(lib|include|sys-root)' \
  %{?_gcclibdir:| grep -v '^${RPM_BUILD_ROOT}%{_gcclibdir}/gcc/i686-pc-mingw32/'} | %__find_requires
EOF
chmod +x %{_builddir}/%{name}-%{mpc_rpmvers}/find-requires
%define __find_requires %{_builddir}/%{name}-%{mpc_rpmvers}/find-requires

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

%files
%defattr(-,root,root,-)
%{_mingw32_sysroot}/mingw
%exclude %{_mingw32_sysroot}/mingw/share/info/dir
